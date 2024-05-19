#ifndef TRANSLATION_CONTEXT_H_
#define TRANSLATION_CONTEXT_H_

#include <string>
#include <unordered_map>

#include "AST/AstNode.h"
#include "AST/AstType.h"
#include "IRBuilder.h"
#include "IRContext.h"
#include "Instruction.h"

namespace Ast {

    struct TranslationContext {
        AstNode *root = nullptr;
        std::vector <AstNode *> treeNodes = {};

        std::vector <std::string> nameTable = {};
    
        IR::IRContext irContext = {};
        IR::IRBuilder builder   = IR::IRBuilder (&irContext);

        std::unordered_map <size_t, IR::Value *> localVariables  = {};
        std::unordered_map <size_t, IR::Value *> globalVariables = {};
        
        std::unordered_map <size_t, IR::Function *> functions = {};

        size_t entryPoint = 0;
    
        bool isParsingCallArguments = false;

        std::unordered_map <const IR::Type *, TypeOperators> operators = {};

        ~TranslationContext () {
            for (size_t nodeIndex = 0; nodeIndex < treeNodes.size (); nodeIndex++)
                delete treeNodes [nodeIndex];
        }
    };
}
#endif
