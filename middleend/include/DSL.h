#ifndef DSL_H_
#define DSL_H_

#define Eval(direction) EvalSubtree (context, node->direction)

#define Diff(direction) DifferentiateInternal (context, node->direction, identifierIndex)
#define Copy(direction) CopySubtree (node->direction)

#define OperationNode(kword, leftChild, rightChild)                                                                     \
    EmplaceNode (Tree::Node<AstNode> {.left = leftChild, .right = rightChild, .parent = nullptr,                        \
                                      .nodeData = {.type = NodeType::KEYWORD, .content = {.keyword = Keyword::kword}}})

#define Add(leftChild, rightChild) OperationNode (ADD,  leftChild, rightChild)
#define Sub(leftChild, rightChild) OperationNode (SUB,  leftChild, rightChild)
#define Mul(leftChild, rightChild) OperationNode (MUL,  leftChild, rightChild)
#define Div(leftChild, rightChild) OperationNode (DIV,  leftChild, rightChild)
#define Sin(rightChild)            OperationNode (SIN,  nullptr, rightChild)
#define Cos(rightChild)            OperationNode (COS,  nullptr, rightChild)
#define Sqrt(rightChild)           OperationNode (SQRT, nullptr, rightChild)

#endif

