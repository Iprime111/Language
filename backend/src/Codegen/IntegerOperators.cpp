#include <cassert>

#include "AST/AstNode.h"
#include "InstructionId.h"
#include "Value.h"
#include "AST/TranslationContext.h"
#include "Codegen/IntegerOperators.h"

static IR::Value *Int64Add (Ast::TranslationContext *context, Ast::AstNode *node, IR::Value *lhs, IR::Value *rhs) {
    return context->builder.CreateBinaryOperator (IR::BinaryOperatorId::ADD, lhs, rhs);
}

static IR::Value *Int64Sub (Ast::TranslationContext *context, Ast::AstNode *node, IR::Value *lhs, IR::Value *rhs) {
    return context->builder.CreateBinaryOperator (IR::BinaryOperatorId::SUB, lhs, rhs);
}

static IR::Value *Int64Mul (Ast::TranslationContext *context, Ast::AstNode *node, IR::Value *lhs, IR::Value *rhs) {
    return context->builder.CreateBinaryOperator (IR::BinaryOperatorId::MUL, lhs, rhs);
}

static IR::Value *Int64Div (Ast::TranslationContext *context, Ast::AstNode *node, IR::Value *lhs, IR::Value *rhs) {
    return context->builder.CreateBinaryOperator (IR::BinaryOperatorId::DIV, lhs, rhs);
}

static IR::Value *Int64CmpE (Ast::TranslationContext *context, Ast::AstNode *node, IR::Value *lhs, IR::Value *rhs) {
    return context->builder.CreateCmpOperator (IR::CmpOperatorId::CMP_E, lhs, rhs);
}

static IR::Value *Int64CmpL (Ast::TranslationContext *context, Ast::AstNode *node, IR::Value *lhs, IR::Value *rhs) {
    return context->builder.CreateCmpOperator (IR::CmpOperatorId::CMP_L, lhs, rhs);
}

static IR::Value *Int64CmpG (Ast::TranslationContext *context, Ast::AstNode *node, IR::Value *lhs, IR::Value *rhs) {
    return context->builder.CreateCmpOperator (IR::CmpOperatorId::CMP_G, lhs, rhs);
}

static IR::Value *Int64CmpLE (Ast::TranslationContext *context, Ast::AstNode *node, IR::Value *lhs, IR::Value *rhs) {
    return context->builder.CreateCmpOperator (IR::CmpOperatorId::CMP_LE, lhs, rhs);
}

static IR::Value *Int64CmpGE (Ast::TranslationContext *context, Ast::AstNode *node, IR::Value *lhs, IR::Value *rhs) {
    return context->builder.CreateCmpOperator (IR::CmpOperatorId::CMP_GE, lhs, rhs);
}

static IR::Value *Int64CmpNE (Ast::TranslationContext *context, Ast::AstNode *node, IR::Value *lhs, IR::Value *rhs) {
    return context->builder.CreateCmpOperator (IR::CmpOperatorId::CMP_NE, lhs, rhs);
}

#define StoreCallback(id, callback) \
    context->operators [int64Type].AddOperatorCallback (Ast::AstOperatorId::id, callback)

void RegisterIntegerOperations (Ast::TranslationContext *context) {
    assert (context);

    const IR::Type *int64Type = context->builder.GetInt64Ty ();
    
    StoreCallback (ADD,    Int64Add);
    StoreCallback (SUB,    Int64Sub);
    StoreCallback (MUL,    Int64Mul);
    StoreCallback (DIV,    Int64Div);
    StoreCallback (CMP_E,  Int64CmpE);
    StoreCallback (CMP_L,  Int64CmpL);
    StoreCallback (CMP_G,  Int64CmpG);
    StoreCallback (CMP_LE, Int64CmpLE);
    StoreCallback (CMP_GE, Int64CmpGE);
    StoreCallback (CMP_NE, Int64CmpNE);

}

#undef StoreCallback


