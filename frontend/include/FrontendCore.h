#ifndef FRONTEND_CORE_H_
#define FRONTEND_CORE_H_

#include "NameTable.h"
#include "Buffer.h"
#include "SyntaxTree.h"

enum class CompilationError {
    NO_ERRORS                    = 0,
    CONTEXT_ERROR                = 1 << 0,
    NAME_TABLE_ERROR             = 1 << 1,
    TOKEN_BUFFER_ERROR           = 1 << 2,
    IDENTIFIER_EXPECTED          = 1 << 4,
    INITIAL_OPERATOR_EXPECTED    = 1 << 5,
    OPERATOR_SEPARATOR_EXPECTED  = 1 << 6,
    TYPE_NAME_EXPECTED           = 1 << 7,
    BRACKET_EXPECTED             = 1 << 8,
    CODE_BLOCK_EXPECTED          = 1 << 9,
    FUNCTION_EXPECTED            = 1 << 10,
    ASSIGNMENT_EXPECTED          = 1 << 11,
    CONDITION_SEPARATOR_EXPECTED = 1 << 12,
    IF_EXPECTED                  = 1 << 13,
    WHILE_EXPECTED               = 1 << 14,
    OPERATION_EXPECTED           = 1 << 15,
    CONSTANT_EXPECTED            = 1 << 16,
    DUMP_ERROR                   = 1 << 17,
    HTML_ERROR                   = 1 << 18,
};

struct ErrorData {
    CompilationError error = CompilationError::NO_ERRORS;
    int              line  = 0;
    char            *file  = NULL;
};

struct CompilationContext {
    Buffer <NameTableRecord>        nameTable = {};
    Buffer <Tree::Node <AstNode> *> tokens    = {};

    size_t tokenIndex = 0;

    int currentLine = 0;

    CompilationError   error     = CompilationError::NO_ERRORS;
    Buffer <ErrorData> errorList = {};

    Tree::Tree <AstNode> abstractSyntaxTree = {};
};

CompilationError InitCompilationContext    (CompilationContext *context);
CompilationError DestroyCompilationContext (CompilationContext *context);

CompilationError DumpTokenTable (CompilationContext *context);
CompilationError DumpToken      (CompilationContext *context, Tree::Node <AstNode> *token);

#ifndef NDEBUG
    #define ON_DEBUG(...) __VA_ARGS__
#else
    #define ON_DEBUG(...)
#endif

#endif
