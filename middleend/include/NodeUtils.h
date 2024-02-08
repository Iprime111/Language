#ifndef NODE_UTILS_H_
#define NODE_UTILS_H_

#include "MiddleEndCore.h"
#include "TreeDefinitions.h"
#include "TreeReader.h"

Tree::TreeEdge        GetNodeDirection  (Tree::Node <AstNode> *node);
Tree::Node <AstNode> *CopySubtree       (Tree::Node <AstNode> *subtreeRoot);

TranslationError      SubstituteNode    (TranslationContext *context, Tree::Node <AstNode> *oldNode, Tree::Node <AstNode> *newNode);
TranslationError      SubstituteSubtree (TranslationContext *context, Tree::Node <AstNode> *oldNode, Tree::Node <AstNode> *newNode);



#endif 
