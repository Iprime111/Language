#include <cassert>

#include "AST/AstNode.h"
#include "AST/TranslationContext.h"
#include "BasicBlock.h"
#include "Instruction.h"
#include "Value.h"
#include "Codegen/BoolOperations.h"

static void GenerateBranchCode (Ast::TranslationContext *context, Ast::AstNode *node, Ast::AstNode *codegenNode, IR::BasicBlock *trueBranch, IR::BasicBlock *falseBranch);

static IR::Value *BoolAnd (Ast::TranslationContext *context, Ast::AstNode *node, IR::Value *lhs, IR::Value *rhs) {

    IR::BasicBlock   *currentBlock    = context->builder.GetInsertBlock ();
    IR::Function     *currentFunction = static_cast <IR::Function *>     (currentBlock->parent);
    Ast::OperatorAst *currentOperator = static_cast <Ast::OperatorAst *> (node);

    IR::BasicBlock *secondCheck = IR::BasicBlock::Create ("and_second_condition", currentFunction, &context->irContext);

    context->builder.SetInsertPoint (currentBlock);
    GenerateBranchCode (context, node, currentOperator->GetLeft (),  secondCheck, node->parent->falseBranch);
    
    context->builder.SetInsertPoint (secondCheck);
    GenerateBranchCode (context, node, currentOperator->GetRight (), node->parent->trueBranch, node->parent->falseBranch);

    return nullptr;
}

static IR::Value *BoolOr (Ast::TranslationContext *context, Ast::AstNode *node, IR::Value *lhs, IR::Value *rhs) {

    IR::BasicBlock   *currentBlock    = context->builder.GetInsertBlock ();
    IR::Function     *currentFunction = static_cast <IR::Function *>     (currentBlock->parent);
    Ast::OperatorAst *currentOperator = static_cast <Ast::OperatorAst *> (node);

    IR::BasicBlock *secondCheck = IR::BasicBlock::Create ("or_second_condition", currentFunction, &context->irContext);

    context->builder.SetInsertPoint (currentBlock);
    GenerateBranchCode (context, node, currentOperator->GetLeft (),  node->parent->trueBranch, secondCheck);

    context->builder.SetInsertPoint (secondCheck);
    GenerateBranchCode (context, node, currentOperator->GetRight (), node->parent->trueBranch, node->parent->falseBranch);

    return nullptr;
}

static void GenerateBranchCode (Ast::TranslationContext *context, Ast::AstNode *node, Ast::AstNode *codegenNode, IR::BasicBlock *trueBranch, IR::BasicBlock *falseBranch) {
    node->trueBranch  = trueBranch;
    node->falseBranch = falseBranch;

    IR::Value *codegenResult = codegenNode->Codegen (context);

    if (codegenResult)
        context->builder.CreateBranchInstruction (codegenResult, trueBranch, falseBranch);
}

static IR::Value *BoolNot (Ast::TranslationContext *context, Ast::AstNode *node, IR::Value *lhs, IR::Value *rhs) {

    node->falseBranch = node->parent->trueBranch;
    node->trueBranch  = node->parent->falseBranch;

    Ast::LogicOperatorAst *childOperator = static_cast <Ast::LogicOperatorAst *> (node);
    
    childOperator->GetLeft  ()->Codegen (context);
    childOperator->GetRight ()->Codegen (context);

    return nullptr;
}

#define StoreCallback(id, callback) \
    context->operators [int1Type].AddOperatorCallback (Ast::AstOperatorId::id, callback)

void RegisterBoolOperations (Ast::TranslationContext *context) {
    assert (context);

    const IR::Type *int1Type = context->builder.GetInt1Ty ();
    
    StoreCallback (AND, BoolAnd);
    StoreCallback (OR,  BoolOr);
    StoreCallback (NOT, BoolNot);
}

#undef StoreCallback
