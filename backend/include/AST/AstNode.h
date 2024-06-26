#ifndef AST_NODE_H_
#define AST_NODE_H_

#include <vector>

#include "BasicBlock.h"
#include "Type.h"
#include "Value.h"

namespace Ast {
    struct TranslationContext;
    class  AstNode;

    using OperatorFunction = IR::Value *(*) (TranslationContext *context, AstNode *node, IR::Value *lhs, IR::Value *rhs);

    enum class AstTypeId {
        CONSTANT             = 1,
        VARIABLE             = 3,
        IDENTIFIER           = 4,
        OPERATOR             = 5,
        TYPE                 = 6,
        FUNCTION_PARAMETERS  = 7,
        FUNCTION_DEFINITION  = 8,
        OPERATOR_SEPARATOR   = 9,
        PARAMETER_SEPARATOR  = 10,
        VARIABLE_DECLARATION = 11,
        CALL                 = 12,
        IF                   = 13,
        WHILE                = 14,
        RETURN               = 15,
        ASSIGNMENT           = 16,
        LOGIC_OPERATOR       = 17,
        IN                   = 18, 
        OUT                  = 19,
        BREAK                = 20,
        CONTINUE             = 21,
    };

    enum class AstOperatorId {
        ADD    = 0,
        SUB    = 1,
        MUL    = 2,
        DIV    = 3,
        CMP_L  = 4,
        CMP_G  = 5,
        CMP_E  = 6,
        CMP_LE = 7,
        CMP_GE = 8,
        CMP_NE = 9,
        SIN    = 10,
        COS    = 11,
        FLOOR  = 12,
        SQRT   = 13,
        AND    = 14,
        OR     = 15,
        NOT    = 16,
    };

    class AstNode {
        public:
            AstNode *parent;
            IR::BasicBlock *trueBranch, *falseBranch;
            IR::BasicBlock *breakBlock, *continueBlock;

            virtual ~AstNode () = default;

            AstTypeId GetAstTypeId () const;

            virtual IR::Value *Codegen (TranslationContext *context) = 0;

        protected:
            explicit AstNode (AstTypeId typeId);


        private:
            AstTypeId typeId;
    };
    
    class ConstantAst final : public AstNode {
        public:
            explicit ConstantAst (double constant);
    
            IR::Value *Codegen (TranslationContext *context) override;
    
        private:
            double constantValue;
    };

    class VariableAst final : public AstNode {
        public:
            explicit VariableAst (size_t identifier);

            IR::Value *Codegen (TranslationContext *context) override;

            size_t GetIdentifierIndex () const;

        private:
            size_t identifierIndex;
    };
    
    class IdentifierAst final : public AstNode {
        public:
            explicit IdentifierAst (size_t identifier);

            IR::Value *Codegen (TranslationContext *context) override { return nullptr; };

            size_t GetIdentifierIndex () const;

        private:
            size_t identifierIndex;
    };
    
    class OperatorAst : public AstNode {
        public:
            explicit OperatorAst (AstNode *leftNode, AstNode *rightNode, AstOperatorId astOperatorId);

            AstOperatorId GetOperatorId () const;
            AstNode      *GetLeft       () const;
            AstNode      *GetRight      () const;
    
            IR::Value *Codegen (TranslationContext *context) override;

        protected:
            explicit OperatorAst (AstNode *leftNode, AstNode *rightNode, AstOperatorId operatorId, AstTypeId type);

            static OperatorFunction GetOperatorFunction (TranslationContext *context, const IR::Type *type, AstOperatorId operatorId);

        private:
            AstNode *left, *right;
            AstOperatorId operatorId;

    };

    class LogicOperatorAst final : public OperatorAst {
        public:
            explicit LogicOperatorAst (AstNode *leftNode, AstNode *rightNode, AstOperatorId astOperatorId);

            IR::Value *Codegen (TranslationContext *context) override;
    };

    class TypeAst final : public AstNode {
        public:
            explicit TypeAst (const IR::Type *type);

            IR::Value *Codegen (TranslationContext *context) override { return nullptr; };
            void ConstructFunctionParameters (std::vector <const IR::Type *> *params);

            const IR::Type *GetType () const;

        private:
            const IR::Type *nodeType;
    };

