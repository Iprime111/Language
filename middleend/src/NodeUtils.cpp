#include <cassert>

#include "MiddleEndCore.h"
#include "SyntaxTree.h"
#include "TreeDefinitions.h"
#include "TreeReader.h"
#include "NodeUtils.h"

static TranslationError SubstituteTreeSegment (TranslationContext *context, Tree::Node<AstNode> *oldNode, Tree::Node<AstNode> *newNode);

TranslationError SubstituteNode (TranslationContext *context, Tree::Node <AstNode> *oldNode, Tree::Node <AstNode> *newNode) {
    assert (context);
    assert (oldNode);
    assert (newNode);

    TranslationError error = SubstituteTreeSegment (context, oldNode, newNode);
    
    Tree::DestroySingleNode (oldNode);

    return error;
}

TranslationError SubstituteSubtree (TranslationContext *context, Tree::Node <AstNode> *oldNode, Tree::Node <AstNode> *newNode) {
    assert (context);
    assert (oldNode);
    assert (newNode);

    TranslationError error = SubstituteTreeSegment (context, oldNode, newNode);
    
    Tree::DestroySubtreeNode (&context->abstractSyntaxTree, oldNode);

    return error;
}

static TranslationError SubstituteTreeSegment (TranslationContext *context, Tree::Node<AstNode> *oldNode, Tree::Node<AstNode> *newNode) {
    assert (context);
    assert (oldNode);
    assert (newNode);

    Tree::TreeEdge edge = GetNodeDirection (oldNode);

    if (edge == Tree::LEFT_CHILD)
        oldNode->parent->left  = newNode;
    else if (edge == Tree::RIGHT_CHILD)
        oldNode->parent->right = newNode; 

    if (newNode)
        newNode->parent = oldNode->parent;

    return TranslationError::NO_ERRORS;
}

Tree::TreeEdge GetNodeDirection (Tree::Node <AstNode> *node) {
    assert (node);

    if (!node || !node->parent || !(node->parent->left || node->parent->right))
        return Tree::NO_EDGE;

    if (node == node->parent->left)
        return Tree::LEFT_CHILD;

    return Tree::RIGHT_CHILD;

}

Tree::Node <AstNode> *CopySubtree (Tree::Node <AstNode> *subtreeRoot) {
    assert (subtreeRoot);

    Tree::Node <AstNode> *rootCopy = {};
    Tree::InitNode (&rootCopy);

    rootCopy->nodeData = subtreeRoot->nodeData;

    #define CopyRootSubtree(direction)                                      \
        if (subtreeRoot->direction) {                                       \
            rootCopy->direction = CopySubtree (subtreeRoot->direction);     \
            rootCopy->direction->parent = rootCopy;                         \
        }

    CopyRootSubtree (left);
    CopyRootSubtree (right);

    #undef CopyRootSubtree

    return rootCopy;
}

