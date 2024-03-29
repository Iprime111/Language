#include "Dump.h"
#include "Logger.h"
#include "NameTable.h"
#include "SyntaxTree.h"

static CompilationError EmitDumpHeader      (CompilationContext *context, Buffer <char> *dumpBuffer);
static CompilationError EmitNodeData        (CompilationContext *context, Tree::Node <AstNode> *node, Buffer <char> *dumpBuffer);
static CompilationError EmitNode            (CompilationContext *context, Tree::Node <AstNode> *node, Buffer <char> *dumpBuffer);
static const  char     *GetOutlineColor     (CompilationContext *context, Tree::Node <AstNode> *node);
static const  char     *GetBackgroundColor  (CompilationContext *context, Tree::Node <AstNode> *node);
static CompilationError EmitNodeConnections (Tree::Node <AstNode> *node, Buffer <char> *dumpBuffer);

#define WriteToDumpWithErrorCheck(dumpBuffer, data)                                                     \
    do {                                                                                                \
        if (WriteDataToBuffer (dumpBuffer, data, strlen (data)) != BufferErrorCode::NO_BUFFER_ERRORS) { \
            RETURN CompilationError::DUMP_ERROR;                                                        \
        }                                                                                               \
    } while (0)

CompilationError DumpSyntaxTree (CompilationContext *context, char *dumpFilename) {
    PushLog (3);

    custom_assert (context, pointer_is_null, CompilationError::CONTEXT_ERROR);

    Buffer <char> dumpBuffer = {};

    #define CatchError(returnValue, ...)    \
        do {                                \
            if (!(__VA_ARGS__)) {           \
                RETURN returnValue;         \
            }                               \
        } while (0)

    CatchError (CompilationError::DUMP_ERROR, InitBuffer (&dumpBuffer) == BufferErrorCode::NO_BUFFER_ERRORS);
    CatchError (CompilationError::DUMP_ERROR, EmitDumpHeader (context, &dumpBuffer) == CompilationError::NO_ERRORS);

    if (context->abstractSyntaxTree.root) {
        CatchError (CompilationError::DUMP_ERROR, EmitNode (context, context->abstractSyntaxTree.root, &dumpBuffer) == CompilationError::NO_ERRORS);
    }

    WriteToDumpWithErrorCheck (&dumpBuffer, "}");

    FILE *dumpFile = fopen (dumpFilename, "w");
    CatchError (CompilationError::DUMP_ERROR, dumpFile != NULL);

    fwrite (dumpBuffer.data, sizeof (char), dumpBuffer.currentIndex, dumpFile);

    fclose (dumpFile);
    CatchError (CompilationError::DUMP_ERROR, DestroyBuffer (&dumpBuffer) == BufferErrorCode::NO_BUFFER_ERRORS);

    #undef CatchError

    RETURN CompilationError::NO_ERRORS;
}

static CompilationError EmitNode (CompilationContext *context, Tree::Node <AstNode> *node, Buffer <char> *dumpBuffer) {
    PushLog (3);

    char indexBuffer [MAX_NODE_INDEX_LENGTH] = "";
    sprintf (indexBuffer, "%lu", (unsigned long) node);

    WriteToDumpWithErrorCheck (dumpBuffer, "\t");
    WriteToDumpWithErrorCheck (dumpBuffer, indexBuffer);
    WriteToDumpWithErrorCheck (dumpBuffer, " [style=\"filled,rounded\" shape=\"record\" fillcolor=\"");

    const char *outlineColor    = GetOutlineColor    (context, node);
    const char *backgroundColor = GetBackgroundColor (context, node); 

    if (!outlineColor || !backgroundColor) {
        RETURN CompilationError::DUMP_ERROR;
    }

    WriteToDumpWithErrorCheck (dumpBuffer, backgroundColor);
    WriteToDumpWithErrorCheck (dumpBuffer, "\" color=\"");
    WriteToDumpWithErrorCheck (dumpBuffer, outlineColor);
    WriteToDumpWithErrorCheck (dumpBuffer, "\" label=\"");

    EmitNodeData (context, node, dumpBuffer);

    WriteToDumpWithErrorCheck (dumpBuffer, "\"]\n");

    EmitNodeConnections (node, dumpBuffer);

    if (node->left)
        EmitNode (context, node->left, dumpBuffer);

    if (node->right)
        EmitNode (context, node->right, dumpBuffer);

    RETURN CompilationError::NO_ERRORS;
}

