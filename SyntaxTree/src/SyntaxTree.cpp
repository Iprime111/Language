#include "SyntaxTree.h"
#include "TreeDefinitions.h"

Tree::Node <AstNode> *EmplaceNode (Tree::Node <AstNode> node) {
    Tree::Node <AstNode> *newNode = nullptr;

    Tree::InitNode (&newNode);

    if (!newNode) {
        return nullptr;
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
