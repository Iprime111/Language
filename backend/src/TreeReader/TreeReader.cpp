#include <cassert>
#include <cmath>

#include "TreeReader/TreeReader.h"
#include "AST/AstNode.h"
#include "AST/TranslationContext.h"

#define SkipSpaces()                            \
    do {                                        \
        while (isspace ((*treeBegin) [0])) {    \
            (*treeBegin)++;                     \
        }                                       \
    } while (0)

namespace Ast {
    #define StoreCallback(nodeId, callback) readerFunctions [ReaderNodeId::nodeId] = &TreeReader::callback

    TreeReader::TreeReader (TranslationContext *translationContext) : context (translationContext) {
        StoreCallback (CONSTANT,             ReadConstant);
        StoreCallback (IDENTIFIER,           ReadIdentifier);
        StoreCallback (KEYWORD,              ReadKeyword);
        StoreCallback (PARAMETERS,           ReadFunctionParameters);
        StoreCallback (FUNCTION_DEFINITION,  ReadFunctionDefinition);
        StoreCallback (VARIABLE_DECLARATION, ReadVariableDeclaration);
        StoreCallback (CALL,                 ReadCall);
    }

    #undef StoreCallback

    void TreeReader::ReadTree (char *fileContent) {
        assert (fileContent);

        int entryPointLength = 0;
        sscanf (fileContent, "%lu%n", &context->entryPoint, &entryPointLength);

        fileContent += entryPointLength;

        context->root = ReadTreeInternal (&fileContent);
    }

    void TreeReader::ReadNameTables (char *fileContent) {
        assert (fileContent);
    
        ReadGlobalTable (&fileContent);
    
        size_t tablesCount       = 0;
        int    countNumberLength = 0;
    
        sscanf (fileContent, "%lu%n", &tablesCount, &countNumberLength);
    
        fileContent += countNumberLength;
    }

    void TreeReader::ReadGlobalTable (char **fileContent) {
        assert (fileContent);
    
        size_t tableSize        = 0;
        int    sizeNumberLength = 0;
    
        sscanf (*fileContent, "%lu%n", &tableSize, &sizeNumberLength);

        (*fileContent) += sizeNumberLength;
    
        for (size_t globalTableRecord = 0; globalTableRecord < tableSize; globalTableRecord++) {
            
            char *newIdentifier  = nullptr;
            int   identifierSize = 0;
            sscanf (*fileContent, "%ms%n[^\n]", &newIdentifier, &identifierSize);
            
            (*fileContent) += identifierSize;
            
            context->nameTable.emplace_back (newIdentifier);
            free (newIdentifier);
        }
    }
    
    AstNode *TreeReader::ReadTreeInternal (char **treeBegin) {
        assert (treeBegin);
    
        SkipSpaces ();
    
        if ((*treeBegin) [0] != '(') {
            (*treeBegin)++;
            return nullptr;
        }
    
        (*treeBegin)++;
        SkipSpaces ();
    
        int intNodeType = -1;
        sscanf (*treeBegin, "%d", &intNodeType);
    
        (*treeBegin)++;
        SkipSpaces ();

        ReaderFunction reader = readerFunctions [(ReaderNodeId) intNodeType];

        if (!reader)
            return nullptr;

        AstNode *newNode = (this->*reader) (treeBegin);

        context->treeNodes.push_back (newNode);

        SkipSpaces ();
        (*treeBegin)++;
    
        return newNode;
    }

    AstNode *TreeReader::ReadConstant (char **fileContent) {
        assert (fileContent);

        double constant = NAN;
        int    length   = 0;
        sscanf (*fileContent, "%lf%n", &constant, &length);

        (*fileContent) += length;

        ReadTreeInternal (fileContent);
        ReadTreeInternal (fileContent); //skipping blank subtrees

        return new ConstantAst (constant);
    }

    AstNode *TreeReader::ReadIdentifier (char **fileContent) {
        assert (fileContent);

        int    identifierLength = 0; 
        size_t identifierIndex  = 0;
        sscanf (*fileContent, "%lu%n", &identifierIndex, &identifierLength);

        (*fileContent) += identifierLength;

        ReadTreeInternal (fileContent);
        ReadTreeInternal (fileContent); //skipping blank subtrees

        return new VariableAst (identifierIndex);
    }

