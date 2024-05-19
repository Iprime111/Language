#include <cassert>

#include "AST/TranslationContext.h"
#include "Value.h"

namespace Ast {
    IR::Value *InAst::Codegen (TranslationContext *context) {
        assert (context);

        return context->builder.CreateInInstruction ();
    }

    IR::Value *OutAst::Codegen (TranslationContext *context) {
        assert (context);

        return context->builder.CreateOutInstruction (outExpression->Codegen (context));
    }
}
