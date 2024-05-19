#include <cassert>

#include "BasicBlock.h"
#include "Value.h"
#include "AST/AstNode.h"
#include "AST/TranslationContext.h"

namespace Ast {

    IR::Value *IfAst::Codegen (TranslationContext *context) {
        assert (context);

        IR::Function  *currentFunction = static_cast <IR::Function *> (context->builder.GetInsertBlock ()->parent);
        IR::IRContext *irContext       = context->builder.GetContext ();

        IR::BasicBlock *thenBlock  = IR::BasicBlock::Create ("if_then", currentFunction, irContext);
        IR::BasicBlock *mergeBlock = IR::BasicBlock::Create ("if_end",  currentFunction, irContext);

        trueBranch  = thenBlock;
        falseBranch = mergeBlock;

        IR::Value *ifCondition = condition->Codegen (context);

        if (condition->GetAstTypeId () != AstTypeId::LOGIC_OPERATOR) 
            context->builder.CreateBranchInstruction (ifCondition, thenBlock, mergeBlock);

        //TODO else branch
        context->builder.SetInsertPoint (thenBlock);
        body->Codegen (context);

        context->builder.SetInsertPoint (mergeBlock);

        return nullptr;
    }

    IR::Value *WhileAst::Codegen (TranslationContext *context) {
        assert (context);

        IR::BasicBlock *currentBlock    = context->builder.GetInsertBlock ();
        IR::Function   *currentFunction = static_cast <IR::Function *> (currentBlock->parent);
        IR::IRContext  *irContext       = context->builder.GetContext ();

        IR::BasicBlock *conditionBlock = IR::BasicBlock::Create ("while_condition", currentFunction, irContext);
        IR::BasicBlock *thenBlock      = IR::BasicBlock::Create ("while_then",      currentFunction, irContext);
        IR::BasicBlock *endBlock       = IR::BasicBlock::Create ("while_end",       currentFunction, irContext);

        trueBranch  = thenBlock;
        falseBranch = endBlock;

        context->builder.SetInsertPoint (conditionBlock);
        IR::Value *whileCondition = condition->Codegen (context);

        if (condition->GetAstTypeId () != AstTypeId::LOGIC_OPERATOR)
            context->builder.CreateBranchInstruction (whileCondition, thenBlock, endBlock);

        context->builder.SetInsertPoint (thenBlock);
        body->Codegen (context);
        context->builder.CreateBranchInstruction (conditionBlock);

        context->builder.SetInsertPoint (endBlock);

        return nullptr;
    }
}