    AstNode *TreeReader::ReadKeyword (char **fileContent) {
        assert (fileContent);

        int keywordIndex = 0;
        int length   = 0;
        sscanf (*fileContent, "%d%n", &keywordIndex, &length);
    
        (*fileContent) += length;

        AstNode *leftSubtree  = ReadTreeInternal (fileContent);
        AstNode *rightSubtree = ReadTreeInternal (fileContent);

        #define OperatorNode(id)                                                        \
            case ReaderKeyword::id: {                                                   \
                return new OperatorAst (leftSubtree, rightSubtree, AstOperatorId::id);  \
            }

        #define LogicOperatorNode(id)                                                       \
            case ReaderKeyword::id: {                                                       \
                return new LogicOperatorAst (leftSubtree, rightSubtree, AstOperatorId::id); \
            }

        switch (static_cast <ReaderKeyword> (keywordIndex)) {
            case ReaderKeyword::IF:
                return new IfAst (leftSubtree, rightSubtree);

            case ReaderKeyword::WHILE:
                return new WhileAst (leftSubtree, rightSubtree);

            case ReaderKeyword::ASSIGNMENT:
                return new AssignmentAst (static_cast <VariableAst *> (rightSubtree), leftSubtree);

            OperatorNode (SIN)
            OperatorNode (COS)
            OperatorNode (FLOOR)
            OperatorNode (ADD)
            OperatorNode (SUB)
            OperatorNode (MUL)
            OperatorNode (DIV)
            OperatorNode (SQRT)
            OperatorNode (CMP_E)
            OperatorNode (CMP_L)
            OperatorNode (CMP_G)
            OperatorNode (CMP_LE)
            OperatorNode (CMP_GE)
            OperatorNode (CMP_NE)
            LogicOperatorNode (AND)
            LogicOperatorNode (OR)
            LogicOperatorNode (NOT)

            case ReaderKeyword::OPERATOR_SEPARATOR:
                return new OperatorSeparatorAst (leftSubtree, rightSubtree);

            case ReaderKeyword::PARAMETER_SEPARATOR:
                return new ParameterSeparatorAst (leftSubtree, rightSubtree);

            case ReaderKeyword::NUMBER_TYPE:
                return new TypeAst (context->builder.GetDoubleTy ());

            case ReaderKeyword::IN:
                return new InAst ();

            case ReaderKeyword::OUT:
                return new OutAst (rightSubtree);

            case ReaderKeyword::RETURN:
                return new ReturnAst (rightSubtree);

            case ReaderKeyword::BREAK:
            case ReaderKeyword::CONTINUE:
            case ReaderKeyword::ABORT:
              break;
        }

        #undef OperatorNode
        #undef LogicOperatorNode

        return nullptr;
    }

    AstNode *TreeReader::ReadFunctionParameters (char **fileContent) {
        assert (fileContent);

        AstNode *parameters   = ReadTreeInternal (fileContent);
        AstNode *functionBody = ReadTreeInternal (fileContent);

        return new FunctionParametersAst (parameters, functionBody);
    }

    AstNode *TreeReader::ReadFunctionDefinition (char **fileContent) {
        assert (fileContent);

        int    identifierLength = 0; 
        size_t identifierIndex  = 0;
        sscanf (*fileContent, "%lu%n", &identifierIndex, &identifierLength);

        (*fileContent) += identifierLength;

        TypeAst               *returnType = static_cast <TypeAst *>              (ReadTreeInternal (fileContent));
        FunctionParametersAst *parameters = static_cast <FunctionParametersAst *>(ReadTreeInternal (fileContent));

        return new FunctionDefinitionAst (identifierIndex, parameters, returnType);
    }

    AstNode *TreeReader::ReadVariableDeclaration (char **fileContent) {
        assert (fileContent);

        int    identifierLength = 0; 
        size_t identifierIndex  = 0;
        sscanf (*fileContent, "%lu%n", &identifierIndex, &identifierLength);

        (*fileContent) += identifierLength;

        TypeAst *varType    = static_cast <TypeAst *>(ReadTreeInternal (fileContent));
        AstNode *assignment =                         ReadTreeInternal (fileContent);

        return new VariableDeclarationAst (identifierIndex, assignment, varType);
    }

    AstNode *TreeReader::ReadCall (char **fileContent) {
        assert (fileContent);

        AstNode *arguments        = ReadTreeInternal (fileContent);
        IdentifierAst *identifier = ReadIdentifierSubtree (fileContent);

        return new CallAst (identifier, arguments);
    }

    IdentifierAst *TreeReader::ReadIdentifierSubtree (char **treeBegin) {
        assert (treeBegin);

         SkipSpaces ();
    
        if ((*treeBegin) [0] != '(') {
            (*treeBegin)++;
            return nullptr;
        }
    
        (*treeBegin)++;
        SkipSpaces ();
    
        int intNodeType = -1;
        sscanf (*treeBegin, "%d", &intNodeType);
    
        (*treeBegin)++;
        SkipSpaces ();

        int    identifierLength = 0; 
        size_t identifierIndex  = 0;
        sscanf (*treeBegin, "%lu%n", &identifierIndex, &identifierLength);

        (*treeBegin) += identifierLength;

        IdentifierAst *newNode = new IdentifierAst (identifierIndex);

        context->treeNodes.push_back (newNode);

        ReadTreeInternal (treeBegin);
        ReadTreeInternal (treeBegin);

        SkipSpaces ();
        (*treeBegin)++;
    
        return newNode;
    }
}
