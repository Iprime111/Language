#include "FunctionType.h"
#include "AST/AstNode.h"

namespace Ast {
    void AstNode::ConstructCallArgumentsForChild (TranslationContext *context, std::vector <IR::Value *> *args, AstNode *child) {
        IR::Value *childCodegen = child->Codegen (context);

        if (!childCodegen)
            return;

        args->push_back (childCodegen);
    }

    ConstantAst::ConstantAst (int64_t constantValue) : constantValue (constantValue) {}

    VariableAst::VariableAst (size_t identifierIndex) : identifierIndex (identifierIndex) {}
    size_t VariableAst::GetIdentifierIndex () const { return identifierIndex; }

    IdentifierAst::IdentifierAst (size_t identifierIndex) : identifierIndex (identifierIndex) {}
    size_t IdentifierAst::GetIdentifierIndex () const { return identifierIndex; }
    
    OperatorAst::OperatorAst (AstNode *left, AstNode *right, AstOperatorId operatorId) : 
        left (left), right (right), operatorId (operatorId) {}

    TypeAst::TypeAst (const IR::Type *type) : nodeType (type) {}

    const IR::Type *TypeAst::GetType () const { return nodeType; }

    FunctionParametersAst::FunctionParametersAst (AstNode *functionContent) : 
        functionContent (functionContent) {}

    FunctionDefinitionAst::FunctionDefinitionAst (size_t identifierIndex, FunctionParametersAst *parameters, TypeAst *returnType) :
        parameters (parameters), returnType (returnType), identifierIndex (identifierIndex) {}

    OperatorSeparatorAst::OperatorSeparatorAst (AstNode *left, AstNode *right) : 
        left (left), right (right) {}

    ParameterSeparatorAst::ParameterSeparatorAst (AstNode *left, AstNode *right) :
        left (left), right (right) {}

    VariableDeclarationAst::VariableDeclarationAst (VariableAst *assignmentExpression, TypeAst *type) : 
        assignment (assignmentExpression), nodeType (type) {}

    CallAst::CallAst (IdentifierAst *functionIdentifier, AstNode *arguments) : 
        arguments (arguments), functionIdentifier (functionIdentifier) {}

    IfAst::IfAst (AstNode *condition, AstNode *body) : 
        condition (condition), body (body) {}

    ReturnAst::ReturnAst (AstNode *returnStatement) :
        returnStatement (returnStatement) {}
}
