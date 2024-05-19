#include <cassert>

#include "AST/AstNode.h"
#include "AST/TranslationContext.h"
#include "Value.h"

namespace Ast {
    IR::Value *OperatorAst::Codegen (TranslationContext *context) {
        assert (context);

        IR::Value *lhs = nullptr;
        IR::Value *rhs = nullptr;

        if (left)
            lhs = left->Codegen  (context);

        if (right)
            rhs = right->Codegen (context);

        if (!rhs)
            return nullptr;

        //TODO errors logging
        if (lhs)
            if (lhs->GetType () != rhs->GetType ())
                return nullptr;

        OperatorFunction operatorFunction = GetOperatorFunction (context, rhs->GetType (), operatorId);
        
        if (!operatorFunction)
            return nullptr;

        return operatorFunction (context, this, lhs, rhs);
    }

    IR::Value *LogicOperatorAst::Codegen (TranslationContext *context) {
        assert (context);

        OperatorFunction operatorFunction = GetOperatorFunction (context, context->builder.GetInt1Ty (), GetOperatorId ());
        
        return operatorFunction (context, this, nullptr, nullptr);
    }
}
