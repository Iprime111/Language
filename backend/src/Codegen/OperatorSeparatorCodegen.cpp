#include "AST/AstNode.h"
namespace Ast {

    IR::Value *OperatorSeparatorAst::Codegen (TranslationContext *context) {
        if (left) {
            left->continueBlock = continueBlock;
            left->breakBlock    = breakBlock;

            left->Codegen (context);
        }

        if (right) {
            right->continueBlock = continueBlock;
            right->breakBlock    = breakBlock;

            right->Codegen (context);
        }

        return nullptr;
    }
}
