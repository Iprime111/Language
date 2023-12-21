#include "Dump.h"
#include "Logger.h"
#include "NameTable.h"
#include "SyntaxTree.h"
#include "TreeDump.h"
#include <cstdio>

static TranslationError EmitDumpHeader      (TranslationContext *context, Buffer <char> *dumpBuffer);
static TranslationError EmitNodeData        (TranslationContext *context, Tree::Node <AstNode> *node, Buffer <char> *dumpBuffer);
static TranslationError EmitNode            (TranslationContext *context, Tree::Node <AstNode> *node, Buffer <char> *dumpBuffer);
static const  char     *GetNodeColor        (TranslationContext *context, Tree::Node <AstNode> *node);
static TranslationError EmitNodeConnections (Tree::Node <AstNode> *node, Buffer <char> *dumpBuffer);

#define WriteToDumpWithErrorCheck(dumpBuffer, data)                                                     \
    do {                                                                                                \
        if (WriteDataToBuffer (dumpBuffer, data, strlen (data)) != BufferErrorCode::NO_BUFFER_ERRORS) { \
            RETURN TranslationError::DUMP_ERROR;                                                        \
        }                                                                                               \
    } while (0)

TranslationError DumpSyntaxTree (TranslationContext *context, char *dumpFilename) {
    PushLog (3);

    custom_assert (context, pointer_is_null, TranslationError::CONTEXT_ERROR);

    Buffer <char> dumpBuffer = {};

    #define CatchError(returnValue, ...)    \
        do {                                \
            if (!(__VA_ARGS__)) {           \
                RETURN returnValue;         \
            }                               \
        } while (0)

    CatchError (TranslationError::DUMP_ERROR, InitBuffer (&dumpBuffer) == BufferErrorCode::NO_BUFFER_ERRORS);
    CatchError (TranslationError::DUMP_ERROR, EmitDumpHeader (context, &dumpBuffer) == TranslationError::NO_ERRORS);

    if (context->abstractSyntaxTree.root) {
        CatchError (TranslationError::DUMP_ERROR, EmitNode (context, context->abstractSyntaxTree.root, &dumpBuffer) == TranslationError::NO_ERRORS);
    }

    WriteToDumpWithErrorCheck (&dumpBuffer, "}");

    FILE *dumpFile = fopen (dumpFilename, "w");
    CatchError (TranslationError::DUMP_ERROR, dumpFile != NULL);

    fwrite (dumpBuffer.data, sizeof (char), dumpBuffer.currentIndex, dumpFile);

    fclose (dumpFile);
    CatchError (TranslationError::DUMP_ERROR, DestroyBuffer (&dumpBuffer) == BufferErrorCode::NO_BUFFER_ERRORS);

    #undef CatchError

    RETURN TranslationError::NO_ERRORS;
}

static TranslationError EmitNode (TranslationContext *context, Tree::Node <AstNode> *node, Buffer <char> *dumpBuffer) {
    PushLog (3);

    char indexBuffer [MAX_NODE_INDEX_LENGTH] = "";
    sprintf (indexBuffer, "%lu", (unsigned long) node);

    WriteToDumpWithErrorCheck (dumpBuffer, "\t");
    WriteToDumpWithErrorCheck (dumpBuffer, indexBuffer);
    WriteToDumpWithErrorCheck (dumpBuffer, " [style=\"filled,rounded\" shape=\"record\" fillcolor=\"" DUMP_NODE_COLOR "\" color=\"");

    const char *nodeColor = GetNodeColor (context, node);
    if (!nodeColor) {
        RETURN TranslationError::DUMP_ERROR;
    }

    WriteToDumpWithErrorCheck (dumpBuffer, nodeColor);
    WriteToDumpWithErrorCheck (dumpBuffer, "\" label=\"{ ");

    EmitNodeData (context, node, dumpBuffer);

    WriteToDumpWithErrorCheck (dumpBuffer, " }\"]\n");

    EmitNodeConnections (node, dumpBuffer);

    if (node->left)
        EmitNode (context, node->left, dumpBuffer);

    if (node->right)
        EmitNode (context, node->right, dumpBuffer);

    RETURN TranslationError::NO_ERRORS;
}

