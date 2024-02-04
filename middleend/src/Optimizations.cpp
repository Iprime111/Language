#include "Optimizations.h"
#include <math.h>

#include "CustomAssert.h"
#include "MiddleEndCore.h"
#include "NodeUtils.h"
#include "SyntaxTree.h"
#include "TreeDefinitions.h"
#include "TreeReader.h"

#define Eval(direction) EvalOptimizationInternal (context, node->direction, treeChanged)

static double           EvalOptimizationInternal          (TranslationContext *context, Tree::Node <AstNode> *node, bool *treeChanged);
static TranslationError SpecialValuesOptimizationInternal (TranslationContext *context, Tree::Node <AstNode> *node, bool *treeChanged);
static TranslationError ProcessSpecialValue               (TranslationContext *context, Tree::Node <AstNode> *node, SpecialValue specialValue, bool *treeChanged);

TranslationError DoOptimizations (TranslationContext *context) {
    PushLog (1);

    custom_assert (context,                          pointer_is_null, TranslationError::CONTEXT_ERROR);
    custom_assert (context->abstractSyntaxTree.root, pointer_is_null, TranslationError::TREE_ERROR);

    bool treeChanged = true;

    while (treeChanged) {
        treeChanged = false;

        EvalOptimizationInternal          (context, context->abstractSyntaxTree.root, &treeChanged);
        SpecialValuesOptimizationInternal (context, context->abstractSyntaxTree.root, &treeChanged);
    }

    RETURN TranslationError::NO_ERRORS;
}

TranslationError EvalOptimization (TranslationContext *context, bool *treeChanged) {
    PushLog (1);

    custom_assert (context,                          pointer_is_null, TranslationError::CONTEXT_ERROR);
    custom_assert (context->abstractSyntaxTree.root, pointer_is_null, TranslationError::TREE_ERROR);

    EvalOptimizationInternal (context, context->abstractSyntaxTree.root, treeChanged);

    RETURN TranslationError::NO_ERRORS;
}

static double EvalOptimizationInternal (TranslationContext *context, Tree::Node <AstNode> *node, bool *treeChanged) {
    PushLog (2);

    if (!node) {
        RETURN NAN;
    }

    switch (node->nodeData.type) {

        case NodeType::CONSTANT: RETURN node->nodeData.content.number;
        case NodeType::KEYWORD:  break;
        default:                 RETURN NAN;
    };

    double evalResult = NAN;

    #define OPERATION(KWORD, EVAL_FUNCTION, ...)                \
        if (node->nodeData.content.keyword == Keyword::KWORD) { \
            evalResult = EVAL_FUNCTION;                         \
        }

    #include "Operations.def"

    #undef OPERATION

    if (evalResult != NAN && evalResult != INFINITY) {
        SubstituteNode (context, node, Const (evalResult));
        *treeChanged = true;
    }

    RETURN NAN;
}

static TranslationError SpecialValuesOptimizationInternal (TranslationContext *context, Tree::Node <AstNode> *node, bool *treeChanged) {
    PushLog (2);

    if (!node) {
        RETURN TranslationError::NO_ERRORS;
    }

    if (node->nodeData.type != NodeType::KEYWORD) {
        SpecialValuesOptimizationInternal (context, node->left,  treeChanged);
        SpecialValuesOptimizationInternal (context, node->right, treeChanged);

        RETURN TranslationError::NO_ERRORS;
    }

    #define OPERATION(KWORD, EVAL_FUNCTION, ...)                            \
        if (node->nodeData.content.keyword == Keyword::KWORD) {             \
            ProcessSpecialValue (context, node, __VA_ARGS__, treeChanged);  \
        }

    #include "Operations.def"

    #undef OPERATION

    SpecialValuesOptimizationInternal (context, node->left,  treeChanged);
    SpecialValuesOptimizationInternal (context, node->right, treeChanged);

    RETURN TranslationError::NO_ERRORS;
}

static TranslationError ProcessSpecialValue (TranslationContext *context, Tree::Node <AstNode> *node, SpecialValue specialValue, bool *treeChanged) {
    PushLog (3);

    #define CheckForChild(dir, child)                                                                                   \
        do {                                                                                                            \
            if ((Tree::child & specialValue.validEdge) && node->dir && node->dir->nodeData.type == NodeType::CONSTANT   \
                && abs (node->dir->nodeData.content.number - specialValue.validValue) < EPS) {                          \
                SubstituteNode (context, node, Const (specialValue.result));                                            \
                *treeChanged = true;                                                                                    \
                RETURN TranslationError::NO_ERRORS;                                                                     \
            }                                                                                                           \
        } while (0)

    CheckForChild (left,  LEFT_CHILD);
    CheckForChild (right, RIGHT_CHILD);

    #undef CheckForChild

    RETURN TranslationError::NO_ERRORS;
}

