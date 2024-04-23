#ifndef FRONTEND_CORE_H_
#define FRONTEND_CORE_H_

#include "NameTable.h"
#include "Buffer.h"
#include "SyntaxTree.h"

enum class CompilationError: long long int {
    NO_ERRORS                    = 0,
    CONTEXT_ERROR                = 1 << 0,
    DERIVATIVE_EXPECTED          = 1 << 1,
    TOKEN_BUFFER_ERROR           = 1 << 2,
    FUNCTION_CALL_EXPECTED       = 1 << 3,
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
    ARGUMENT_SEPARATOR_EXPECTED  = 1 << 17,
    DUMP_ERROR                   = 1 << 18,
    HTML_ERROR                   = 1 << 19,
    OUTPUT_FILE_ERROR            = 1 << 20,
    RETURN_EXPECTED              = 1 << 21,
    BREAK_EXPECTED               = 1 << 22,
    CONTINUE_EXPECTED            = 1 << 23,
    IN_EXPECTED                  = 1 << 24,
    OUT_EXPECTED                 = 1 << 25,
    ABORT_EXPECTED               = 1 << 26,
    FUNCTION_REDEFINITION        = 1 << 27,
    VARIABLE_REDECLARATION       = 1 << 28,
    FUNCTION_NOT_DECLARED        = 1 << 29,
    VARIABLE_NOT_DECLARED        = 1 << 30,
    OPERATOR_NOT_FOUND           = 1 << 31,
};

struct ErrorData {
    CompilationError error = CompilationError::NO_ERRORS;
    int              line  = 0;
    char            *file  = nullptr;
};

struct CompilationContext {
    Buffer <NameTableRecord>        nameTable   = {};
    Buffer <LocalNameTable>         localTables = {};
    Buffer <Tree::Node <AstNode> *> tokens    = {};

    size_t tokenIndex = 0;

    int currentLine = 0;

    CompilationError   error     = CompilationError::NO_ERRORS;
    Buffer <ErrorData> errorBuffer = {};

    Tree::Tree <AstNode> abstractSyntaxTree = {};

    char *fileContent = nullptr;
    size_t fileLength = 0;

    size_t entryPoint = 0;

    Buffer <Tree::Node <AstNode> *> functionCalls = {};
};

CompilationError InitCompilationContext    (CompilationContext *context, char *fileContent);
CompilationError DestroyCompilationContext (CompilationContext *context);

CompilationError DumpTokenTable (CompilationContext *context);
CompilationError DumpToken      (CompilationContext *context, Tree::Node <AstNode> *token);

#ifndef NDEBUG
    #define ON_DEBUG(...) __VA_ARGS__
#else
    #define ON_DEBUG(...)
#endif

#endif
