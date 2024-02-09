#include <stdio.h>

#include "Logger.h"
#include "MiddleEndCore.h"
#include "TreeReader.h"
#include "TreeSaver.h"

#define WriteString(data)                                                                       \
    do {                                                                                        \
        if (WriteStringToBuffer (outputBuffer, data) != BufferErrorCode::NO_BUFFER_ERRORS) {    \
            RETURN TranslationError::OUTPUT_FILE_ERROR;                                         \
        }                                                                                       \
    } while (0)


static TranslationError SaveTreeInternal (TranslationContext *context, Tree::Node <AstNode> *node, Buffer <char> *outputBuffer);
static TranslationError WriteNodeContent (TranslationContext *context, Tree::Node <AstNode> *node, Buffer <char> *outputBuffer);

TranslationError SaveTree (TranslationContext *context, FILE *stream) {
    PushLog (3);

    custom_assert (context, pointer_is_null, TranslationError::CONTEXT_ERROR);
    custom_assert (stream,  pointer_is_null, TranslationError::OUTPUT_FILE_ERROR);  

    Buffer <char> outputBuffer = {};

    if (InitBuffer (&outputBuffer) != BufferErrorCode::NO_BUFFER_ERRORS) {
        RETURN TranslationError::OUTPUT_FILE_ERROR;
    }

    char numberBuffer [MAX_NUMBER_LENGTH] = "";
    snprintf (numberBuffer, MAX_NUMBER_LENGTH, "%lu", context->entryPoint);

    WriteStringToBuffer (&outputBuffer, numberBuffer);
    WriteStringToBuffer (&outputBuffer, " ");

    SaveTreeInternal (context, context->abstractSyntaxTree.root, &outputBuffer);

    fwrite (outputBuffer.data, outputBuffer.currentIndex, 1, stream);

    DestroyBuffer (&outputBuffer);

    RETURN TranslationError::NO_ERRORS;
}

static TranslationError SaveTreeInternal (TranslationContext *context, Tree::Node <AstNode> *node, Buffer <char> *outputBuffer) {
    PushLog (4);

    if (!node) {
        WriteString ("_ ");
        RETURN TranslationError::NO_ERRORS;
    }

    WriteString ("( ");

    WriteNodeContent (context, node, outputBuffer);

    SaveTreeInternal (context, node->left,  outputBuffer);
    SaveTreeInternal (context, node->right, outputBuffer);

    WriteString (") ");

    RETURN TranslationError::NO_ERRORS;
}

static TranslationError WriteNodeContent (TranslationContext *context, Tree::Node <AstNode> *node, Buffer <char> *outputBuffer) {
    PushLog (4);

    char numberBuffer [MAX_NUMBER_LENGTH] = "";

    snprintf    (numberBuffer, MAX_NUMBER_LENGTH, "%d",  (int) node->nodeData.type);
    WriteString (numberBuffer);
    WriteString (" ");

    #define PrintNode(FORMAT, NUMBER)                                       \
            snprintf    (numberBuffer, MAX_NUMBER_LENGTH, FORMAT, NUMBER);  \
            WriteString (numberBuffer);                                     \
            break

    switch (node->nodeData.type) {

        case NodeType::CONSTANT:
            PrintNode ("%lg", node->nodeData.content.number);

        case NodeType::STRING:
            PrintNode ("%lu", node->nodeData.content.nameTableIndex);

        case NodeType::VARIABLE_DECLARATION:
        case NodeType::FUNCTION_DEFINITION:
        case NodeType::KEYWORD:
            PrintNode ("%d", (int) node->nodeData.content.keyword);


        default:
          break;
    };

    WriteString (" ");

    #undef PrintNode

    RETURN TranslationError::NO_ERRORS;
}

