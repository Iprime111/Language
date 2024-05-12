#include "Value.h"
#include "AST/AstNode.h"
#include "AST/TranslationContext.h"

namespace Ast {
    IR::Value *IfAst::Codegen (TranslationContext *context) {
        IR::Function *currentFunction =  static_cast <IR::Function *> (context->builder.GetInsertBlock ()->parent);

        IR::Value *ifCondition = condition->Codegen (context);
        IR::IRContext *irContext = context->builder.GetContext ();

        IR::BasicBlock *thenBlock  = IR::BasicBlock::Create ("Then block (\"if\")", currentFunction, irContext);
        IR::BasicBlock *mergeBlock = IR::BasicBlock::Create ("Merge after \"if\"",  currentFunction, irContext);

        //TODO else branch
        context->builder.CreateBranchInstruction (ifCondition, thenBlock, mergeBlock);

        //TODO special function for 'or', 'not' and 'and' 
        context->builder.SetInsertPoint (thenBlock);
        body->Codegen (context);

        context->builder.SetInsertPoint (mergeBlock);
    }

}
