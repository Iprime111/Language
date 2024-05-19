#include "AST/AstNode.h"
#include "AST/TranslationContext.h"

namespace Ast {
    IR::Value *ReturnAst::Codegen (TranslationContext *context) {
        return context->builder.CreateReturnOperator (returnStatement->Codegen (context));
    }
}
