#ifndef DSL_H_
#define DSL_H_

#define Eval(direction) EvalSubtree (context, node->direction)

#define Diff(direction) DifferentiateInternal (context, node->direction, identifierIndex)
#define Copy(direction) CopySubtree (node->direction)

#define OperationNode(kword, leftChild, rightChild)                                                                     \
    EmplaceNode (Tree::Node<AstNode> {.left = leftChild, .right = rightChild, .parent = NULL,                           \
                                      .nodeData = {.type = NodeType::KEYWORD, .content = {.keyword = Keyword::kword}}})

#define Add(leftChild, rightChild) OperationNode (ADD,  leftChild, rightChild)
#define Sub(leftChild, rightChild) OperationNode (SUB,  leftChild, rightChild)
#define Mul(leftChild, rightChild) OperationNode (MUL,  leftChild, rightChild)
#define Div(leftChild, rightChild) OperationNode (DIV,  leftChild, rightChild)
#define Sin(rightChild)            OperationNode (SIN,  NULL, rightChild)
#define Cos(rightChild)            OperationNode (COS,  NULL, rightChild)
#define Sqrt(rightChild)           OperationNode (SQRT, NULL, rightChild)

#endif

