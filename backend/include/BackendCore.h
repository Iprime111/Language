#ifndef BACKEND_CORE_H_
#define BACKEND_CORE_H_

#include "NameTable.h"
#include "SyntaxTree.h"
#include "TreeReader.h"

struct OperatorsCount {

    size_t ifCount              = 0;
    size_t whileCount           = 0;
    size_t assignmentCount      = 0;
    size_t callCount            = 0;
    size_t logicExpressionCount = 0;
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

    OperatorsCount operatorsCount = {};
};

TranslationError InitTranslationContext    (TranslationContext *context);
TranslationError DestroyTranslationContext (TranslationContext *context);

#endif
