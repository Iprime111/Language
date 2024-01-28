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

static CompilationError SaveLocalTable  (CompilationContext *context, size_t localTableIndex, Buffer <char> *outputBuffer, FILE *stream);
static CompilationError SaveGlobalTable (CompilationContext *context, Buffer <char> *outputBuffer);

CompilationError SaveNameTables (CompilationContext *context, FILE *stream) {
    PushLog (3);

    Buffer <char> outputBuffer = {};

    if (InitBuffer (&outputBuffer) != BufferErrorCode::NO_BUFFER_ERRORS) {
        RETURN CompilationError::OUTPUT_FILE_ERROR;
    }

    SaveGlobalTable (context, &outputBuffer);

    for (size_t localTableIndex = 0; localTableIndex < context->localTables.currentIndex; localTableIndex++) {
        SaveLocalTable (context, localTableIndex, &outputBuffer, stream);
    }

    fwrite (outputBuffer.data, outputBuffer.currentIndex, 1, stream);

    DestroyBuffer (&outputBuffer);

    RETURN CompilationError::NO_ERRORS;
}

static CompilationError SaveLocalTable (CompilationContext *context, size_t localTableIndex, Buffer <char> *outputBuffer, FILE *stream) {
    PushLog (3);

    char numberBuffer [MAX_NUMBER_LENGTH] = "";

    snprintf (numberBuffer, MAX_NUMBER_LENGTH, "%lu", localTableIndex);
    WriteString (numberBuffer);

    WriteString (" ");

    snprintf (numberBuffer, MAX_NUMBER_LENGTH, "%lu", context->localTables.data [localTableIndex].items.currentIndex);
    WriteString (numberBuffer);
    WriteString ("\n");

    for (size_t itemIndex = 0; itemIndex < context->localTables.data [localTableIndex].items.currentIndex; itemIndex++) {
        LocalNameTableRecord *item = &context->localTables.data [localTableIndex].items.data [itemIndex];

        snprintf (numberBuffer, MAX_NUMBER_LENGTH, "%lu", item->globalNameId);
        WriteString (numberBuffer);

        WriteString (" ");

        snprintf (numberBuffer, MAX_NUMBER_LENGTH, "%d", (int) item->nameType);
        WriteString (numberBuffer);

        WriteString ("\n");
    }

    WriteString ("-1\n");

    RETURN CompilationError::NO_ERRORS;
}

static CompilationError SaveGlobalTable (CompilationContext *context, Buffer <char> *outputBuffer) {
    PushLog (3);

    char numberBuffer [MAX_NUMBER_LENGTH] = "";

    snprintf (numberBuffer, MAX_NUMBER_LENGTH, "%lu", context->nameTable.currentIndex);
    WriteString (numberBuffer);
    WriteString ("\n");

    for (size_t itemIndex = 0; itemIndex < context->nameTable.currentIndex; itemIndex++) {
        if (context->nameTable.data [itemIndex].type != NameType::IDENTIFIER)
            continue;

        snprintf (numberBuffer, MAX_NUMBER_LENGTH, "%lu", itemIndex);

        WriteString (numberBuffer);
        WriteString (" \"");
        WriteString (context->nameTable.data [itemIndex].name);
        WriteString ("\"\n");
    }

    WriteString ("-1\n");

    RETURN CompilationError::NO_ERRORS;
}

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
            WriteString ("2 ");

            snprintf (numberBuffer, MAX_NUMBER_LENGTH, "%lu", node->nodeData.content.nameTableIndex);
            WriteString (numberBuffer);
            WriteString (" ");

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

        snprintf (numberBuffer, MAX_NUMBER_LENGTH, "%lu", node->nodeData.content.nameTableIndex);
        WriteString (numberBuffer);
        WriteString (" ");

    }

    WriteString (" ");

    RETURN CompilationError::NO_ERRORS;
}
