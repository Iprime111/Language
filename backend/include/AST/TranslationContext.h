#ifndef TRANSLATION_CONTEXT_H_
#define TRANSLATION_CONTEXT_H_

#include <string>
#include <unordered_map>

#include "AST/AstType.h"
#include "IRBuilder.h"
#include "IRContext.h"
#include "Instruction.h"

namespace Ast {

    struct TranslationContext {
        std::vector <std::string> nameTable = {};
    
        IR::IRContext irContext = {};
        IR::IRBuilder builder = IR::IRBuilder (&irContext);

        std::unordered_map <size_t, std::unordered_map <size_t, size_t>> localTables = {};

        std::unordered_map <size_t, IR::AllocaInstruction *> localVariables  = {};
        std::unordered_map <size_t, IR::AllocaInstruction *> globalVariables = {};
        
        std::unordered_map <size_t, IR::Function *> functions = {};
    
        bool isParsingCallArguments = false;

        std::unordered_map <const IR::Type *, TypeOperators> operators = {};
    };
}
#endif
