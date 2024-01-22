#include <cctype>

#include "Lexer.h"
#include "Buffer.h"
#include "FrontendCore.h"
#include "Logger.h"
#include "SyntaxTree.h"

#define currentSymbol context->fileContent [*currentIndex]
#define AddToken(token)                                                                         \
    if (WriteDataToBuffer (&context->tokens, token, 1) != BufferErrorCode::NO_BUFFER_ERRORS) {  \
        RETURN CompilationError::TOKEN_BUFFER_ERROR;                                            \
    }

static CompilationError TokenizeNumber (CompilationContext *context, size_t *currentIndex);
static CompilationError TokenizeLexeme (CompilationContext *context, size_t *currentIndex);
static CompilationError GetNextWord    (CompilationContext *context, size_t *currentIndex);

static StringIntersection CheckWordIntersection (const char *word, const char *pattern, size_t wordLength);
static StringIntersection FindMaxIntersection   (CompilationContext *context, const char *word, size_t wordLength, size_t *foundNameIndex);
static SymbolGroup        GetSymbolGroup        (CompilationContext *context, size_t symbolIndex);

CompilationError LexicalAnalysis (CompilationContext *context) {
    PushLog (1);

    size_t currentIndex = 0;

    while (currentIndex < context->fileLength) {
        if (isdigit (context->fileContent [currentIndex])) {
            TokenizeNumber (context, &currentIndex); // TODO: Catch errors
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
        numberLength++;
    }

    RETURN CompilationError::NO_ERRORS;
}

static CompilationError TokenizeLexeme (CompilationContext *context, size_t *currentIndex) {
    PushLog (3);

    size_t lexemeLength = 0;
    StringIntersection intersectionStatus = StringIntersection::IS_BEGIN;

    while () {

    }

    RETURN CompilationError::NO_ERRORS;
}

static CompilationError GetNextWord (CompilationContext *context, size_t *currentIndex, size_t *lexemeLength) {
    PushLog (3);

    if (*currentIndex < context->fileLength || currentSymbol == '\n') {
        RETURN CompilationError::TOKEN_BUFFER_ERROR;
    }

    SymbolGroup currentGroup = GetSymbolGroup (context, *currentIndex);

    while (*currentIndex < context->fileLength && GetSymbolGroup (context, *currentIndex + *lexemeLength) == currentGroup) {
        (*lexemeLength)++;
    }

    RETURN CompilationError::NO_ERRORS;
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
        if (pattern [symbol] == '\0')
            RETURN StringIntersection::NO_MATCH;

        if (pattern [symbol] != word [symbol])
            RETURN StringIntersection::NO_MATCH;
    }

    if (pattern [wordLength] == '\0')
        RETURN StringIntersection::FULL_MATCH;

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
