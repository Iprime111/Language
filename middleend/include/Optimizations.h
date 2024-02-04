#ifndef OPTIMIZATIONS_H_
#define OPTIMIZATIONS_H_

#include "MiddleEndCore.h"
#include "TreeDefinitions.h"
#include "TreeReader.h"

const double EPS = 1e-7;

struct SpecialValue {
    Tree::TreeEdge validEdge  = Tree::NO_EDGE;
    double         validValue = NAN;
    double         result     = NAN;
};

TranslationError DoOptimizations  (TranslationContext *context);
TranslationError EvalOptimization (TranslationContext *context, bool *treeChanged);

#endif
