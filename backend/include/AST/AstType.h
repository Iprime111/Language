#ifndef AST_TYPE_H_
#define AST_TYPE_H_

#include <unordered_map>

#include "AST/AstNode.h"
#include "Value.h"

namespace Ast {

    class TypeOperators final {
        public:
            TypeOperators () = default;

            OperatorFunction GetOperatorCallback (AstOperatorId id);
            void             AddOperatorCallback (AstOperatorId id, OperatorFunction function);

        private:
            std::unordered_map <AstOperatorId, OperatorFunction> operators;
    };
}

#endif
