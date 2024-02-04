#include "MiddleEndCore.h"
#include "SyntaxTree.h"
#include "TreeDefinitions.h"
#include "TreeReader.h"
#include "NodeUtils.h"

TranslationError SubstituteNode (TranslationContext *context, Tree::Node <AstNode> *oldNode, Tree::Node <AstNode> *newNode) {
    PushLog (4);

    Tree::TreeEdge edge = GetNodeDirection (oldNode);

    newNode->parent = oldNode->parent;

    if (edge == Tree::LEFT_CHILD) {
        newNode->parent->left  = newNode;
        RETURN TranslationError::NO_ERRORS;

    } else if (edge == Tree::RIGHT_CHILD) {
        newNode->parent->right = newNode; 
        RETURN TranslationError::NO_ERRORS;
    }

    Tree::DestroySubtreeNode (&context->abstractSyntaxTree, oldNode);

    RETURN TranslationError::TREE_ERROR;
}

Tree::TreeEdge GetNodeDirection (Tree::Node <AstNode> *node) {
    PushLog (4);

    if (!node || !node->parent || !(node->parent->left || node->parent->right)) {
        RETURN Tree::NO_EDGE;
    }

    if (node == node->parent->left) {
        RETURN Tree::LEFT_CHILD;
    }

    RETURN Tree::RIGHT_CHILD;

}

