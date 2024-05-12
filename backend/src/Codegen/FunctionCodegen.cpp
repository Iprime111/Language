#include <cassert>
#include <unordered_map>

#include "AST/AstNode.h"
#include "AST/TranslationContext.h"
#include "BasicBlock.h"
#include "Function.h"
#include "FunctionType.h"
#include "IRContext.h"
#include "Value.h"

namespace Ast {
    IR::Value *CallAst::Codegen (TranslationContext *context) {
        std::unordered_map <size_t, IR::Function *>::iterator foundFunction = 
            context->functions.find (functionIdentifier->GetIdentifierIndex ());

        //TODO be able call a function defined anywhere
        if (foundFunction == context->functions.end ())
            return nullptr;

        IR::Function *function = foundFunction->second;

        std::vector <IR::Value *> callArguments = {};

        context->isParsingCallArguments = true;
        ConstructCallArguments (context, &callArguments);

        return context->builder.CreateCall (function, &callArguments);
    }

    IR::Value *FunctionDefinitionAst::Codegen (TranslationContext *context) {
        
        IR::FunctionType type = {
            .returnValue = returnType->GetType (),
            .params      = {},
        };

        parameters->ConstructFunctionParameters (context, &type.params);

        IR::IRContext *irContext = context->builder.GetContext ();

        IR::Function *function = IR::Function::Create   (irContext, &type, context->nameTable [identifierIndex].c_str ());
        IR::BasicBlock *block  = IR::BasicBlock::Create ("function begin", function, irContext);

        context->builder.SetInsertPoint (block);

        context->isParsingCallArguments = false;
        parameters->Codegen (context);

        context->localVariables.clear ();

        return function;
    }

    IR::Value *ParameterSeparatorAst::Codegen (TranslationContext *context) {
        if (context->isParsingCallArguments) {
            left->Codegen  (context);
            right->Codegen (context);
        } else {
            right->Codegen (context);
            left->Codegen  (context);
        }

        return nullptr;
    }

    void OperatorAst::ConstructCallArguments (TranslationContext *context, std::vector <IR::Value *> *args) {
        ConstructCallArgumentsForChild (context, args, left);
        ConstructCallArgumentsForChild (context, args, right);
    }

    void ParameterSeparatorAst::ConstructCallArguments (TranslationContext *context, std::vector <IR::Value *> *args) {
        ConstructCallArgumentsForChild (context, args, left);
        ConstructCallArgumentsForChild (context, args, right);
    }

    void CallAst::ConstructCallArguments (TranslationContext *context, std::vector <IR::Value *> *args) {
        ConstructCallArgumentsForChild (context, args, arguments);
    }

    void FunctionParametersAst::ConstructFunctionParameters (TranslationContext *context, std::vector <const IR::Type *> *params) {
        if (!parameters)
            return;

        parameters->ConstructFunctionParameters (context, params);
    }

    void ParameterSeparatorAst::ConstructFunctionParameters (TranslationContext *context, std::vector <const IR::Type *> *params) {

        if (left)
            left->ConstructFunctionParameters  (context, params);

        if (right)
            right->ConstructFunctionParameters (context, params);
    }

    void VariableDeclarationAst::ConstructFunctionParameters (TranslationContext *context, std::vector <const IR::Type *> *params) {
        if (!nodeType)
            return;

        nodeType->ConstructFunctionParameters (context, params);
    }

    void TypeAst::ConstructFunctionParameters (TranslationContext *context, std::vector <const IR::Type *> *params) {
        assert (context);
        assert (params);

        params->push_back (nodeType);
    }
}
