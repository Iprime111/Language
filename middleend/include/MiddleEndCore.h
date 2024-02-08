#ifndef MIDDLE_END_CORE_H_
#define MIDDLE_END_CORE_H_

#include "NameTable.h"
#include "SyntaxTree.h"
#include "TreeReader.h"

const double EPS = 1e-7;

struct TranslationContext {
    Tree::Tree <AstNode>     abstractSyntaxTree = {};

    Buffer <NameTableRecord> nameTable          = {};
    Buffer <LocalNameTable>  localTables        = {};

    size_t entryPoint = 0;

    TranslationError error = TranslationError::NO_ERRORS;

    int currentLine  = 0;
};

TranslationError DestroyTranslationContext (TranslationContext *context);

#endif
