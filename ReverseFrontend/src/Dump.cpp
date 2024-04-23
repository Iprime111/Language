#include <cassert>
#include <cstdio>

#include "Dump.h"
#include "NameTable.h"
#include "SyntaxTree.h"

static TranslationError EmitDumpHeader      (TranslationContext *context, Buffer <char> *dumpBuffer);
static TranslationError EmitNodeData        (TranslationContext *context, Tree::Node <AstNode> *node, Buffer <char> *dumpBuffer);
static TranslationError EmitNode            (TranslationContext *context, Tree::Node <AstNode> *node, Buffer <char> *dumpBuffer);
static const  char     *GetNodeColor        (TranslationContext *context, Tree::Node <AstNode> *node);
static TranslationError EmitNodeConnections (Tree::Node <AstNode> *node, Buffer <char> *dumpBuffer);

#define WriteToDumpWithErrorCheck(dumpBuffer, data)                                                     \
    do {                                                                                                \
        if (WriteDataToBuffer (dumpBuffer, data, strlen (data)) != BufferErrorCode::NO_BUFFER_ERRORS) { \
            return TranslationError::DUMP_ERROR;                                                        \
        }                                                                                               \
    } while (0)

TranslationError DumpSyntaxTree (TranslationContext *context, char *dumpFilename) {
    assert (context);
    assert (dumpFilename);

    Buffer <char> dumpBuffer = {};

    #define CatchError(returnValue, ...)    \
        do {                                \
            if (!(__VA_ARGS__)) {           \
                return returnValue;         \
            }                               \
        } while (0)

    CatchError (TranslationError::DUMP_ERROR, InitBuffer (&dumpBuffer) == BufferErrorCode::NO_BUFFER_ERRORS);
    CatchError (TranslationError::DUMP_ERROR, EmitDumpHeader (context, &dumpBuffer) == TranslationError::NO_ERRORS);

    if (context->abstractSyntaxTree.root) {
        CatchError (TranslationError::DUMP_ERROR, EmitNode (context, context->abstractSyntaxTree.root, &dumpBuffer) == TranslationError::NO_ERRORS);
    }

    WriteToDumpWithErrorCheck (&dumpBuffer, "}");

    FILE *dumpFile = fopen (dumpFilename, "w");
    CatchError (TranslationError::DUMP_ERROR, dumpFile);

    fwrite (dumpBuffer.data, sizeof (char), dumpBuffer.currentIndex, dumpFile);

    fclose (dumpFile);
    CatchError (TranslationError::DUMP_ERROR, DestroyBuffer (&dumpBuffer) == BufferErrorCode::NO_BUFFER_ERRORS);

    #undef CatchError

    return TranslationError::NO_ERRORS;
}

static TranslationError EmitNode (TranslationContext *context, Tree::Node <AstNode> *node, Buffer <char> *dumpBuffer) {
    assert (context);
    assert (node);
    assert (dumpBuffer);

    char indexBuffer [MAX_NODE_INDEX_LENGTH] = "";
    sprintf (indexBuffer, "%lu", (unsigned long) node);

    WriteToDumpWithErrorCheck (dumpBuffer, "\t");
    WriteToDumpWithErrorCheck (dumpBuffer, indexBuffer);
    WriteToDumpWithErrorCheck (dumpBuffer, " [style=\"filled,rounded\" shape=\"record\" fillcolor=\"" DUMP_NODE_COLOR "\" color=\"");

    const char *nodeColor = GetNodeColor (context, node);
    if (!nodeColor) {
        return TranslationError::DUMP_ERROR;
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

    return TranslationError::NO_ERRORS;
}

static TranslationError EmitNodeData (TranslationContext *context, Tree::Node <AstNode> *node, Buffer <char> *dumpBuffer) {
    assert (context);
    assert (node);
    assert (dumpBuffer);

    char indexBuffer [MAX_NODE_INDEX_LENGTH] = "";

    switch (node->nodeData.type) {
        case NodeType::CONSTANT: {
            snprintf (indexBuffer, MAX_NODE_INDEX_LENGTH, "%lf", node->nodeData.content.number);

            WriteToDumpWithErrorCheck (dumpBuffer, "{");
            WriteToDumpWithErrorCheck (dumpBuffer, indexBuffer);

            break;
        }

        case NodeType::STRING: {
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
            return TranslationError::NO_ERRORS;
        }
    }

    WriteToDumpWithErrorCheck (dumpBuffer, "}");


    return TranslationError::NO_ERRORS;
}



static TranslationError EmitNodeConnections (Tree::Node <AstNode> *node, Buffer <char> *dumpBuffer) {
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
            WriteToDumpWithErrorCheck (dumpBuffer, " -> ");                                             \
            WriteToDumpWithErrorCheck (dumpBuffer, indexBuffer);                                        \
            WriteToDumpWithErrorCheck (dumpBuffer, " [color=\"" DUMP_NEXT_CONNECTION_COLOR "\"]\n");    \
        }


    ChildConnection (left);
    ChildConnection (right);

    #undef ChildConnection
    
    return TranslationError::NO_ERRORS;
}

static const char *GetNodeColor (TranslationContext *context, Tree::Node <AstNode> *node) {
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

static TranslationError EmitDumpHeader (TranslationContext *context, Buffer <char> *dumpBuffer) {
    assert (context);
    assert (dumpBuffer);

    WriteToDumpWithErrorCheck (dumpBuffer, "digraph {\n\tbgcolor=\"" DUMP_BACKGROUND_COLOR "\"\n");

    return TranslationError::NO_ERRORS;
}
