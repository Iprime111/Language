#ifndef REVERSE_FRONTEND_H_
#define REVERSE_FRONTEND_H_

#include "NameTable.h"
#include "SyntaxTree.h"
#include "TreeReader.h"

struct TranslationContext {
    Tree::Tree <AstNode> abstractSyntaxTree    = {};

    Buffer <NameTableRecord> nameTable   = {};
    Buffer <LocalNameTable>  localTables = {};

    TranslationError error = TranslationError::NO_ERRORS;
};

TranslationError InitTranslationContext    (TranslationContext *context);
TranslationError DestroyTranslationContext (TranslationContext *context);

#endif
