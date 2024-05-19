#include "AST/AstNode.h"
namespace Ast {

    IR::Value *OperatorSeparatorAst::Codegen (TranslationContext *context) {
        if (left)
            left->Codegen (context);

        if (right)
            right->Codegen (context);

        return nullptr;
    }
}
