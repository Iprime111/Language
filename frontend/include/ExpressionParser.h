#ifndef EXPRESSION_PARSER_H_
#define EXPRESSION_PARSER_H_

#include "FrontendCore.h"
#include "SyntaxTree.h"
#include "TreeDefinitions.h"

Tree::Node <AstNode> *GetExpression (CompilationContext *context);

#endif
