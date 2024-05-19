#include <cassert>
#include <cstddef>

#include "Buffer.h"
#include "FrontendCore.h"
#include "NameTable.h"
#include "SyntaxTree.h"
#include "TreeSaver.h"

#define WriteString(data)                                                                       \
    do {                                                                                        \
        if (WriteStringToBuffer (outputBuffer, data) != BufferErrorCode::NO_BUFFER_ERRORS) {    \
            return CompilationError::OUTPUT_FILE_ERROR;                                         \
        }                                                                                       \
    } while (0)

static CompilationError SaveTreeInternal (CompilationContext *context, Tree::Node <AstNode> *node, Buffer <char> *outputBuffer, size_t keywordsCount);
static CompilationError WriteNodeContent (CompilationContext *context, Tree::Node <AstNode> *node, Buffer <char> *outputBuffer, size_t keywordsCount);

static CompilationError SaveLocalTable   (CompilationContext *context, size_t localTableIndex, Buffer <char> *outputBuffer,
                                          FILE *stream, size_t keywordsCount);

static CompilationError SaveGlobalTable  (CompilationContext *context, Buffer <char> *outputBuffer, size_t keywordsCount);
static size_t           GetKeywordsCount (CompilationContext *context);

CompilationError SaveNameTables (CompilationContext *context, FILE *stream) {
    assert (context);
    assert (stream);

    Buffer <char> outputBuffer = {};

    if (InitBuffer (&outputBuffer) != BufferErrorCode::NO_BUFFER_ERRORS) {
        return CompilationError::OUTPUT_FILE_ERROR;
    }

    size_t keywordsCount = GetKeywordsCount (context);

    SaveGlobalTable (context, &outputBuffer, keywordsCount);

    char numberBuffer [MAX_NUMBER_LENGTH + 1] = "";
    snprintf (numberBuffer, MAX_NUMBER_LENGTH + 1, "%lu\n", context->localTables.currentIndex);

    if (WriteStringToBuffer (&outputBuffer, numberBuffer) != BufferErrorCode::NO_BUFFER_ERRORS) {
        return CompilationError::CONTEXT_ERROR;
    }

    for (size_t localTableIndex = 0; localTableIndex < context->localTables.currentIndex; localTableIndex++) {
        SaveLocalTable (context, localTableIndex, &outputBuffer, stream, keywordsCount);
    }

    fwrite (outputBuffer.data, outputBuffer.currentIndex, 1, stream);

    DestroyBuffer (&outputBuffer);

    return CompilationError::NO_ERRORS;
}

static CompilationError SaveLocalTable (CompilationContext *context, size_t localTableIndex, Buffer <char> *outputBuffer, FILE *stream, size_t keywordsCount) {

    char numberBuffer [MAX_NUMBER_LENGTH] = "";

    snprintf    (numberBuffer, MAX_NUMBER_LENGTH, "%lu", context->localTables.data [localTableIndex].items.currentIndex);
    WriteString (numberBuffer);
    WriteString (" ");

    int nameTableId = context->localTables.data [localTableIndex].nameTableId;
    nameTableId     = nameTableId - (nameTableId > 0 ? (int) keywordsCount : 0);

    snprintf    (numberBuffer, MAX_NUMBER_LENGTH, "%d", nameTableId);
    WriteString (numberBuffer);
    WriteString ("\n");

    for (size_t itemIndex = 0; itemIndex < context->localTables.data [localTableIndex].items.currentIndex; itemIndex++) {
        LocalNameTableRecord *item = &context->localTables.data [localTableIndex].items.data [itemIndex];

        snprintf (numberBuffer, MAX_NUMBER_LENGTH, "%lu", item->globalNameId - keywordsCount);
        WriteString (numberBuffer);

        WriteString (" ");

        snprintf (numberBuffer, MAX_NUMBER_LENGTH, "%d", (int) item->nameType);
        WriteString (numberBuffer);

        WriteString ("\n");
    }

    return CompilationError::NO_ERRORS;
}

