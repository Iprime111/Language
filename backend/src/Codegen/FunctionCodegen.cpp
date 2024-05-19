#include <cassert>
#include <unordered_map>

#include "AST/AstNode.h"
#include "AST/TranslationContext.h"
#include "Argument.h"
#include "BasicBlock.h"
#include "Function.h"
#include "Type.h"
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
        
        std::vector <const IR::Type *> argumentsTypes = {};
        std::vector <size_t>           names          = {};

        parameters->ConstructFunctionParameters (&argumentsTypes, &names);

        IR::IRContext *irContext = context->builder.GetContext ();

        IR::Function *function = IR::Function::Create (irContext, returnType->GetType (), 
                &argumentsTypes, context->nameTable [identifierIndex].c_str ());

        const std::vector <IR::Argument *> &functionArguments = function->GetArgs ();
        size_t argumentsCount = functionArguments.size ();

        context->localVariables.clear ();

        for (size_t argumentIndex = 0; argumentIndex < argumentsCount; argumentIndex++)
            context->localVariables [names [argumentIndex]] = functionArguments [argumentIndex];

        IR::BasicBlock *block  = IR::BasicBlock::Create ("entry", function, irContext);

        context->functions [identifierIndex] = function;

        context->builder.SetInsertPoint (block);

        context->isParsingCallArguments = false;
        parameters->Codegen (context);

        return function;
    }

    IR::Value *FunctionParametersAst::Codegen (TranslationContext *context) {
        return functionContent->Codegen (context);
    }

    IR::Value *ParameterSeparatorAst::Codegen (TranslationContext *context) {
        if (context->isParsingCallArguments) {
            if (left)
                left->Codegen  (context);
            
            if (right)
                right->Codegen (context);
        } else {
            if (right)
                right->Codegen (context);
            
            if (left)
                left->Codegen  (context);
        }

        return nullptr;
    }

    void ParameterSeparatorAst::ConstructCallArguments (TranslationContext *context, std::vector <IR::Value *> *args) {
        if (left && left->GetAstTypeId () == AstTypeId::PARAMETER_SEPARATOR)
            left->ConstructCallArguments (context, args);
        else
            ConstructCallArgumentsForChild (context, args, left);

        if (right && right->GetAstTypeId () == AstTypeId::PARAMETER_SEPARATOR)
            right->ConstructCallArguments (context, args);
        else
            ConstructCallArgumentsForChild (context, args, right);
    }

    void CallAst::ConstructCallArguments (TranslationContext *context, std::vector <IR::Value *> *args) {
        arguments->ConstructCallArguments (context, args);
    }

    void FunctionParametersAst::ConstructFunctionParameters (std::vector <const IR::Type *> *params, std::vector <size_t> *argumentNames) {
        if (!parameters)
            return;

        AstTypeId parametersNodeType = parameters->GetAstTypeId ();

        if (parametersNodeType == AstTypeId::PARAMETER_SEPARATOR)
            static_cast <ParameterSeparatorAst *> (parameters)->ConstructFunctionParameters (params, argumentNames);

        if (parametersNodeType == AstTypeId::VARIABLE_DECLARATION)
            static_cast <VariableDeclarationAst *> (parameters)->ConstructFunctionParameters (params, argumentNames);
    }

    void ParameterSeparatorAst::ConstructFunctionParameters (std::vector <const IR::Type *> *params, std::vector <size_t> *argumentNames) {
        #define ConstructParameters(node)                                                                               \
            if (node) {                                                                                                 \
                AstTypeId nodeTypeId = node->GetAstTypeId ();                                                           \
                if (nodeTypeId == AstTypeId::PARAMETER_SEPARATOR) {                                                     \
                    static_cast <ParameterSeparatorAst *> (node)->ConstructFunctionParameters (params, argumentNames);  \
                }                                                                                                       \
                if (nodeTypeId == AstTypeId::VARIABLE_DECLARATION) {                                                    \
                    static_cast <VariableDeclarationAst *> (node)->ConstructFunctionParameters (params, argumentNames); \
                }                                                                                                       \
            }

        ConstructParameters (left);
        ConstructParameters (right);

        #undef ConstructParameters
    }

    void VariableDeclarationAst::ConstructFunctionParameters (std::vector <const IR::Type *> *params, std::vector <size_t> *argumentNames) {
        if (!nodeType)
            return;

        argumentNames->push_back (identifierIndex);
        nodeType->ConstructFunctionParameters (params);
    }

    void TypeAst::ConstructFunctionParameters (std::vector <const IR::Type *> *params) {
        assert (params);

        params->push_back (nodeType);
    }
}
