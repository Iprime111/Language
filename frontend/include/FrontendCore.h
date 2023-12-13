#ifndef FRONTEND_CORE_H_
#define FRONTEND_CORE_H_

#include "NameTable.h"
#include "Buffer.h"
#include "SyntaxTree.h"

enum class CompilationError {
    NO_ERRORS           = 0,
    CONTEXT_ERROR       = 1 << 0,
    NAME_TABLE_ERROR    = 1 << 1,
    TOKEN_BUFFER_ERROR  = 1 << 2,
    NUMBER_EXPECTED     = 1 << 3,
    IDENTIFIER_EXPECTED = 1 << 4,
};

struct CompilationContext {
    Buffer <NameTableRecord>        nameTable = {};
    Buffer <Tree::Node <AstNode> *> tokens    = {};

    CompilationError error = CompilationError::NO_ERRORS;
};

CompilationError InitCompilationContext (CompilationContext *context);

CompilationError DumpTokenTable (CompilationContext *context);
CompilationError DumpToken      (CompilationContext *context, Tree::Node <AstNode> *token);

#endif
