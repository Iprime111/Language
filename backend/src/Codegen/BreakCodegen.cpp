#include "AST/AstNode.h"
#include "AST/TranslationContext.h"
#include "Value.h"

namespace Ast {
    IR::Value *BreakAst::Codegen (TranslationContext *context) {
        return context->builder.CreateBranchInstruction (breakBlock);
    }

    IR::Value *ContinueAst::Codegen (TranslationContext *context) {
        return context->builder.CreateBranchInstruction (continueBlock);
    }
}
