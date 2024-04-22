#include <math.h>
#include <cassert>

#include "CustomAssert.h"
#include "MiddleEndCore.h"
#include "NodeUtils.h"
#include "SyntaxTree.h"
#include "TreeDefinitions.h"
#include "TreeReader.h"
#include "Optimizations.h"

#define Eval(direction) EvalOptimizationInternal (context, node->direction, treeChanged)

static double           EvalOptimizationInternal          (TranslationContext *context, Tree::Node <AstNode> *node, bool *treeChanged);
static TranslationError SpecialValuesOptimizationInternal (TranslationContext *context, Tree::Node <AstNode> *node, bool *treeChanged);
static TranslationError ProcessSpecialValue               (TranslationContext *context, Tree::Node <AstNode> *node, SpecialValue specialValue, bool *treeChanged);

TranslationError DoOptimizations (TranslationContext *context) {
    assert (context);
    assert (context->abstractSyntaxTree.root);

    bool treeChanged = true;

    while (treeChanged) {
        treeChanged = false;

        EvalOptimizationInternal          (context, context->abstractSyntaxTree.root, &treeChanged);
        SpecialValuesOptimizationInternal (context, context->abstractSyntaxTree.root, &treeChanged);
    }

    return TranslationError::NO_ERRORS;
}

TranslationError EvalOptimization (TranslationContext *context, bool *treeChanged) {
    PushLog (1);

    custom_assert (context,                          pointer_is_null, TranslationError::CONTEXT_ERROR);
    custom_assert (context->abstractSyntaxTree.root, pointer_is_null, TranslationError::TREE_ERROR);

    EvalOptimizationInternal (context, context->abstractSyntaxTree.root, treeChanged);

    return TranslationError::NO_ERRORS;
}

static double EvalOptimizationInternal (TranslationContext *context, Tree::Node <AstNode> *node, bool *treeChanged) {
    assert (context);
    assert (node);
    assert (treeChanged);

    if (!node)
        return NAN;

    switch (node->nodeData.type) {

        case NodeType::CONSTANT: { return node->nodeData.content.number; }
        case NodeType::KEYWORD:  { break; }
        default:                 { Eval (left); Eval (right); return NAN; }
    };

    double evalResult = NAN;

    #define OPERATION(KWORD, EVAL_FUNCTION, ...)                \
        case (Keyword::KWORD): {                                \
            evalResult = EVAL_FUNCTION;                         \
            break;                                              \
        }

    switch (node->nodeData.content.keyword) {
        #include "Operations.def"
        default: { Eval (left); Eval (right); return NAN; }
    }

    #undef OPERATION

    if (!isnan (evalResult) && !isinf (evalResult)) {

        SubstituteSubtree (context, node, Const (evalResult));
        *treeChanged = true;
    }

    return evalResult;
}

static TranslationError SpecialValuesOptimizationInternal (TranslationContext *context, Tree::Node <AstNode> *node, bool *treeChanged) {
    assert (context);
    assert (node);
    assert (treeChanged);

    if (!node)
        return TranslationError::NO_ERRORS;

    if (node->nodeData.type != NodeType::KEYWORD) {
        SpecialValuesOptimizationInternal (context, node->left,  treeChanged);
        SpecialValuesOptimizationInternal (context, node->right, treeChanged);

        return TranslationError::NO_ERRORS;
    }

    #define OPERATION(KWORD, EVAL_FUNCTION, ...)                            \
        if (node->nodeData.content.keyword == Keyword::KWORD) {             \
            ProcessSpecialValue (context, node, __VA_ARGS__, treeChanged);  \
        }

    #include "Operations.def"

    #undef OPERATION

    SpecialValuesOptimizationInternal (context, node->left,  treeChanged);
    SpecialValuesOptimizationInternal (context, node->right, treeChanged);

    return TranslationError::NO_ERRORS;
}

static TranslationError ProcessSpecialValue (TranslationContext *context, Tree::Node <AstNode> *node, SpecialValue specialValue, bool *treeChanged) {
    assert (context);
    assert (node);
    assert (treeChanged);

    #define CheckForChild(dir, child)                                                                                   \
        do {                                                                                                            \
            if ((Tree::child & specialValue.validEdge) && node->dir && node->dir->nodeData.type == NodeType::CONSTANT   \
                && abs (node->dir->nodeData.content.number - specialValue.validValue) < EPS) {                          \
                SubstituteSubtree (context, node, Const (specialValue.result));                                         \
                *treeChanged = true;                                                                                    \
                return TranslationError::NO_ERRORS;                                                                     \
            }                                                                                                           \
        } while (0)

    CheckForChild (left,  LEFT_CHILD);
    CheckForChild (right, RIGHT_CHILD);

    #undef CheckForChild

    return TranslationError::NO_ERRORS;
}

