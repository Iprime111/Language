#include <unordered_map>

#include "AST/AstType.h"
#include "AST/AstNode.h"

namespace Ast {
    OperatorFunction TypeOperators::GetOperatorCallback (AstOperatorId id) {
        std::unordered_map <AstOperatorId, OperatorFunction>::iterator foundFunction = operators.find (id);

        if (foundFunction == operators.end ())
            return nullptr;

        return foundFunction->second;
    }

    void TypeOperators::AddOperatorCallback (AstOperatorId id, OperatorFunction function) {
        operators [id] = function;
    }
}
