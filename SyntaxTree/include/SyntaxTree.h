#ifndef SYNTAX_TREE_H_
#define SYNTAX_TREE_H_

#include <math.h>

#include "NameTable.h"
#include "Tree.h"

enum class NodeType {
    TERMINATOR           = 0,
    CONSTANT             = 1,
    STRING               = 2,
    KEYWORD              = 3,
    FUNCTION_DEFINITION  = 4,
    FUNCTION_ARGUMENTS   = 5,
    VARIABLE_DECLARATION = 6,
    FUNCTION_CALL        = 7,
};

union NodeContent {
    double  number = NAN;
    size_t  nameTableIndex;
    Keyword keyword;
};

struct AstNode {
    NodeType    type    = NodeType::CONSTANT;
    NodeContent content = {.number = NAN};
    int         line    = 0;
    char       *file    = NULL;
};

Tree::Node <AstNode> *EmplaceNode (Tree::Node <AstNode> node);

#define Const(NUMBER) EmplaceNode (Tree::Node <AstNode> {.left = NULL, .right = NULL, .parent = NULL, .nodeData = {.type = NodeType::CONSTANT,  .content = {.number         = NUMBER}, .line = context->currentLine}})
#define Name(INDEX)   EmplaceNode (Tree::Node <AstNode> {.left = NULL, .right = NULL, .parent = NULL, .nodeData = {.type = NodeType::STRING,    .content = {.nameTableIndex = INDEX},  .line = context->currentLine}})

#define FunctionDefinition(leftChild, rightChild, identifierIndex)  EmplaceNode (Tree::Node <AstNode> {.left = leftChild, .right = rightChild, .parent = NULL, .nodeData = {.type = NodeType::FUNCTION_DEFINITION,  .content = {.nameTableIndex = identifierIndex}}})
#define VariableDeclaration(leftChild, rightChild, identifierIndex) EmplaceNode (Tree::Node <AstNode> {.left = leftChild, .right = rightChild, .parent = NULL, .nodeData = {.type = NodeType::VARIABLE_DECLARATION, .content = {.nameTableIndex = identifierIndex}}})
#define FunctionArguments(leftChild, rightChild)                    EmplaceNode (Tree::Node <AstNode> {.left = leftChild, .right = rightChild, .parent = NULL, .nodeData = {.type = NodeType::FUNCTION_ARGUMENTS}})
#define FunctionCall(leftChild, rightChild)                         EmplaceNode (Tree::Node <AstNode> {.left = leftChild, .right = rightChild, .parent = NULL, .nodeData = {.type = NodeType::FUNCTION_CALL}})

#define Terminator() EmplaceNode (Tree::Node <AstNode> {.left = NULL, .right = NULL, .parent = NULL, .nodeData = {.type = NodeType::TERMINATOR}})

#define OperatorSeparator(leftChild, rightChild) EmplaceNode (Tree::Node <AstNode> {.left = leftChild, .right = rightChild, .parent = NULL, .nodeData = {.type = NodeType::STRING, .content = {.nameTableIndex = 0}}});
#define ArgumentSeparator(leftChild, rightChild) EmplaceNode (Tree::Node <AstNode> {.left = leftChild, .right = rightChild, .parent = NULL, .nodeData = {.type = NodeType::STRING, .content = {.nameTableIndex = 1}}});

#endif
