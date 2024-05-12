#ifndef AST_TYPE_H_
#define AST_TYPE_H_

#include <unordered_map>

#include "AST/AstNode.h"
#include "Value.h"

namespace Ast {
    using OperatorFunction = IR::Value *(AstNode::*) (IR::Value *lhs, IR::Value *rhs);

    class TypeOperators final {
        public:
            TypeOperators () = default;

            OperatorFunction GetOperatorFunction (AstOperatorId id);

        private:
            std::unordered_map <AstOperatorId, OperatorFunction> operators;
    };
}

#endif
