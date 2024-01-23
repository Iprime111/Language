#include <cctype>

#include "Lexer.h"
#include "Buffer.h"
#include "FrontendCore.h"
#include "Logger.h"
#include "SyntaxTree.h"

#define currentSymbol context->fileContent [*currentIndex]
#define AddToken(node)                                                                                  \
    do {                                                                                                \
        Tree::Node <AstNode> *newToken = node;                                                          \
        if (WriteDataToBuffer (&context->tokens, &newToken, 1) != BufferErrorCode::NO_BUFFER_ERRORS) {  \
            RETURN CompilationError::TOKEN_BUFFER_ERROR;                                                \
        }                                                                                               \
    } while (0)

static CompilationError TokenizeNumber    (CompilationContext *context, size_t *currentIndex);
static CompilationError TokenizeWord      (CompilationContext *context, size_t *currentIndex);
static size_t           GetNextWordLength (CompilationContext *context, size_t currentIndex);

static CompilationError TokenizeNewIdentifier      (CompilationContext *context, size_t *currentIndex, size_t length);
static CompilationError TokenizeExistingIdentifier (CompilationContext *context, size_t *currentIndex, size_t length, size_t nameIndex);

static StringIntersection CheckWordIntersection (const char *word, const char *pattern, size_t wordLength);
static StringIntersection FindMaxIntersection   (CompilationContext *context, const char *word, size_t wordLength, size_t *foundNameIndex);
static SymbolGroup        GetSymbolGroup        (CompilationContext *context, size_t symbolIndex);

CompilationError LexicalAnalysis (CompilationContext *context) {
    PushLog (1);

    size_t currentIndex = 0;

    while (currentIndex < context->fileLength) {
        if (isspace (context->fileContent [currentIndex])) {
            currentIndex++;
            continue;
        }

        if (isdigit (context->fileContent [currentIndex])) {
            TokenizeNumber (context, &currentIndex); // TODO: Catch errors
        } else {
            TokenizeWord (context, &currentIndex);
        }
    }

    RETURN CompilationError::NO_ERRORS;
}

static CompilationError TokenizeNumber (CompilationContext *context, size_t *currentIndex) {
    PushLog (3);
    
    double number = NAN;
    int numberLength = 0;

    if (sscanf (&currentSymbol, "%lf%n", &number, &numberLength) > 0) {
        AddToken (Const (number));
        (*currentIndex) += (size_t) numberLength;

        RETURN CompilationError::NO_ERRORS;
    }

    RETURN CompilationError::TOKEN_BUFFER_ERROR;
}


static CompilationError TokenizeWord (CompilationContext *context, size_t *currentIndex) {
    PushLog (3);

    size_t initialWordLength = 0;
    size_t wordLength = 0;

    while (true) {
        size_t nextWordLength = GetNextWordLength (context, *currentIndex + wordLength);

        if (initialWordLength == 0)
            initialWordLength = nextWordLength;

        if (nextWordLength == 0) {
            if (initialWordLength == 0) {
                RETURN CompilationError::IDENTIFIER_EXPECTED;
            }

            RETURN TokenizeNewIdentifier (context, currentIndex, initialWordLength); 
        }

        wordLength += nextWordLength;

        size_t foundNameIndex = 0;
        switch (FindMaxIntersection (context, &currentSymbol, wordLength, &foundNameIndex)) {

            case StringIntersection::NO_MATCH: {
                RETURN TokenizeNewIdentifier (context, currentIndex, initialWordLength);
            }

            case StringIntersection::FULL_MATCH: {
                RETURN TokenizeExistingIdentifier (context, currentIndex, wordLength, foundNameIndex);
            }

            case StringIntersection::IS_BEGIN:
                break;
        }
    }
}

static CompilationError TokenizeNewIdentifier (CompilationContext *context, size_t *currentIndex, size_t length) {
    PushLog (3);

    char *newIdentifier = (char *) calloc (length + 1, sizeof (char));
    memcpy (newIdentifier, &currentSymbol, length);

    if (AddIdentifier (&context->nameTable, newIdentifier) != BufferErrorCode::NO_BUFFER_ERRORS) {
        RETURN CompilationError::NAME_TABLE_ERROR;
    }

    AddToken (Name (context->nameTable.currentIndex - 1));

    (*currentIndex) += length;

    RETURN CompilationError::NO_ERRORS;
}

static CompilationError TokenizeExistingIdentifier (CompilationContext *context, size_t *currentIndex, size_t length, size_t nameIndex) {
    PushLog (3);

    AddToken (Name (nameIndex));

    (*currentIndex) += length;

    RETURN CompilationError::NO_ERRORS;
}


static size_t GetNextWordLength (CompilationContext *context, size_t currentIndex) {
    PushLog (3);

    if (currentIndex > context->fileLength || context->fileContent [currentIndex] == '\n') {
        RETURN 0;
    }

    SymbolGroup currentGroup = GetSymbolGroup (context, currentIndex);
    size_t length = 0;

    while (currentIndex + length < context->fileLength && context->fileContent [currentIndex + length] != '\n'
           && GetSymbolGroup (context, currentIndex + length) == currentGroup) {

        length++;
    }

    RETURN length;
}

static StringIntersection FindMaxIntersection (CompilationContext *context, const char *word, size_t wordLength, size_t *foundNameIndex) {
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
    PushLog (4); // FIXME: keywords with same beginning

    for (size_t symbol = 0; symbol < wordLength; symbol++) {
        if (pattern [symbol] == '\0') {
            RETURN StringIntersection::NO_MATCH;
        }

        if (pattern [symbol] != word [symbol]) {
            RETURN StringIntersection::NO_MATCH;
        }
    }

    if (pattern [wordLength] == '\0') {
        RETURN StringIntersection::FULL_MATCH;
    }

    RETURN StringIntersection::IS_BEGIN;
}
static SymbolGroup GetSymbolGroup (CompilationContext *context, size_t symbolIndex) {
    PushLog (4);

    char symbol = context->fileContent [symbolIndex];

    if (isalnum (symbol) || symbol == '_') {
        RETURN SymbolGroup::ALNUM;
    } else if (symbol == '{' || symbol == '}' || symbol == '(' || symbol == ')' ||
               symbol == '[' || symbol == ']') {

        RETURN SymbolGroup::BRACKET;
    } else if (symbol == '+' || symbol == '-' || symbol == '*' || symbol == '/' || 
               symbol == '=' || symbol == '!' || symbol == '<' || symbol == '>') {

        RETURN SymbolGroup::OPERATION;
    } else if (isspace (symbol)) {
        RETURN SymbolGroup::SPACE;
    } else {
        RETURN SymbolGroup::INVALID_SYMBOL;
    }
}
