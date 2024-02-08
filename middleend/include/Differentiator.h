#ifndef DIFFERENTIATOR_H_
#define DIFFERENTIATOR_H_

#include "MiddleEndCore.h"
#include "TreeReader.h"

TranslationError DifferentiationTraversal (TranslationContext *context, Tree::Node <AstNode> *node);
double           EvalSubtree              (TranslationContext *context, Tree::Node <AstNode> *node);

#endif
