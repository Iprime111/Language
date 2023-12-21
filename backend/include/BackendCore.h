#ifndef BACKEND_CORE_H_
#define BACKEND_CORE_H_

#include "NameTable.h"
#include "SyntaxTree.h"

enum class TranslationError {
    NO_ERRORS         = 0,
    CONTEXT_ERROR     = 1 << 0,
    NAME_TABLE_ERROR  = 1 << 1,
    INPUT_FILE_ERROR  = 1 << 2,
    DUMP_ERROR        = 1 << 3,
    OUTPUT_FILE_ERROR = 1 << 4,
    TREE_ERROR        = 1 << 5,
};

struct TranslationContext {
    Tree::Tree <AstNode>     abstractSyntaxTree = {};
    Buffer <NameTableRecord> nameTable          = {};

    TranslationError error = TranslationError::NO_ERRORS;

    Buffer <LocalNameTable> localTables = {};

    size_t lastCycleEndLabel   = 0;
    size_t lastCycleBeginLabel = 0;

    bool areCallArguments = false;

    size_t labelIndex = 0;
};

TranslationError InitTranslationContext    (TranslationContext *context);
TranslationError DestroyTranslationContext (TranslationContext *context);

#endif
