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

#define WriteWordNode(function)         \
    do {                                \
        CallbackFunction (function);    \
    } while (0)

#define Traversal(nextDirection) TreeTraversal (context, node->nextDirection, outputBuffer)

TranslationError TranslateTree (TranslationContext *context, FILE *stream);

#endif
