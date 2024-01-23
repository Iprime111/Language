#ifndef LEXER_H_
#define LEXER_H_

#include "FrontendCore.h"

enum class StringIntersection {
    NO_MATCH   = 0,
    IS_BEGIN   = 1,
    FULL_MATCH = 2,
};

enum class SymbolGroup {
    ALNUM          = 1 << 0,
    BRACKET        = 1 << 1,
    OPERATION      = 1 << 2,
    INVALID_SYMBOL = 1 << 3,
    SPACE          = 1 << 4,
};

CompilationError LexicalAnalysis (CompilationContext *context);
#endif
