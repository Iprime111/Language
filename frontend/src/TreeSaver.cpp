#include "Buffer.h"
#include "FrontendCore.h"
#include "NameTable.h"
#include "SyntaxTree.h"
#include "TreeSaver.h"

#define WriteString(data)                                                                       \
    do {                                                                                        \
        if (WriteStringToBuffer (outputBuffer, data) != BufferErrorCode::NO_BUFFER_ERRORS) {    \
            RETURN CompilationError::OUTPUT_FILE_ERROR;                                         \
        }                                                                                       \
    } while (0)

static CompilationError SaveTreeInternal (CompilationContext *context, Tree::Node <AstNode> *node, Buffer <char> *outputBuffer);
static CompilationError WriteNodeContent (CompilationContext *context, Tree::Node <AstNode> *node, Buffer <char> *outputBuffer);

CompilationError SaveTree (CompilationContext *context, FILE *stream) {
    PushLog (3);

    Buffer <char> outputBuffer = {};

    if (InitBuffer (&outputBuffer) != BufferErrorCode::NO_BUFFER_ERRORS) {
        RETURN CompilationError::OUTPUT_FILE_ERROR;
    }

    SaveTreeInternal (context, context->abstractSyntaxTree.root, &outputBuffer);

    fwrite (outputBuffer.data, outputBuffer.currentIndex, 1, stream);

    DestroyBuffer (&outputBuffer);

    RETURN CompilationError::NO_ERRORS;
}

static CompilationError SaveTreeInternal (CompilationContext *context, Tree::Node <AstNode> *node, Buffer <char> *outputBuffer) {
    PushLog (4);

    if (!node) {
        WriteString ("_ ");
        RETURN CompilationError::NO_ERRORS;
    }

    WriteString ("( ");

    WriteNodeContent (context, node, outputBuffer);

    SaveTreeInternal (context, node->left,  outputBuffer);
    SaveTreeInternal (context, node->right, outputBuffer);

    WriteString (") ");

    RETURN CompilationError::NO_ERRORS;
}

static CompilationError WriteNodeContent (CompilationContext *context, Tree::Node <AstNode> *node, Buffer <char> *outputBuffer) {
    PushLog (4);

    char numberBuffer [MAX_NUMBER_LENGTH] = "";

    if (node->nodeData.type == NodeType::NAME) {
        if (context->nameTable.data [node->nodeData.content.nameTableIndex].keyword == Keyword::NOT_KEYWORD) {
            WriteString ("2 \"");
            WriteString (context->nameTable.data [node->nodeData.content.nameTableIndex].name);
            WriteString ("\" ");

        } else {
            WriteString ("3 ");

            snprintf    (numberBuffer, MAX_NUMBER_LENGTH, "%d", (int) context->nameTable.data [node->nodeData.content.nameTableIndex].keyword);
            WriteString (numberBuffer);
            WriteString (" ");
        }

        RETURN CompilationError::NO_ERRORS;
    } else {
        snprintf    (numberBuffer, MAX_NUMBER_LENGTH, "%d", (int) node->nodeData.type);
        WriteString (numberBuffer);
    }

    WriteString (" ");

    if (node->nodeData.type == NodeType::CONSTANT) {
        snprintf    (numberBuffer, MAX_NUMBER_LENGTH, "%lg", node->nodeData.content.number);
        WriteString (numberBuffer);

    } else if (node->nodeData.type == NodeType::FUNCTION_DEFINITION || node->nodeData.type == NodeType::VARIABLE_DECLARATION) {

        WriteString ("\"");
        WriteString (context->nameTable.data [node->nodeData.content.nameTableIndex].name);
        WriteString ("\" ");

    }

    WriteString (" ");

    RETURN CompilationError::NO_ERRORS;
}
