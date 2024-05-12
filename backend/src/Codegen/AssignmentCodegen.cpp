#include <unordered_map>

#include "AST/AstNode.h"
#include "AST/TranslationContext.h"
#include "Instruction.h"

namespace Ast {
    IR::Value *AssignmentAst::Codegen (TranslationContext *context) {
        std::unordered_map <size_t, IR::AllocaInstruction *>::iterator foundLocalVariable  = 
            context->localVariables.find (variable->GetIdentifierIndex ());

        if (foundLocalVariable != context->localVariables.end ()) {
            IR::AllocaInstruction *allocaInstruction = foundLocalVariable->second;

            context->builder.CreateStoreInstruction (allocaInstruction, assignmentExpression->Codegen (context));
        } else {
            context->builder.CreateStoreInstruction (context->globalVariables [variable->GetIdentifierIndex ()], 
                                                     assignmentExpression->Codegen (context));
        }

        return nullptr;
    }
}
