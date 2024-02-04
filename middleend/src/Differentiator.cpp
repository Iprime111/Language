#include <math.h>

#include "MiddleEndCore.h"
#include "SyntaxTree.h"
#include "DSL.h"

double EvalSubtree (TranslationContext *context, Tree::Node <AstNode> *node) {
    PushLog (1);

    if (!node) {
        RETURN NAN;
    }

    switch (node->nodeData.type) {

        case NodeType::CONSTANT: RETURN node->nodeData.content.number;
        case NodeType::KEYWORD:  break;
        default:                 RETURN NAN;
    };

    #define OPERATION(KWORD, EVAL_FUNCTION, ...)                \
        if (node->nodeData.content.keyword == Keyword::KWORD) { \
            RETURN (EVAL_FUNCTION);                             \
        }

    #include "Operations.def"

    #undef OPERATION

    RETURN NAN;
}