static CompilationError EmitNodeData (CompilationContext *context, Tree::Node <AstNode> *node, Buffer <char> *dumpBuffer) {
    PushLog (3);

    char indexBuffer [MAX_NODE_INDEX_LENGTH] = "";

    WriteToDumpWithErrorCheck (dumpBuffer, "{ Address: ");
    
    snprintf (indexBuffer, MAX_NODE_INDEX_LENGTH, "%p", node);
    WriteToDumpWithErrorCheck (dumpBuffer, indexBuffer);
    WriteToDumpWithErrorCheck (dumpBuffer, " | Type: ");

    switch (node->nodeData.type) {
        case NodeType::CONSTANT: {
            WriteToDumpWithErrorCheck (dumpBuffer, "Constant |");

            snprintf (indexBuffer, MAX_NODE_INDEX_LENGTH, "%lf", node->nodeData.content.number);
            WriteToDumpWithErrorCheck (dumpBuffer, indexBuffer);

            break;
        }

        case NodeType::STRING: {
            switch (context->nameTable.data [node->nodeData.content.nameTableIndex].type) {
                case NameType::IDENTIFIER:
                    WriteToDumpWithErrorCheck (dumpBuffer, "Identifier"); break;
                case NameType::OPERATOR:
                    WriteToDumpWithErrorCheck (dumpBuffer, "Operator");   break;
                case NameType::TYPE_NAME:
                    WriteToDumpWithErrorCheck (dumpBuffer, "Type name");  break;
                case NameType::SEPARATOR:
                    WriteToDumpWithErrorCheck (dumpBuffer, "Separator");  break;
            };

            WriteToDumpWithErrorCheck (dumpBuffer, " | Value: [");

            const char *name = context->nameTable.data [node->nodeData.content.nameTableIndex].name;

            if (name [0] == '>' || name [0] == '<') {
                WriteToDumpWithErrorCheck (dumpBuffer, "\\");
            }

            WriteToDumpWithErrorCheck (dumpBuffer, name);
            WriteToDumpWithErrorCheck (dumpBuffer, "]");

            break;
        }

        case NodeType::FUNCTION_DEFINITION: {
            WriteToDumpWithErrorCheck (dumpBuffer, "Function definition | Value: [");
            WriteToDumpWithErrorCheck (dumpBuffer, context->nameTable.data [node->nodeData.content.nameTableIndex].name);
            WriteToDumpWithErrorCheck (dumpBuffer, "]");

            break;
        }

        case NodeType::FUNCTION_ARGUMENTS: {
            WriteToDumpWithErrorCheck (dumpBuffer, "Arguments");
            break;
        }

        case NodeType::VARIABLE_DECLARATION: {
            WriteToDumpWithErrorCheck (dumpBuffer, "Variable declaration");
            break;
        }

        case NodeType::FUNCTION_CALL: {
            WriteToDumpWithErrorCheck (dumpBuffer, "Function call");
            break;
        }

        case NodeType::TERMINATOR: {
            WriteToDumpWithErrorCheck (dumpBuffer, "Terminator");
            break;
        }

        default: {
            RETURN CompilationError::NO_ERRORS;
        }
    }

    WriteToDumpWithErrorCheck (dumpBuffer, " | {<left> left: ");
    
    snprintf (indexBuffer, MAX_NODE_INDEX_LENGTH, "%p", node->left);
    WriteToDumpWithErrorCheck (dumpBuffer, indexBuffer);

    WriteToDumpWithErrorCheck (dumpBuffer, " | <right> right: ");

    snprintf (indexBuffer, MAX_NODE_INDEX_LENGTH, "%p", node->right);
    WriteToDumpWithErrorCheck (dumpBuffer, indexBuffer);

    WriteToDumpWithErrorCheck (dumpBuffer, "}}");


    RETURN CompilationError::NO_ERRORS;
}



static CompilationError EmitNodeConnections (Tree::Node <AstNode> *node, Buffer <char> *dumpBuffer) {
    PushLog (3);

    char currentNodeIndex [MAX_NODE_INDEX_LENGTH] = "";
    char indexBuffer      [MAX_NODE_INDEX_LENGTH] = "";

    snprintf (currentNodeIndex, MAX_NODE_INDEX_LENGTH, "%lu", (unsigned long) node);

    #define ChildConnection(child)                                                                      \
        if (node->child) {                                                                              \
            snprintf (indexBuffer, MAX_NODE_INDEX_LENGTH, "%lu", (unsigned long) node->child);          \
            WriteToDumpWithErrorCheck (dumpBuffer, "\t");                                               \
            WriteToDumpWithErrorCheck (dumpBuffer, currentNodeIndex);                                   \
            WriteToDumpWithErrorCheck (dumpBuffer, ":" #child " -> ");                                  \
            WriteToDumpWithErrorCheck (dumpBuffer, indexBuffer);                                        \
            WriteToDumpWithErrorCheck (dumpBuffer, " [color=\"" DUMP_NEXT_CONNECTION_COLOR "\"]\n");    \
        }


    ChildConnection (left);
    ChildConnection (right);

    #undef ChildConnection
    
    RETURN CompilationError::NO_ERRORS;
}

static const char *GetOutlineColor (CompilationContext *context, Tree::Node <AstNode> *node) {
    PushLog (4);

    if (node->nodeData.type == NodeType::CONSTANT) {
            RETURN DUMP_CONSTANT_NODE_OUTLINE_COLOR;

    } else if (node->nodeData.type == NodeType::STRING) {
        if (context->nameTable.data [node->nodeData.content.nameTableIndex].type == NameType::IDENTIFIER) {
            
            RETURN DUMP_IDENTIFIER_NODE_OUTLINE_COLOR;    
        } else {
            
            RETURN DUMP_KEYWORD_NODE_OUTLINE_COLOR;
        }
    } else {
            RETURN DUMP_SERVICE_NODE_OUTLINE_COLOR;
    }

}

static const char *GetBackgroundColor (CompilationContext *context, Tree::Node <AstNode> *node) {
    PushLog (4);

    if (node->nodeData.type == NodeType::CONSTANT) {
            RETURN DUMP_CONSTANT_NODE_BACKGROUND_COLOR;

    } else if (node->nodeData.type == NodeType::STRING) {
        if (context->nameTable.data [node->nodeData.content.nameTableIndex].type == NameType::IDENTIFIER) {

            RETURN DUMP_IDENTIFIER_NODE_BACKGROUND_COLOR;    
        } else {

            RETURN DUMP_KEYWORD_NODE_BACKGROUND_COLOR;
        }
    } else {
            RETURN DUMP_SERVICE_NODE_BACKGROUND_COLOR;
    }

}

static CompilationError EmitDumpHeader (CompilationContext *context, Buffer <char> *dumpBuffer) {
    PushLog (4);

    WriteToDumpWithErrorCheck (dumpBuffer, "digraph {\n\tfontsize=\"20pt\"\n");

    RETURN CompilationError::NO_ERRORS;
}
