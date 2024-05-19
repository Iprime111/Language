#include "Type.h"
#include "AST/AstNode.h"
#include "AST/TranslationContext.h"

#define SetParent(node)             \
    do {                            \
        if (node) {                 \
            node->parent = this;    \
        }                           \
    } while (0)

namespace Ast {
    //--------------------------------------------------------------------------------------------------------------------------------
    //------------------------------------------------------------AstNode-------------------------------------------------------------
    //--------------------------------------------------------------------------------------------------------------------------------

    AstNode::AstNode (AstTypeId astTypeId) : parent (nullptr), trueBranch (nullptr), falseBranch (nullptr), typeId (astTypeId) {}

    AstTypeId AstNode::GetAstTypeId () const { return typeId; }

    void ParameterSeparatorAst::ConstructCallArgumentsForChild (TranslationContext *context, std::vector <IR::Value *> *args, AstNode *child) {
        if (!child)
            return;

        IR::Value *childCodegen = child->Codegen (context);

        if (!childCodegen)
            return;

        args->push_back (childCodegen);
    }

    //--------------------------------------------------------------------------------------------------------------------------------
    //------------------------------------------------------------ConstantAst---------------------------------------------------------
    //--------------------------------------------------------------------------------------------------------------------------------

    ConstantAst::ConstantAst (int64_t constant) : AstNode (AstTypeId::CONSTANT), constantValue (constant) {}

    //--------------------------------------------------------------------------------------------------------------------------------
    //------------------------------------------------------------VariableAst---------------------------------------------------------
    //--------------------------------------------------------------------------------------------------------------------------------

    VariableAst::VariableAst (size_t identifier) : AstNode (AstTypeId::VARIABLE), identifierIndex (identifier) {}
    size_t VariableAst::GetIdentifierIndex () const { return identifierIndex; }

    //--------------------------------------------------------------------------------------------------------------------------------
    //------------------------------------------------------------IdentifierAst-------------------------------------------------------
    //--------------------------------------------------------------------------------------------------------------------------------

    IdentifierAst::IdentifierAst (size_t identifier) : AstNode(AstTypeId::IDENTIFIER), identifierIndex (identifier) {}
    size_t IdentifierAst::GetIdentifierIndex () const { return identifierIndex; }
    
    //--------------------------------------------------------------------------------------------------------------------------------
    //------------------------------------------------------------OperatorAst---------------------------------------------------------
    //--------------------------------------------------------------------------------------------------------------------------------

    OperatorAst::OperatorAst (AstNode *leftNode, AstNode *rightNode, AstOperatorId astOperatorId) : 
        AstNode(AstTypeId::OPERATOR), left (leftNode), right (rightNode), operatorId (astOperatorId) {

        SetParent (right);
        SetParent (left);
    }

    OperatorAst::OperatorAst (AstNode *leftNode, AstNode *rightNode, AstOperatorId astOperatorId, AstTypeId type) : 
        AstNode (type), left (leftNode), right (rightNode), operatorId (astOperatorId) {

        SetParent (right);
        SetParent (left);
    }

    AstOperatorId OperatorAst::GetOperatorId () const { return operatorId; }
    AstNode *OperatorAst::GetLeft  () const { return left; }
    AstNode *OperatorAst::GetRight () const { return right; }

    OperatorFunction OperatorAst::GetOperatorFunction (TranslationContext *context, const IR::Type *type, AstOperatorId operatorId) {
        if (!context || !type)
            return nullptr;

        std::unordered_map <const IR::Type *, TypeOperators>::iterator foundTypeOperators = 
            context->operators.find (type);

        if (foundTypeOperators == context->operators.end ())
            return nullptr;

        return foundTypeOperators->second.GetOperatorCallback (operatorId);
    }

    //--------------------------------------------------------------------------------------------------------------------------------
    //------------------------------------------------------------LogicOperatorAst----------------------------------------------------
    //--------------------------------------------------------------------------------------------------------------------------------

    LogicOperatorAst::LogicOperatorAst (AstNode *leftNode, AstNode *rightNode, AstOperatorId astOperatorId) : 
        OperatorAst (leftNode, rightNode, astOperatorId, AstTypeId::LOGIC_OPERATOR) {}

    //--------------------------------------------------------------------------------------------------------------------------------
    //------------------------------------------------------------TypeAst-------------------------------------------------------------
    //--------------------------------------------------------------------------------------------------------------------------------

    TypeAst::TypeAst (const IR::Type *type) : AstNode (AstTypeId::TYPE), nodeType (type) {}

    const IR::Type *TypeAst::GetType () const { return nodeType; }

    //--------------------------------------------------------------------------------------------------------------------------------
    //------------------------------------------------------------FunctionParametersAst-----------------------------------------------
    //--------------------------------------------------------------------------------------------------------------------------------

    FunctionParametersAst::FunctionParametersAst (AstNode *functionParameters, AstNode *functionBody) : 
        AstNode (AstTypeId::FUNCTION_PARAMETERS), parameters (functionParameters), functionContent (functionBody) {

        SetParent (parameters);
        SetParent (functionContent);
    }

