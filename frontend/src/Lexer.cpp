#include <cctype>
#include <climits>
#include <cassert>
#include <clocale>

#include "Lexer.h"
#include "Buffer.h"
#include "FrontendCore.h"
#include "NameTable.h"
#include "SyntaxTree.h"

#define currentSymbol context->fileContent [*currentIndex]
#define AddToken(node)                                                                                  \
    do {                                                                                                \
        Tree::Node <AstNode> *newToken = node;                                                          \
        if (WriteDataToBuffer (&context->tokens, &newToken, 1) != BufferErrorCode::NO_BUFFER_ERRORS) {  \
            return CompilationError::TOKEN_BUFFER_ERROR;                                                \
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

static SymbolGroup GetPermittedSymbols (SymbolGroup group);
static size_t      GetMaxWordLength    (SymbolGroup group);

static bool IsCyrillic           (const char *multiByteSymbol);
static bool IsCyrillicSecondByte (char byte);

CompilationError LexicalAnalysis (CompilationContext *context) {
    assert (context);

    size_t currentIndex = 0;

    setlocale (LC_ALL, "en_US.utf8");

    while (currentIndex < context->fileLength) {
        if (context->fileContent [currentIndex] == '\n')
            context->currentLine++;

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

    AddToken (Terminator ());

    return CompilationError::NO_ERRORS;
}

static CompilationError TokenizeNumber (CompilationContext *context, size_t *currentIndex) {
    assert (context);
    assert (currentIndex);
    
    double number = NAN;
    int numberLength = 0;

    if (sscanf (&currentSymbol, "%lf%n", &number, &numberLength) > 0) {
        Tree::Node <AstNode> *constToken = Const (number);
        constToken->nodeData.line = context->currentLine;

        AddToken (constToken);

        if (context->fileContent [*currentIndex + (size_t) numberLength - 1] == '.') {
            numberLength--;
        }

        (*currentIndex) += (size_t) numberLength;


        return CompilationError::NO_ERRORS;
    }

    return CompilationError::TOKEN_BUFFER_ERROR;
}


static CompilationError TokenizeWord (CompilationContext *context, size_t *currentIndex) {
    assert (context);
    assert (currentIndex);

    size_t initialWordLength = 0;
    size_t wordLength = 0;

    while (true) {
        size_t nextWordLength = GetNextWordLength (context, *currentIndex + wordLength);

        if (initialWordLength == 0)
            initialWordLength = nextWordLength;

        if (nextWordLength == 0) {
            if (initialWordLength == 0) {
                return CompilationError::IDENTIFIER_EXPECTED;
            }

            return TokenizeNewIdentifier (context, currentIndex, initialWordLength); 
        }

        wordLength += nextWordLength;

        size_t foundNameIndex = 0;
        switch (FindMaxIntersection (context, &currentSymbol, wordLength, &foundNameIndex)) {

            case StringIntersection::NO_MATCH: {
                return TokenizeNewIdentifier (context, currentIndex, initialWordLength);
            }

            case StringIntersection::FULL_MATCH: {
                return TokenizeExistingIdentifier (context, currentIndex, wordLength, foundNameIndex);
            }

            case StringIntersection::IS_BEGIN:
                break;
        }
    }
}

static CompilationError TokenizeNewIdentifier (CompilationContext *context, size_t *currentIndex, size_t length) {
    assert (context);
    assert (currentIndex);

    char *newIdentifier = (char *) calloc (length + 1, sizeof (char));
    memcpy (newIdentifier, &currentSymbol, length);

    if (AddIdentifier (&context->nameTable, newIdentifier) != BufferErrorCode::NO_BUFFER_ERRORS) {
        return CompilationError::CONTEXT_ERROR;
    }

    Tree::Node <AstNode> *identifierToken = Name (context->nameTable.currentIndex - 1);
    identifierToken->nodeData.line = context->currentLine;

    AddToken (identifierToken);

    (*currentIndex) += length;

    return CompilationError::NO_ERRORS;
}

static CompilationError TokenizeExistingIdentifier (CompilationContext *context, size_t *currentIndex, size_t length, size_t nameIndex) {
    assert (context);
    assert (currentIndex);

    Tree::Node <AstNode> *identifierToken = Name (nameIndex);
    identifierToken->nodeData.line = context->currentLine;

    AddToken (identifierToken);

    (*currentIndex) += length;

    return CompilationError::NO_ERRORS;
}


static size_t GetNextWordLength (CompilationContext *context, size_t currentIndex) {
    assert (context);

    if (currentIndex > context->fileLength || context->fileContent [currentIndex] == '\n') {
        return 0;
    }

    SymbolGroup currentGroup   = GetSymbolGroup (context, currentIndex);
    SymbolGroup permittedGroup = GetPermittedSymbols (currentGroup);

    size_t maxLength = GetMaxWordLength (currentGroup);
    size_t length = 0;

    while (context->fileContent [currentIndex + length] != '\n'
           && ((int) currentGroup & (int) permittedGroup) && length < maxLength) {

        if (currentGroup == SymbolGroup::CYRILLIC)
            length++;

        length++;

        if (currentIndex + length < context->fileLength) {
            currentGroup = GetSymbolGroup (context, currentIndex + length);
        } else {
            break;
        }
    }

    return length;
}

static StringIntersection FindMaxIntersection (CompilationContext *context, const char *word, size_t wordLength, size_t *foundNameIndex) {
    assert (context);
    assert (word);
    assert (foundNameIndex);

    StringIntersection maxIntersection = StringIntersection::NO_MATCH;

    for (size_t nameIndex = 0; nameIndex < context->nameTable.currentIndex; nameIndex++) {
        StringIntersection currentIntersction = CheckWordIntersection (word, context->nameTable.data [nameIndex].name, wordLength);

        if (currentIntersction == StringIntersection::FULL_MATCH) {
            *foundNameIndex = nameIndex;

            return StringIntersection::FULL_MATCH;
        }

        if (maxIntersection == StringIntersection::NO_MATCH && currentIntersction == StringIntersection::IS_BEGIN) {
            maxIntersection = StringIntersection::IS_BEGIN;
        }
    }

    return maxIntersection;
}

static StringIntersection CheckWordIntersection (const char *word, const char *pattern, size_t wordLength) {
    assert (word);
    assert (pattern);

    // FIXME: keywords with same beginning (don't care)

    for (size_t symbol = 0; symbol < wordLength; symbol++) {
        if (pattern [symbol] == '\0') {
            return StringIntersection::NO_MATCH;
        }

        if (pattern [symbol] != word [symbol]) {
            return StringIntersection::NO_MATCH;
        }
    }

    if (pattern [wordLength] == '\0') {
        return StringIntersection::FULL_MATCH;
    }

    return StringIntersection::IS_BEGIN;
}

static size_t GetMaxWordLength (SymbolGroup group) {
    switch (group) {

        case SymbolGroup::ENGLISH:
        case SymbolGroup::CYRILLIC:
        case SymbolGroup::DIGIT:
        case SymbolGroup::UNDERSCORE:
        case SymbolGroup::SPACE:
            return INT_MAX;

        case SymbolGroup::BRACKET:
        case SymbolGroup::SEPARATOR:
            return 1;

        case SymbolGroup::OPERATION:
            return 2;

        case SymbolGroup::INVALID_SYMBOL:
        default:
            return 0;
    }
}

static SymbolGroup GetPermittedSymbols (SymbolGroup group) {
    switch (group) {

        case SymbolGroup::ENGLISH:
        case SymbolGroup::CYRILLIC:
        case SymbolGroup::DIGIT:
        case SymbolGroup::UNDERSCORE:
            return (SymbolGroup) ((int) SymbolGroup::ENGLISH | (int) SymbolGroup::CYRILLIC | (int) SymbolGroup::DIGIT | (int) SymbolGroup::UNDERSCORE);

        default:
            return group;
    }
}

static SymbolGroup GetSymbolGroup (CompilationContext *context, size_t symbolIndex) {
    assert (context);

    char symbol = context->fileContent [symbolIndex];

    if (isalpha (symbol)) {
        return SymbolGroup::ENGLISH;
    } else if (isdigit (symbol)) {
        return SymbolGroup::DIGIT;
    } else if (symbol == '{' || symbol == '}' || symbol == '(' || symbol == ')' ||
               symbol == '[' || symbol == ']') {

        return SymbolGroup::BRACKET;
    } else if (symbol == '+' || symbol == '-' || symbol == '*' || symbol == '/' || 
               symbol == '=' || symbol == '!' || symbol == '<' || symbol == '>') {

        return SymbolGroup::OPERATION;
    } else if (isspace (symbol)) {
        return SymbolGroup::SPACE;
    } else if (symbol == '_') {
        return SymbolGroup::UNDERSCORE;
    } else if (symbol == '.' || symbol == ',') {
        return SymbolGroup::SEPARATOR;
    } else if (IsCyrillic (&context->fileContent [symbolIndex])) {
        return SymbolGroup::CYRILLIC;   
    } else {
        return SymbolGroup::INVALID_SYMBOL;
    }
}

static bool IsCyrillic (const char *multiByteSymbol) {
    assert (multiByteSymbol);

    //WARNING: implementation defined

    wchar_t symbol = 0;
    mbtowc (&symbol, multiByteSymbol, 2);

    char *symbolBytes = (char *) (&symbol);

    if (symbolBytes [1] == 0x04 && IsCyrillicSecondByte (symbolBytes [0]))
        return true;

    return false;
}

static bool IsCyrillicSecondByte (char byte) {
    return (byte >= 0x10 && byte <= 0x4f) || byte == 0x51 || byte == 0x01; // Magic constants from unicode characters table
}
