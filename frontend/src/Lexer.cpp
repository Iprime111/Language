#include <cstring>
#include <ctype.h>

#include "Buffer.h"
#include "CustomAssert.h"
#include "FrontendCore.h"
#include "Lexer.h"
#include "Logger.h"
#include "NameTable.h"
#include "SyntaxTree.h"

#define AddToken(node)                                                                                  \
    do {                                                                                                \
        Tree::Node <AstNode> *newToken = node;                                                          \
        if (WriteDataToBuffer (&context->tokens, &newToken, 1) != BufferErrorCode::NO_BUFFER_ERRORS) {  \
            RETURN CompilationError::TOKEN_BUFFER_ERROR;                                                \
        }                                                                                               \
    } while (0)

#define LexerAssert(FUNCTION_CALL)                                                      \
    do {                                                                                \
        CompilationError error_ = FUNCTION_CALL;                                        \
        if (error_ != CompilationError::NO_ERRORS) {                                    \
            context->error = (CompilationError) ((int) context->error | (int) error_);  \
            RETURN error_;                                                              \
        }                                                                               \
    } while (0)

static CompilationError TokenizeNumber              (CompilationContext *context, const char **symbol);
static CompilationError TokenizeWord                (CompilationContext *context, const char **symbol);
static CompilationError TokenizeNewIdentifier       (CompilationContext *context, const char **identifier, size_t length);
static CompilationError TokenizeExistingIdentifier  (CompilationContext *context, const char **identifier, size_t length, size_t index);

static StringIntersection CheckWordIntersection (const char *word, const char *pattern, size_t wordLength);
static StringIntersection FindMatch (CompilationContext *context, const char *word, size_t wordLength, size_t *foundNameIndex);

static size_t GetNextWordLength (const char *wordBegin);

CompilationError LexicalAnalysis (CompilationContext *context, const char *fileContent) {
    PushLog (1);

    custom_assert (context,     pointer_is_null, CompilationError::CONTEXT_ERROR);
    custom_assert (fileContent, pointer_is_null, CompilationError::CONTEXT_ERROR);
    
    const char *symbol = fileContent;

    while (*symbol != '\0') {

        if (isspace (*symbol)) {
            symbol++;
            continue;
        }

        if (isdigit (*symbol)) {
            LexerAssert (TokenizeNumber (context, &symbol));
            continue;
        }

        LexerAssert (TokenizeWord (context, &symbol));
    } 

    RETURN CompilationError::NO_ERRORS;
}

static CompilationError TokenizeNumber (CompilationContext *context, const char **symbol) {
    PushLog (3);

    double number       = NAN;
    int    numberLength = 0;
            
    if (sscanf (*symbol, "%lf%n", &number, &numberLength) > 0) {
        AddToken (Const (number));
        (*symbol) += numberLength;

        RETURN CompilationError::NO_ERRORS;
    }

    RETURN CompilationError::NUMBER_EXPECTED;
}

static CompilationError TokenizeWord (CompilationContext *context, const char **symbol) {
    PushLog (3);

    size_t initialWordLength = 0;
    size_t wordLength = 0;

    while (true) {
        size_t nextWordLength = GetNextWordLength (*symbol + wordLength);

        if (initialWordLength == 0)
            initialWordLength = nextWordLength;

        if (nextWordLength == 0) {
            if (initialWordLength == 0) {
                RETURN CompilationError::IDENTIFIER_EXPECTED;
            }

            RETURN TokenizeNewIdentifier (context, symbol, initialWordLength); 
        }

        wordLength += nextWordLength;

        size_t foundNameIndex = 0;
        switch (FindMatch (context, *symbol, wordLength, &foundNameIndex)) {

            case StringIntersection::NO_MATCH: {
                RETURN TokenizeNewIdentifier (context, symbol, initialWordLength);
            }

            case StringIntersection::FULL_MATCH: {
                RETURN TokenizeExistingIdentifier (context, symbol, wordLength, foundNameIndex);
            }

            case StringIntersection::IS_BEGIN:
                break;
        }
    }
}

static CompilationError TokenizeNewIdentifier (CompilationContext *context, const char **identifier, size_t length) {
    PushLog (3);

    char *newIdentifier = (char *) calloc (length+ 1, sizeof (char));
    memcpy (newIdentifier, *identifier, length);

    if (AddIdentifier (&context->nameTable, newIdentifier) != BufferErrorCode::NO_BUFFER_ERRORS) {
        RETURN CompilationError::NAME_TABLE_ERROR;
    }

    AddToken (Name (context->nameTable.currentIndex - 1));

    (*identifier) += length;

    RETURN CompilationError::NO_ERRORS;
}

static CompilationError TokenizeExistingIdentifier (CompilationContext *context, const char **identifier, size_t length, size_t index) {
    PushLog (3);

    AddToken (Name (index));

    (*identifier) += length;

    RETURN CompilationError::NO_ERRORS;
}

static size_t GetNextWordLength (const char *wordBegin) {
    PushLog (4);

    if (wordBegin [0] == '\0') {
        RETURN 0;
    }

    size_t nextWordLength = 1;

    while (!isspace (wordBegin [nextWordLength]) && wordBegin [nextWordLength] != '\0') {
        nextWordLength++;
    }

    RETURN nextWordLength;
}

static StringIntersection FindMatch (CompilationContext *context, const char *word, size_t wordLength, size_t *foundNameIndex) {
    PushLog (4);

    StringIntersection maxIntersection = StringIntersection::NO_MATCH;

    for (size_t nameIndex = 0; nameIndex < context->nameTable.currentIndex; nameIndex++) {
        StringIntersection currentIntersction = CheckWordIntersection (word, context->nameTable.data [nameIndex].name, wordLength);

        if (currentIntersction == StringIntersection::FULL_MATCH) {
            *foundNameIndex = nameIndex;

            RETURN StringIntersection::FULL_MATCH;
        }

        if (maxIntersection == StringIntersection::NO_MATCH && currentIntersction == StringIntersection::IS_BEGIN) {
            maxIntersection = StringIntersection::IS_BEGIN;
        }
    }

    RETURN maxIntersection;
}

static StringIntersection CheckWordIntersection (const char *word, const char *pattern, size_t wordLength) {

    for (size_t symbol = 0; symbol < wordLength; symbol++) {
        if (pattern [symbol] == '\0')
            return StringIntersection::NO_MATCH;

        if (pattern [symbol] != word [symbol])
            return StringIntersection::NO_MATCH;
    }

    if (pattern [wordLength] == '\0')
        return StringIntersection::FULL_MATCH;

    return StringIntersection::IS_BEGIN;
}
