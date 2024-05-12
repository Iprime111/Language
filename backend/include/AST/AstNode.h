#ifndef AST_NODE_H_
#define AST_NODE_H_

#include <cstdint>

#include "FunctionType.h"
#include "Value.h"

namespace Ast {
    struct TranslationContext;

    enum class AstId {
        CONSTANT            = 1,
        IDENTIFIER          = 2,
        KEYWORD             = 3,
        FUNCTION_DEFINITION = 4,
        PARAMETERS          = 5,
        VAR_DECLARATION     = 6,
        CALL                = 7,
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
        AND    = 10,
        OR     = 11,
        SIN    = 12,
        COS    = 13,
        FLOOR  = 14,
        SQRT   = 15,
        NOT    = 16,
    };
    
    class AstNode {
        public:
            virtual ~AstNode () = default;
    
            virtual IR::Value *Codegen                     (TranslationContext *context)                                         = 0;
            virtual void       ConstructFunctionParameters (TranslationContext *context, std::vector <const IR::Type *> *params) = 0; //TODO protected?
            virtual void       ConstructCallArguments      (TranslationContext *context, std::vector <IR::Value *>      *args)   = 0;

        protected:
            void ConstructCallArgumentsForChild (TranslationContext *context, std::vector <IR::Value *> *args, AstNode *child);
    };
    
    class ConstantAst final : public AstNode {
        public:
            ConstantAst (int64_t constantValue);
    
            IR::Value *Codegen                     (TranslationContext *context)                                         override;
            void       ConstructFunctionParameters (TranslationContext *context, std::vector <const IR::Type *> *params) override {};
            void       ConstructCallArguments      (TranslationContext *context, std::vector <IR::Value *>      *args)   override {};
    
        private:
            int64_t constantValue;
    };

    class VariableAst final : public AstNode {
        public:
            VariableAst (size_t identifierIndex);

            IR::Value *Codegen                     (TranslationContext *context)                                         override;
            void       ConstructFunctionParameters (TranslationContext *context, std::vector <const IR::Type *> *params) override {};
            void       ConstructCallArguments      (TranslationContext *context, std::vector <IR::Value *>      *args)   override {};

            size_t GetIdentifierIndex () const;

        private:
            size_t identifierIndex;
    };
    
    class IdentifierAst final : public AstNode {
        public:
            IdentifierAst (size_t identifierIndex);

            IR::Value *Codegen                     (TranslationContext *context)                                         override;
            void       ConstructFunctionParameters (TranslationContext *context, std::vector <const IR::Type *> *params) override {};
            void       ConstructCallArguments      (TranslationContext *context, std::vector <IR::Value *>      *args)   override {};

            size_t GetIdentifierIndex () const;

        private:
            size_t identifierIndex;
    };
    
    class OperatorAst final : public AstNode {
        public:
            OperatorAst (AstNode *left, AstNode *right, AstOperatorId operatorId);
    
            IR::Value *Codegen                     (TranslationContext *context)                                         override;
            void       ConstructFunctionParameters (TranslationContext *context, std::vector <const IR::Type *> *params) override {};
            void       ConstructCallArguments      (TranslationContext *context, std::vector <IR::Value *>      *args)   override;

        private:
            AstNode *left, *right;
            AstOperatorId operatorId;
    };

    class TypeAst final : public AstNode {
        public:
            TypeAst (const IR::Type *type);

            IR::Value *Codegen                     (TranslationContext *context)                                         override;
            void       ConstructFunctionParameters (TranslationContext *context, std::vector <const IR::Type *> *params) override;
            void       ConstructCallArguments      (TranslationContext *context, std::vector <IR::Value *>      *args)   override {};

            const IR::Type *GetType () const;

        private:
            const IR::Type *nodeType;
    };

    class FunctionParametersAst final : public AstNode {
        public:
            FunctionParametersAst (AstNode *functionContent);

            IR::Value *Codegen                     (TranslationContext *context)                                         override;
            void       ConstructFunctionParameters (TranslationContext *context, std::vector <const IR::Type *> *params) override;
            void       ConstructCallArguments      (TranslationContext *context, std::vector <IR::Value *>      *args)   override {};
            
