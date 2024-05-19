#include <unordered_map>

#include "AST/AstNode.h"
#include "AST/TranslationContext.h"
#include "Instruction.h"
#include "Value.h"

namespace Ast {
    IR::Value *VariableAst::Codegen (TranslationContext *context) {
        //TODO global vars
        std::unordered_map <size_t, IR::Value *>::iterator foundVariable = 
            context->localVariables.find (identifierIndex);

        if (foundVariable == context->localVariables.end ())
            return nullptr;

        IR::Value *allocaInstruction = foundVariable->second;

        return context->builder.CreateLoadInstruction (allocaInstruction);
    }

    IR::Value *VariableDeclarationAst::Codegen (TranslationContext *context) {
        IR::AllocaInstruction *newVariable = 
            static_cast <IR::AllocaInstruction *> (context->builder.CreateAllocaInstruction (nodeType->GetType ()));

        context->localVariables [identifierIndex] = newVariable;

        assignment->Codegen (context);

        return newVariable;
    }
}
