#ifndef SYNTAX_TREE_H_
#define SYNTAX_TREE_H_

#include <math.h>

#include "Tree.h"

enum class NodeType {
    NAME     = 1 << 0,
    CONSTANT = 1 << 1,
};

union NodeContent {
    double number = NAN;
    size_t nameTableIndex;
};

struct AstNode {
    NodeType    type    = NodeType::CONSTANT;
    NodeContent content = {.number = NAN};
};

Tree::Node <AstNode> *EmplaceNode (Tree::Node <AstNode> node);

#define Const(NUMBER) EmplaceNode(Tree::Node <AstNode> {.left = NULL, .right = NULL, .parent = NULL, .nodeData = {.type = NodeType::CONSTANT,   .content = {.number         = NUMBER}}})
#define Name(INDEX)   EmplaceNode(Tree::Node <AstNode> {.left = NULL, .right = NULL, .parent = NULL, .nodeData = {.type = NodeType::NAME,       .content = {.nameTableIndex = INDEX}}})

#endif