            void AddParameter (TypeAst *newParameter);

        private:
            AstNode *parameters;
            AstNode *functionContent;
    };

    class FunctionDefinitionAst final : public AstNode {
        public:
            FunctionDefinitionAst (size_t identifierIndex, FunctionParametersAst *parameters, TypeAst *returnType);

            IR::Value *Codegen                     (TranslationContext *context)                                         override;
            void       ConstructFunctionParameters (TranslationContext *context, std::vector <const IR::Type *> *params) override {};
            void       ConstructCallArguments      (TranslationContext *context, std::vector <IR::Value *>      *args)   override {};
        
        private:
            FunctionParametersAst *parameters;
            TypeAst               *returnType;

            size_t identifierIndex;
    };

    class OperatorSeparatorAst final : public AstNode {
        public:
            OperatorSeparatorAst (AstNode *left, AstNode *right);

            IR::Value *Codegen                     (TranslationContext *context)                                         override;
            void       ConstructFunctionParameters (TranslationContext *context, std::vector <const IR::Type *> *params) override {};
            void       ConstructCallArguments      (TranslationContext *context, std::vector <IR::Value *>      *args)   override {};

        private:
            AstNode *left, *right;
    };

    class ParameterSeparatorAst final : public AstNode {
        public:
            ParameterSeparatorAst (AstNode *left, AstNode *right);

            IR::Value *Codegen                     (TranslationContext *context)                                         override;
            void       ConstructFunctionParameters (TranslationContext *context, std::vector <const IR::Type *> *params) override;
            void       ConstructCallArguments      (TranslationContext *context, std::vector <IR::Value *>      *args)   override;

        private:
            AstNode *left, *right;
    };

    class VariableDeclarationAst final : public AstNode {
        public:
            VariableDeclarationAst (VariableAst *assignmentExpression, TypeAst *type);

            IR::Value *Codegen                     (TranslationContext *context)                                         override;
            void       ConstructFunctionParameters (TranslationContext *context, std::vector <const IR::Type *> *params) override;
            void       ConstructCallArguments      (TranslationContext *context, std::vector <IR::Value *>      *args)   override {};

        private:
            VariableAst *assignment;
            TypeAst     *nodeType;
    };

    class CallAst final : public AstNode {
        public:
            CallAst (IdentifierAst *functionIdentifier, AstNode *arguments);

            IR::Value *Codegen                     (TranslationContext *context)                                         override;
            void       ConstructFunctionParameters (TranslationContext *context, std::vector <const IR::Type *> *params) override {};
            void       ConstructCallArguments      (TranslationContext *context, std::vector <IR::Value *>      *args)   override;

        private:
            AstNode       *arguments;
            IdentifierAst *functionIdentifier;
    };

    class IfAst final : public AstNode {
        public:
            IfAst (AstNode *condition, AstNode *body);

            IR::Value *Codegen                     (TranslationContext *context)                                         override;
            void       ConstructFunctionParameters (TranslationContext *context, std::vector <const IR::Type *> *params) override {};
            void       ConstructCallArguments      (TranslationContext *context, std::vector <IR::Value *>      *args)   override {};

        private:
            AstNode *condition;
            AstNode *body;
    };

    //TODO while
    
    class ReturnAst final : public AstNode {
        public:
            ReturnAst (AstNode *returnStatement);

            IR::Value *Codegen                     (TranslationContext *context)                                         override;
            void       ConstructFunctionParameters (TranslationContext *context, std::vector <const IR::Type *> *params) override {};
            void       ConstructCallArguments      (TranslationContext *context, std::vector <IR::Value *>      *args)   override {};

        private:
            AstNode *returnStatement;
    };

    class AssignmentAst final : public AstNode {
        public:
            AssignmentAst (VariableAst *variable, AstNode *assignmentExpression);

            IR::Value *Codegen                     (TranslationContext *context)                                         override;
            void       ConstructFunctionParameters (TranslationContext *context, std::vector <const IR::Type *> *params) override {};
            void       ConstructCallArguments      (TranslationContext *context, std::vector <IR::Value *>      *args)   override {};
        private:
            VariableAst *variable;
            AstNode     *assignmentExpression;
    };  
}

#endif