static CompilationError SaveGlobalTable (CompilationContext *context, Buffer <char> *outputBuffer, size_t keywordsCount) {

    char numberBuffer [MAX_NUMBER_LENGTH] = "";


    snprintf (numberBuffer, MAX_NUMBER_LENGTH, "%lu", context->nameTable.currentIndex - keywordsCount);
    WriteString (numberBuffer);
    WriteString ("\n");

    for (size_t itemIndex = keywordsCount; itemIndex < context->nameTable.currentIndex; itemIndex++) {
        WriteString (context->nameTable.data [itemIndex].name);
        WriteString ("\n");
    }

    return CompilationError::NO_ERRORS;
}

CompilationError SaveTree (CompilationContext *context, FILE *stream) {

    assert (context);
    assert (stream);  

    Buffer <char> outputBuffer = {};

    if (InitBuffer (&outputBuffer) != BufferErrorCode::NO_BUFFER_ERRORS) {
        return CompilationError::OUTPUT_FILE_ERROR;
    }

    size_t keywordsCount = GetKeywordsCount (context);

    char numberBuffer [MAX_NUMBER_LENGTH] = "";
    snprintf (numberBuffer, MAX_NUMBER_LENGTH, "%lu", context->entryPoint - keywordsCount);

    WriteStringToBuffer (&outputBuffer, numberBuffer);
    WriteStringToBuffer (&outputBuffer, " ");

    SaveTreeInternal (context, context->abstractSyntaxTree.root, &outputBuffer, keywordsCount);

    fwrite (outputBuffer.data, outputBuffer.currentIndex, 1, stream);

    DestroyBuffer (&outputBuffer);

    return CompilationError::NO_ERRORS;
}

static CompilationError SaveTreeInternal (CompilationContext *context, Tree::Node <AstNode> *node, Buffer <char> *outputBuffer, size_t keywordsCount) {

    if (!node) {
        WriteString ("_ ");
        return CompilationError::NO_ERRORS;
    }

    WriteString ("( ");

    WriteNodeContent (context, node, outputBuffer, keywordsCount);

    SaveTreeInternal (context, node->left,  outputBuffer, keywordsCount);
    SaveTreeInternal (context, node->right, outputBuffer, keywordsCount);

    WriteString (") ");

    return CompilationError::NO_ERRORS;
}

static CompilationError WriteNodeContent (CompilationContext *context, Tree::Node <AstNode> *node, Buffer <char> *outputBuffer, size_t keywordsCount) {
    char numberBuffer [MAX_NUMBER_LENGTH] = "";

    if (node->nodeData.type == NodeType::STRING) {
        if (context->nameTable.data [node->nodeData.content.nameTableIndex].keyword == Keyword::NOT_KEYWORD) {
            WriteString ("2 ");

            snprintf (numberBuffer, MAX_NUMBER_LENGTH, "%lu", node->nodeData.content.nameTableIndex - keywordsCount);
            WriteString (numberBuffer);
            WriteString (" ");

        } else {
            WriteString ("3 ");

            snprintf    (numberBuffer, MAX_NUMBER_LENGTH, "%d", (int) context->nameTable.data [node->nodeData.content.nameTableIndex].keyword);
            WriteString (numberBuffer);
            WriteString (" ");
        }

        return CompilationError::NO_ERRORS;
    } else {
        snprintf    (numberBuffer, MAX_NUMBER_LENGTH, "%d", (int) node->nodeData.type);
        WriteString (numberBuffer);
    }

    WriteString (" ");

    if (node->nodeData.type == NodeType::CONSTANT) {
        snprintf    (numberBuffer, MAX_NUMBER_LENGTH, "%lg", node->nodeData.content.number);
        WriteString (numberBuffer);

        WriteString (" ");
    } else if (node->nodeData.type == NodeType::FUNCTION_DEFINITION || node->nodeData.type == NodeType::VARIABLE_DECLARATION) {

        snprintf (numberBuffer, MAX_NUMBER_LENGTH, "%lu", node->nodeData.content.nameTableIndex - keywordsCount);
        WriteString (numberBuffer);
        WriteString (" ");

    }


    return CompilationError::NO_ERRORS;
}

static size_t GetKeywordsCount (CompilationContext *context) {
    size_t keywordsCount = 0;

    while (keywordsCount < context->nameTable.currentIndex && context->nameTable.data [keywordsCount].type != NameType::IDENTIFIER)
        keywordsCount++;

    return keywordsCount;
}