    //--------------------------------------------------------------------------------------------------------------------------------
    //------------------------------------------------------------FunctionDefinitionAst-----------------------------------------------
    //--------------------------------------------------------------------------------------------------------------------------------

    FunctionDefinitionAst::FunctionDefinitionAst (size_t identifier, FunctionParametersAst *parametersNode, TypeAst *returnValueType) :
        AstNode (AstTypeId::FUNCTION_DEFINITION), parameters (parametersNode), returnType (returnValueType), identifierIndex (identifier) {

        SetParent (parameters);
        SetParent (returnType);
    }

    //--------------------------------------------------------------------------------------------------------------------------------
    //------------------------------------------------------------OperatorSeparatorAst------------------------------------------------
    //--------------------------------------------------------------------------------------------------------------------------------

    OperatorSeparatorAst::OperatorSeparatorAst (AstNode *leftNode, AstNode *rightNode) : 
        AstNode (AstTypeId::OPERATOR_SEPARATOR), left (leftNode), right (rightNode) {
     
        SetParent (left);
        SetParent (right);
    }

    //--------------------------------------------------------------------------------------------------------------------------------
    //------------------------------------------------------------ParameterSeparatorAst-----------------------------------------------
    //--------------------------------------------------------------------------------------------------------------------------------

    ParameterSeparatorAst::ParameterSeparatorAst (AstNode *leftNode, AstNode *rightNode) :
        AstNode (AstTypeId::PARAMETER_SEPARATOR), left (leftNode), right (rightNode) {

        SetParent (left);
        SetParent (right);
    }

    //--------------------------------------------------------------------------------------------------------------------------------
    //------------------------------------------------------------VariableDeclarationAst----------------------------------------------
    //--------------------------------------------------------------------------------------------------------------------------------

    VariableDeclarationAst::VariableDeclarationAst (size_t identifier, AstNode *assignmentExpression, TypeAst *type) : 
        AstNode (AstTypeId::VARIABLE_DECLARATION), identifierIndex (identifier), assignment (assignmentExpression), nodeType (type) {

        SetParent (assignmentExpression);
        SetParent (type);
    }

    //--------------------------------------------------------------------------------------------------------------------------------
    //------------------------------------------------------------CallAst-------------------------------------------------------------
    //--------------------------------------------------------------------------------------------------------------------------------

    CallAst::CallAst (IdentifierAst *identifier, AstNode *functionArguments) : 
        AstNode (AstTypeId::CALL), arguments (functionArguments), functionIdentifier (identifier) {

        SetParent (functionIdentifier);
        SetParent (arguments);
    }

    //--------------------------------------------------------------------------------------------------------------------------------
    //------------------------------------------------------------IfAst---------------------------------------------------------------
    //--------------------------------------------------------------------------------------------------------------------------------

    IfAst::IfAst (AstNode *ifCondition, AstNode *ifBody) :
        AstNode (AstTypeId::IF), condition (ifCondition), body (ifBody) {

        SetParent (condition);
        SetParent (body);
    }

    //--------------------------------------------------------------------------------------------------------------------------------
    //------------------------------------------------------------WhileAst------------------------------------------------------------
    //--------------------------------------------------------------------------------------------------------------------------------
    
    WhileAst::WhileAst (AstNode *whileCondition, AstNode *loopBody) :
        AstNode (AstTypeId::WHILE), condition (whileCondition), body (loopBody) {

        SetParent (condition);
        SetParent (body);
    }

    //--------------------------------------------------------------------------------------------------------------------------------
    //------------------------------------------------------------ReturnAst-----------------------------------------------------------
    //--------------------------------------------------------------------------------------------------------------------------------

    ReturnAst::ReturnAst (AstNode *statement) :
        AstNode (AstTypeId::RETURN), returnStatement (statement) {}

    //--------------------------------------------------------------------------------------------------------------------------------
    //------------------------------------------------------------AssignmentAst-------------------------------------------------------
    //--------------------------------------------------------------------------------------------------------------------------------

    AssignmentAst::AssignmentAst (VariableAst *variableNode, AstNode *expression) :
        AstNode (AstTypeId::ASSIGNMENT), variable (variableNode), assignmentExpression (expression) {

        SetParent (variableNode);
        SetParent (expression);
    }

    //--------------------------------------------------------------------------------------------------------------------------------
    //------------------------------------------------------------InAst---------------------------------------------------------------
    //--------------------------------------------------------------------------------------------------------------------------------

    InAst::InAst () : AstNode (AstTypeId::IN) {}

    //--------------------------------------------------------------------------------------------------------------------------------
    //------------------------------------------------------------OutAst--------------------------------------------------------------
    //--------------------------------------------------------------------------------------------------------------------------------

    OutAst::OutAst (AstNode *expression) : AstNode (AstTypeId::OUT), outExpression (expression) {}

}