    class FunctionParametersAst final : public AstNode {
        public:
            explicit FunctionParametersAst (AstNode *functionParameters, AstNode *functionBody);

            IR::Value *Codegen (TranslationContext *context) override;
            void ConstructFunctionParameters (std::vector <const IR::Type *> *params, std::vector <size_t> *argumentNames);
            
        private:
            AstNode *parameters;
            AstNode *functionContent;
    };

    class FunctionDefinitionAst final : public AstNode {
        public:
            explicit FunctionDefinitionAst (size_t identifier, FunctionParametersAst *parametersNode, TypeAst *returnValueType);

            IR::Value *Codegen (TranslationContext *context) override;
        
        private:
            FunctionParametersAst *parameters;
            TypeAst               *returnType;

            size_t identifierIndex;
    };

    class OperatorSeparatorAst final : public AstNode {
        public:
            explicit OperatorSeparatorAst (AstNode *leftNode, AstNode *rightNode);

            IR::Value *Codegen (TranslationContext *context) override;

        private:
            AstNode *left, *right;
    };

    class ParameterSeparatorAst final : public AstNode {
        public:
            explicit ParameterSeparatorAst (AstNode *leftNode, AstNode *rightNode);

            IR::Value *Codegen                     (TranslationContext *context) override;
            void       ConstructFunctionParameters (std::vector <const IR::Type *> *params, std::vector <size_t> *argumentNames);
            void       ConstructCallArguments      (TranslationContext *context, std::vector <IR::Value *> *args);

        private:
            AstNode *left, *right;

            void ConstructCallArgumentsForChild (TranslationContext *context, std::vector <IR::Value *> *args, AstNode *child);
    };

    class VariableDeclarationAst final : public AstNode {
        public:
            explicit VariableDeclarationAst (size_t identifier, AstNode *assignmentExpression, TypeAst *type);

            IR::Value *Codegen (TranslationContext *context) override;
            void       ConstructFunctionParameters (std::vector <const IR::Type *> *params, std::vector <size_t> *argumentNames);

        private:
            size_t identifierIndex;

            AstNode *assignment;
            TypeAst *nodeType;
    };

    class CallAst final : public AstNode {
        public:
            explicit CallAst (IdentifierAst *identifier, AstNode *functionArguments);

            IR::Value *Codegen (TranslationContext *context) override;

        private:
            AstNode       *arguments;
            IdentifierAst *functionIdentifier;

            void ConstructCallArguments (TranslationContext *context, std::vector <IR::Value *> *args);
    };

    class IfAst final : public AstNode {
        public:
            explicit IfAst (AstNode *ifCondition, AstNode *ifBody);

            IR::Value *Codegen (TranslationContext *context) override;

        private:
            AstNode *condition;
            AstNode *body;
    };

    class WhileAst final : public AstNode {
        public:
            explicit WhileAst (AstNode *whileCondition, AstNode *whileBody);
            
            IR::Value *Codegen (TranslationContext *context) override;

        private:
            AstNode *condition;
            AstNode *body;
    };

    class ReturnAst final : public AstNode {
        public:
            explicit ReturnAst (AstNode *statement);

            IR::Value *Codegen (TranslationContext *context) override;

        private:
            AstNode *returnStatement;
    };

    class AssignmentAst final : public AstNode {
        public:
            explicit AssignmentAst (VariableAst *variableNode, AstNode *expression);

            IR::Value *Codegen (TranslationContext *context) override;
        private:
            VariableAst *variable;
            AstNode     *assignmentExpression;
    };

    class InAst final : public AstNode {
        public:
            explicit InAst ();

            IR::Value *Codegen (TranslationContext *context) override;
    };

    class OutAst final : public AstNode {
        public:
            explicit OutAst (AstNode *expression);
        
            IR::Value *Codegen (TranslationContext *context) override;

        private:
            AstNode *outExpression;
    };

    class BreakAst final : public AstNode {
        public:
            explicit BreakAst ();

            IR::Value *Codegen (TranslationContext *context) override;
    };

    class ContinueAst final : public AstNode {
        public:
            explicit ContinueAst ();

            IR::Value *Codegen (TranslationContext *context) override;
    };
}

#endif
