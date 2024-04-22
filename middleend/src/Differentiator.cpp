#include <cassert>
#include <math.h>

#include "MiddleEndCore.h"
#include "NameTable.h"
#include "NodeUtils.h"
#include "SyntaxTree.h"
#include "DSL.h"
#include "TreeDefinitions.h"
#include "TreeReader.h"
#include "Differentiator.h"

static Tree::Node <AstNode> *DifferentiateInternal (TranslationContext *context, Tree::Node <AstNode> *node, size_t identifierIndex);

double EvalSubtree (TranslationContext *context, Tree::Node <AstNode> *node) {
    assert (context);
    assert (node);

    if (!node)
        return NAN;

    switch (node->nodeData.type) {

        case NodeType::CONSTANT: return node->nodeData.content.number;
        case NodeType::KEYWORD:  break;
        default:                 return NAN;
    };

    #define OPERATION(KWORD, EVAL_FUNCTION, ...)                \
        if (node->nodeData.content.keyword == Keyword::KWORD) { \
            return (EVAL_FUNCTION);                             \
        }

    #include "Operations.def"

    #undef OPERATION

    return NAN;
}

TranslationError DifferentiationTraversal (TranslationContext *context, Tree::Node <AstNode> *node) {
    assert (context);
    assert (node);

    if (!node)
        return TranslationError::NO_ERRORS;

    if (node->nodeData.type == NodeType::KEYWORD && node->nodeData.content.keyword == Keyword::DIFF) {
        if (!node->left)
            return TranslationError::TREE_ERROR;

        Tree::Node <AstNode> *newNode = DifferentiateInternal (context, node->right, node->left->nodeData.content.nameTableIndex);

        Tree::DestroySingleNode (node->left);
        SubstituteNode (context, node, newNode);

        node = newNode;
    }

    DifferentiationTraversal (context, node->left);
    DifferentiationTraversal (context, node->right);

    return TranslationError::NO_ERRORS;
}

static Tree::Node <AstNode> *DifferentiateInternal (TranslationContext *context, Tree::Node <AstNode> *node, size_t identifierIndex) {
    assert (context);
    assert (node);

    if (!node)
        return NULL;

    Tree::Node <AstNode> *newNode = NULL;

    if (node->nodeData.type == NodeType::CONSTANT ||
        (node->nodeData.type == NodeType::STRING && node->nodeData.content.nameTableIndex != identifierIndex)) {

        newNode = Const (0);
        SubstituteNode (context, node, NULL);
        
        return newNode;
    }

    if (node->nodeData.type == NodeType::STRING && node->nodeData.content.nameTableIndex == identifierIndex) {
        
        newNode = Const (1);
        SubstituteNode (context, node, NULL);
        
        return newNode;
    }

    if (node->nodeData.type != NodeType::KEYWORD)
        return node;


    switch (node->nodeData.content.keyword) {
        case Keyword::ADD: {
            newNode = Add (Diff (left), Diff (right));
            break;
        }

        case Keyword::SUB: {
            newNode = Sub (Diff (left), Diff (right));
            break;
        }

        case Keyword::SIN: {
            Tree::Node <AstNode> *argumentCopy = Copy (right);

            newNode = Mul (Diff (right), Cos (argumentCopy));
            break;
        }

        case Keyword::COS: {
            Tree::Node <AstNode> *argumentCopy = Copy (right);

            newNode = Mul (Diff (right), Mul (Const (-1), Sin (argumentCopy)));
            break;
        }

        case Keyword::MUL: {
            Tree::Node <AstNode> *leftCopy  = Copy (left);
            Tree::Node <AstNode> *rightCopy = Copy (right);

            newNode = Add (Mul (Diff (left), rightCopy), Mul (Diff (right), leftCopy));
            break;
        }

        case Keyword::DIV: {
            Tree::Node <AstNode> *leftCopy  = Copy (left);
            Tree::Node <AstNode> *rightCopy = Copy (right);
            Tree::Node <AstNode> *div       = Mul (Copy (right), Copy (right));

            newNode = Div (Sub (Mul (Diff (left), rightCopy), Mul (Diff (right), leftCopy)), div);
            break;
        }

        case Keyword::SQRT: {
            Tree::Node <AstNode> *argumentCopy = Copy (right);

            newNode = Div (Diff (right), Mul (Const (2), Sqrt (argumentCopy)));
            break;
        }

        default:
            return node;
    }
    
    SubstituteNode (context, node, NULL);
    return newNode;
}

