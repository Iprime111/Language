#ifndef TREE_TRANSLATOR_H_
#define TREE_TRANSLATOR_H_

#include "ReverseFrontendCore.h"
#include "TreeReader.h"

const size_t MAX_NUMBER_LENGTH = 32;

#define WriteString(dataString)                                                                     \
    do {                                                                                            \
        if (WriteStringToBuffer (outputBuffer, dataString) != BufferErrorCode::NO_BUFFER_ERRORS) {  \
            RETURN TranslationError::OUTPUT_FILE_ERROR;                                             \
        }                                                                                           \
    } while (0)

#define CallbackFunction(function) function (context, node, outputBuffer)

#define Traversal(nextDirection) TreeTraversal (context, node->nextDirection, outputBuffer)

#define Indentation() WriteIndentation (context, outputBuffer)

TranslationError TranslateTree (TranslationContext *context, FILE *stream);

enum class BracketsPlacement {
    NO_BRACKETS = 0,
    LEFT        = 1 << 0,
    RIGHT       = 1 << 1,
};

#endif