static TranslationError EmitNodeData (TranslationContext *context, Tree::Node <AstNode> *node, Buffer <char> *dumpBuffer) {
    PushLog (3);

    char indexBuffer [MAX_NODE_INDEX_LENGTH] = "";

    switch (node->nodeData.type) {
        case NodeType::CONSTANT: {
            snprintf (indexBuffer, MAX_NODE_INDEX_LENGTH, "%lf", node->nodeData.content.number);

            WriteToDumpWithErrorCheck (dumpBuffer, "{");
            WriteToDumpWithErrorCheck (dumpBuffer, indexBuffer);

            break;
        }

        case NodeType::NAME: {
            WriteToDumpWithErrorCheck (dumpBuffer, "{");
            WriteToDumpWithErrorCheck (dumpBuffer, context->nameTable.data [node->nodeData.content.nameTableIndex].name);

            break;
        }   

        case NodeType::KEYWORD: {
            snprintf (indexBuffer, MAX_NODE_INDEX_LENGTH, "%d", (int) node->nodeData.content.keyword);

            WriteToDumpWithErrorCheck (dumpBuffer, "{");
            WriteToDumpWithErrorCheck (dumpBuffer, indexBuffer);

            break;
        }

        case NodeType::FUNCTION_DEFINITION: {
            WriteToDumpWithErrorCheck (dumpBuffer, "{Function definition: ");
            WriteToDumpWithErrorCheck (dumpBuffer, context->nameTable.data [node->nodeData.content.nameTableIndex].name);

            break;
        }

        case NodeType::FUNCTION_ARGUMENTS: {
            WriteToDumpWithErrorCheck (dumpBuffer, "{Arguments");
            break;
        }

        case NodeType::VARIABLE_DECLARATION: {
            WriteToDumpWithErrorCheck (dumpBuffer, "{Variable declaration");
            break;
        }

        case NodeType::FUNCTION_CALL: {
            WriteToDumpWithErrorCheck (dumpBuffer, "{Function call");
            break;
        }

        case NodeType::TERMINATOR: {
            WriteToDumpWithErrorCheck (dumpBuffer, "{Terminator");
            break;
        }

        default: {
            RETURN TranslationError::NO_ERRORS;
        }
    }

    WriteToDumpWithErrorCheck (dumpBuffer, "}");


    RETURN TranslationError::NO_ERRORS;
}



static TranslationError EmitNodeConnections (Tree::Node <AstNode> *node, Buffer <char> *dumpBuffer) {
    PushLog (3);

    char currentNodeIndex [MAX_NODE_INDEX_LENGTH] = "";
    char indexBuffer      [MAX_NODE_INDEX_LENGTH] = "";

    snprintf (currentNodeIndex, MAX_NODE_INDEX_LENGTH, "%lu", (unsigned long) node);

    #define ChildConnection(child)                                                                      \
        if (node->child) {                                                                              \
            snprintf (indexBuffer, MAX_NODE_INDEX_LENGTH, "%lu", (unsigned long) node->child);          \
            WriteToDumpWithErrorCheck (dumpBuffer, "\t");                                               \
            WriteToDumpWithErrorCheck (dumpBuffer, currentNodeIndex);                                   \
            WriteToDumpWithErrorCheck (dumpBuffer, " -> ");                                             \
            WriteToDumpWithErrorCheck (dumpBuffer, indexBuffer);                                        \
            WriteToDumpWithErrorCheck (dumpBuffer, " [color=\"" DUMP_NEXT_CONNECTION_COLOR "\"]\n");    \
        }


    ChildConnection (left);
    ChildConnection (right);

    #undef ChildConnection
    
    RETURN TranslationError::NO_ERRORS;
}

static const char *GetNodeColor (TranslationContext *context, Tree::Node <AstNode> *node) {
    PushLog (4);

    if (node->nodeData.type == NodeType::CONSTANT) {
        RETURN DUMP_CONSTANT_NODE_OUTLINE_COLOR;
    } else if (node->nodeData.type == NodeType::NAME) {
        if (context->nameTable.data [node->nodeData.content.nameTableIndex].type == NameType::IDENTIFIER) {
            RETURN DUMP_IDENTIFIER_NODE_OUTLINE_COLOR;    
        } else {
            RETURN DUMP_KEYWORD_NODE_OUTLINE_COLOR;
        }
    } else {
       RETURN DUMP_SERVICE_NODE_OUTLINE_COLOR;
    }

}

static TranslationError EmitDumpHeader (TranslationContext *context, Buffer <char> *dumpBuffer) {
    PushLog (4);

    WriteToDumpWithErrorCheck (dumpBuffer, "digraph {\n\tbgcolor=\"" DUMP_BACKGROUND_COLOR "\"\n");

    RETURN TranslationError::NO_ERRORS;
}
