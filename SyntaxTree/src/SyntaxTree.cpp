#include "SyntaxTree.h"
#include "TreeDefinitions.h"

Tree::Node <AstNode> *EmplaceNode (Tree::Node <AstNode> node) {
    Tree::Node <AstNode> *newNode = NULL;

    Tree::InitNode (&newNode);

    if (!newNode) {
        return NULL;
    }

    *newNode = node;
    
    #define SetParent(child)                        \
        do {                                        \
            if (newNode->child) {                   \
                newNode->child->parent = newNode;   \
            }                                       \
        } while (0)

    SetParent (left);
    SetParent (right);

    #undef SetParent

    return newNode;
}
