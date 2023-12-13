#ifndef LEXER_H_
#define LEXER_H_

#include "FrontendCore.h"

enum class StringIntersection {
    NO_MATCH   = 0,
    IS_BEGIN   = 1,
    FULL_MATCH = 2,
};

CompilationError LexicalAnalysis (CompilationContext *context, const char *fileContent);
#endif
