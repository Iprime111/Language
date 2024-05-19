#ifndef TREE_READER_H_
#define TREE_READER_H_

#include <unordered_map>

#include "AST/AstNode.h"
#include "AST/TranslationContext.h"

namespace Ast {
    enum class ReaderNodeId {
        CONSTANT             = 1,
        IDENTIFIER           = 2,
        KEYWORD              = 3,
        FUNCTION_DEFINITION  = 4,
        PARAMETERS           = 5,
        VARIABLE_DECLARATION = 6,
        CALL                 = 7,
    };

    enum class ReaderKeyword {
        IF                  = 11,
        WHILE               = 12,
        ASSIGNMENT          = 13,
        SIN                 = 21,
        COS                 = 22,
        FLOOR               = 23,
        ADD                 = 24,
        SUB                 = 25,
        MUL                 = 26,
        DIV                 = 27,
        SQRT                = 29,
        CMP_E               = 31,
        CMP_L               = 32,
        CMP_G               = 33,
        CMP_LE              = 34,
        CMP_GE              = 35,
        CMP_NE              = 36,
        AND                 = 37,
        OR                  = 38,
        NOT                 = 39,
        OPERATOR_SEPARATOR  = 41,
        PARAMETER_SEPARATOR = 42,
        NUMBER_TYPE         = 51,
        IN                  = 61,
        OUT                 = 62,
        RETURN              = 71,
        BREAK               = 72,
        CONTINUE            = 73,
        ABORT               = 74,
    };

    class TreeReader;
    using ReaderFunction = AstNode *(TreeReader::*) (char **fileContent);

    class TreeReader final {
        public:
            TreeReader (TranslationContext *translationContext);

            void ReadTree       (char *fileContent);
            void ReadNameTables (char *fileContent);

        private:
            TranslationContext *context;
            std::unordered_map <ReaderNodeId, ReaderFunction> readerFunctions;

            AstNode       *ReadTreeInternal      (char **fileContent);
            IdentifierAst *ReadIdentifierSubtree (char **treeBegin);
    
            void ReadGlobalTable (char **fileContent);

            AstNode *ReadConstant            (char **fileContent);
            AstNode *ReadIdentifier          (char **fileContent);
            AstNode *ReadKeyword             (char **fileContent);
            AstNode *ReadFunctionParameters  (char **fileContent);
            AstNode *ReadFunctionDefinition  (char **fileContent);
            AstNode *ReadVariableDeclaration (char **fileContent);
            AstNode *ReadCall                (char **fileContent);
    };
}

#endif
