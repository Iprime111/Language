#include <cassert>

#include "Dump.h"
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
            return CompilationError::DUMP_ERROR;                                                        \
        }                                                                                               \
    } while (0)

CompilationError DumpSyntaxTree (CompilationContext *context, char *dumpFilename) {
    assert (context);
    assert (dumpFilename);

    Buffer <char> dumpBuffer = {};

    #define CatchError(returnValue, ...)    \
        do {                                \
            if (!(__VA_ARGS__)) {           \
                return returnValue;         \
            }                               \
        } while (0)

    CatchError (CompilationError::DUMP_ERROR, InitBuffer (&dumpBuffer) == BufferErrorCode::NO_BUFFER_ERRORS);
    CatchError (CompilationError::DUMP_ERROR, EmitDumpHeader (context, &dumpBuffer) == CompilationError::NO_ERRORS);

    if (context->abstractSyntaxTree.root) {
        CatchError (CompilationError::DUMP_ERROR, EmitNode (context, context->abstractSyntaxTree.root, &dumpBuffer) == CompilationError::NO_ERRORS);
    }

    WriteToDumpWithErrorCheck (&dumpBuffer, "}");

    FILE *dumpFile = fopen (dumpFilename, "w");
    CatchError (CompilationError::DUMP_ERROR, dumpFile);

    fwrite (dumpBuffer.data, sizeof (char), dumpBuffer.currentIndex, dumpFile);

    fclose (dumpFile);
    CatchError (CompilationError::DUMP_ERROR, DestroyBuffer (&dumpBuffer) == BufferErrorCode::NO_BUFFER_ERRORS);

    #undef CatchError

    return CompilationError::NO_ERRORS;
}

static CompilationError EmitNode (CompilationContext *context, Tree::Node <AstNode> *node, Buffer <char> *dumpBuffer) {
    assert (context);
    assert (node);
    assert (dumpBuffer);

    char indexBuffer [MAX_NODE_INDEX_LENGTH] = "";
    sprintf (indexBuffer, "%lu", (unsigned long) node);

    WriteToDumpWithErrorCheck (dumpBuffer, "\t");
    WriteToDumpWithErrorCheck (dumpBuffer, indexBuffer);
    WriteToDumpWithErrorCheck (dumpBuffer, " [style=\"filled,rounded\" shape=\"record\" fillcolor=\"");

    const char *outlineColor    = GetOutlineColor    (context, node);
    const char *backgroundColor = GetBackgroundColor (context, node); 

    if (!outlineColor || !backgroundColor)
        return CompilationError::DUMP_ERROR;

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

    return CompilationError::NO_ERRORS;
}

static CompilationError EmitNodeData (CompilationContext *context, Tree::Node <AstNode> *node, Buffer <char> *dumpBuffer) {
    assert (context);
    assert (node);
    assert (dumpBuffer);

    char indexBuffer [MAX_NODE_INDEX_LENGTH] = "";

    WriteToDumpWithErrorCheck (dumpBuffer, "{ Address: ");
    
    snprintf (indexBuffer, MAX_NODE_INDEX_LENGTH, "%p", node);
    WriteToDumpWithErrorCheck (dumpBuffer, indexBuffer);
    WriteToDumpWithErrorCheck (dumpBuffer, " | Type: ");

    switch (node->nodeData.type) {
        case NodeType::CONSTANT:
            WriteToDumpWithErrorCheck (dumpBuffer, "Constant |");

            snprintf (indexBuffer, MAX_NODE_INDEX_LENGTH, "%lf", node->nodeData.content.number);
            WriteToDumpWithErrorCheck (dumpBuffer, indexBuffer);

            break;

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

        case NodeType::FUNCTION_DEFINITION:
            WriteToDumpWithErrorCheck (dumpBuffer, "Function definition | Value: [");
            WriteToDumpWithErrorCheck (dumpBuffer, context->nameTable.data [node->nodeData.content.nameTableIndex].name);
            WriteToDumpWithErrorCheck (dumpBuffer, "]");

            break;

        case NodeType::FUNCTION_ARGUMENTS:
            WriteToDumpWithErrorCheck (dumpBuffer, "Arguments");
            break;

        case NodeType::VARIABLE_DECLARATION:
            WriteToDumpWithErrorCheck (dumpBuffer, "Variable declaration");
            break;

        case NodeType::FUNCTION_CALL:
            WriteToDumpWithErrorCheck (dumpBuffer, "Function call");
            break;

        case NodeType::TERMINATOR:
            WriteToDumpWithErrorCheck (dumpBuffer, "Terminator");
            break;

        default:
            return CompilationError::NO_ERRORS;
    }

    WriteToDumpWithErrorCheck (dumpBuffer, " | {<left> left: ");
    
    snprintf (indexBuffer, MAX_NODE_INDEX_LENGTH, "%p", node->left);
    WriteToDumpWithErrorCheck (dumpBuffer, indexBuffer);

    WriteToDumpWithErrorCheck (dumpBuffer, " | <right> right: ");

    snprintf (indexBuffer, MAX_NODE_INDEX_LENGTH, "%p", node->right);
    WriteToDumpWithErrorCheck (dumpBuffer, indexBuffer);

    WriteToDumpWithErrorCheck (dumpBuffer, "}}");


    return CompilationError::NO_ERRORS;
}



static CompilationError EmitNodeConnections (Tree::Node <AstNode> *node, Buffer <char> *dumpBuffer) {
    assert (node);
    assert (dumpBuffer);

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
    
    return CompilationError::NO_ERRORS;
}

static const char *GetOutlineColor (CompilationContext *context, Tree::Node <AstNode> *node) {
    assert (context);
    assert (node);

    if (node->nodeData.type == NodeType::CONSTANT) {
            return DUMP_CONSTANT_NODE_OUTLINE_COLOR;

    } else if (node->nodeData.type == NodeType::STRING) {
        if (context->nameTable.data [node->nodeData.content.nameTableIndex].type == NameType::IDENTIFIER)
            return DUMP_IDENTIFIER_NODE_OUTLINE_COLOR;    
        else
            return DUMP_KEYWORD_NODE_OUTLINE_COLOR;
    } else {
            return DUMP_SERVICE_NODE_OUTLINE_COLOR;
    }

}

static const char *GetBackgroundColor (CompilationContext *context, Tree::Node <AstNode> *node) {
    assert (context);
    assert (node);

    if (node->nodeData.type == NodeType::CONSTANT) {
            return DUMP_CONSTANT_NODE_BACKGROUND_COLOR;

    } else if (node->nodeData.type == NodeType::STRING) {
        if (context->nameTable.data [node->nodeData.content.nameTableIndex].type == NameType::IDENTIFIER)
            return DUMP_IDENTIFIER_NODE_BACKGROUND_COLOR;    
        else
            return DUMP_KEYWORD_NODE_BACKGROUND_COLOR;
    } else {
            return DUMP_SERVICE_NODE_BACKGROUND_COLOR;
    }

}

static CompilationError EmitDumpHeader (CompilationContext *context, Buffer <char> *dumpBuffer) {
    assert (context);
    assert (dumpBuffer);

    WriteToDumpWithErrorCheck (dumpBuffer, "digraph {\n\tfontsize=\"20pt\"\n");

    return CompilationError::NO_ERRORS;
}
