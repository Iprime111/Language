#include <unordered_map>

#include "AST/AstNode.h"
#include "AST/AstType.h"
#include "AST/TranslationContext.h"
#include "Value.h"

namespace Ast {
    IR::Value *OperatorAst::Codegen (TranslationContext *context) {
        IR::Value *lhs = left->Codegen  (context);
        IR::Value *rhs = right->Codegen (context);

        //TODO errors logging
        if (lhs->GetType () != rhs->GetType ())
            return nullptr;

        std::unordered_map <const IR::Type *, TypeOperators>::iterator foundTypeOperators = 
            context->operators.find (lhs->GetType ());

        if (foundTypeOperators == context->operators.end ())
            return nullptr;

        OperatorFunction operatorFunction = foundTypeOperators->second.GetOperatorFunction (operatorId);

        if (operatorFunction == nullptr)
            return nullptr;

        return (this->*operatorFunction) (lhs, rhs);
    }
}
