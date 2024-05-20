#include <cassert>

#include "AST/AstNode.h"
#include "InstructionId.h"
#include "Value.h"
#include "AST/TranslationContext.h"
#include "Codegen/DoubleOperators.h"

static IR::Value *DoubleAdd (Ast::TranslationContext *context, Ast::AstNode *node, IR::Value *lhs, IR::Value *rhs) {
    return context->builder.CreateBinaryOperator (IR::BinaryOperatorId::ADD, lhs, rhs);
}

static IR::Value *DoubleSub (Ast::TranslationContext *context, Ast::AstNode *node, IR::Value *lhs, IR::Value *rhs) {
    return context->builder.CreateBinaryOperator (IR::BinaryOperatorId::SUB, lhs, rhs);
}

static IR::Value *DoubleMul (Ast::TranslationContext *context, Ast::AstNode *node, IR::Value *lhs, IR::Value *rhs) {
    return context->builder.CreateBinaryOperator (IR::BinaryOperatorId::MUL, lhs, rhs);
}

static IR::Value *DoubleDiv (Ast::TranslationContext *context, Ast::AstNode *node, IR::Value *lhs, IR::Value *rhs) {
    return context->builder.CreateBinaryOperator (IR::BinaryOperatorId::DIV, lhs, rhs);
}

static IR::Value *DoubleSqrt (Ast::TranslationContext *context, Ast::AstNode *node, IR::Value *lhs, IR::Value *rhs) {
    return context->builder.CreateUnaryOperator (IR::UnaryOperatorId::SQRT, rhs);
}

static IR::Value *DoubleCmpE (Ast::TranslationContext *context, Ast::AstNode *node, IR::Value *lhs, IR::Value *rhs) {
    return context->builder.CreateCmpOperator (IR::CmpOperatorId::CMP_E, lhs, rhs);
}

static IR::Value *DoubleCmpL (Ast::TranslationContext *context, Ast::AstNode *node, IR::Value *lhs, IR::Value *rhs) {
    return context->builder.CreateCmpOperator (IR::CmpOperatorId::CMP_L, lhs, rhs);
}

static IR::Value *DoubleCmpG (Ast::TranslationContext *context, Ast::AstNode *node, IR::Value *lhs, IR::Value *rhs) {
    return context->builder.CreateCmpOperator (IR::CmpOperatorId::CMP_G, lhs, rhs);
}

static IR::Value *DoubleCmpLE (Ast::TranslationContext *context, Ast::AstNode *node, IR::Value *lhs, IR::Value *rhs) {
    return context->builder.CreateCmpOperator (IR::CmpOperatorId::CMP_LE, lhs, rhs);
}

static IR::Value *DoubleCmpGE (Ast::TranslationContext *context, Ast::AstNode *node, IR::Value *lhs, IR::Value *rhs) {
    return context->builder.CreateCmpOperator (IR::CmpOperatorId::CMP_GE, lhs, rhs);
}

static IR::Value *DoubleCmpNE (Ast::TranslationContext *context, Ast::AstNode *node, IR::Value *lhs, IR::Value *rhs) {
    return context->builder.CreateCmpOperator (IR::CmpOperatorId::CMP_NE, lhs, rhs);
}

#define StoreCallback(id, callback) \
    context->operators [doubleType].AddOperatorCallback (Ast::AstOperatorId::id, callback)

void RegisterDoubleOperations (Ast::TranslationContext *context) {
    assert (context);

    const IR::Type *doubleType = context->builder.GetDoubleTy ();
    
    StoreCallback (ADD,    DoubleAdd);
    StoreCallback (SUB,    DoubleSub);
    StoreCallback (MUL,    DoubleMul);
    StoreCallback (DIV,    DoubleDiv);
    StoreCallback (SQRT,   DoubleSqrt);
    StoreCallback (CMP_E,  DoubleCmpE);
    StoreCallback (CMP_L,  DoubleCmpL);
    StoreCallback (CMP_G,  DoubleCmpG);
    StoreCallback (CMP_LE, DoubleCmpLE);
    StoreCallback (CMP_GE, DoubleCmpGE);
    StoreCallback (CMP_NE, DoubleCmpNE);

}

#undef StoreCallback


