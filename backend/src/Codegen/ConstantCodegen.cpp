#include "AST/AstNode.h"
#include "AST/TranslationContext.h"

namespace Ast {
    IR::Value *ConstantAst::Codegen (TranslationContext *context) {
        return IR::ConstantData::CreateConstant (context->builder.GetContext (), context->builder.GetInt64Ty (), &constantValue);
    }
}
