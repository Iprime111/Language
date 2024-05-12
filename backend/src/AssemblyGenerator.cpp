#include <cstdint>
#include <cstdio>
#include <cassert>

#include "AssemblyGenerator.h"
#include "BackendCore.h"
#include "Buffer.h"
#include "Constant.h"
#include "FunctionType.h"
#include "IRBuilder.h"
#include "Instruction.h"
#include "NameTable.h"
#include "SyntaxTree.h"
#include "TreeDefinitions.h"
#include "TreeReader.h"

#define WriteString(data)                                                                       \
    do {                                                                                        \
        if (WriteStringToBuffer (outputBuffer, data) != BufferErrorCode::NO_BUFFER_ERRORS) {    \
            return TranslationError::OUTPUT_FILE_ERROR;                                         \
        }                                                                                       \
    } while (0)

static Value *TreeTraversal     (IRBuilder *builder, TranslationContext *context, Tree::Node <AstNode> *node, int currentNameTableIndex);
static Value *WriteConstant     (IRBuilder *builder, double constant);
static Value *NewFunction       (IRBuilder *builder, TranslationContext *context, Tree::Node <AstNode> *node, int currentNameTableIndex);
static Value *NewVariable       (IRBuilder *builder, TranslationContext *context, Tree::Node <AstNode> *node, int currentNameTableIndex);
static Value *WriteFunctionCall (IRBuilder *builder, TranslationContext *context, Tree::Node <AstNode> *node, int currentNameTableIndex);
static Value *WriteKeyword      (IRBuilder *builder, TranslationContext *context, Tree::Node <AstNode> *node, int currentNameTableIndex);

static TranslationError ConstructFunctionArguments (FunctionType *functionType, IRBuilder *builder, Tree::Node <AstNode> *node);
static Value *NodeToComparison (IRBuilder *builder, TranslationContext *context, Tree::Node <AstNode> *node, int currentNameTableIndex);
static bool   IsComparisonNode (Tree::Node <AstNode> *node);


TranslationError GenerateAssembly (IRBuilder *builder, TranslationContext *context) {
    assert (builder);

    Buffer <char> outputBuffer = {};

    if (InitBuffer (&outputBuffer) != BufferErrorCode::NO_BUFFER_ERRORS)
        return TranslationError::OUTPUT_FILE_ERROR;

    TreeTraversal (builder, context, context->abstractSyntaxTree.root, 0);

    DestroyBuffer (&outputBuffer);

    return TranslationError::NO_ERRORS;
}

static Value *TreeTraversal (IRBuilder *builder, TranslationContext *context, Tree::Node <AstNode> *node, int currentNameTableIndex) {
    assert (builder);
    assert (context);

    if (!node)
        return nullptr;

    #define NextCall(enumMember, function)                                      \
        case NodeType::enumMember: {                                            \
            return function (builder, context, node, currentNameTableIndex);    \
        }

    switch (node->nodeData.type) {

        case NodeType::TERMINATOR:
            return nullptr;

        case NodeType::CONSTANT:
            return WriteConstant (builder, node->nodeData.content.number);
            break;

        case NodeType::FUNCTION_ARGUMENTS:
            context->isParsingCallArguments = false;

            //TODO parse args
            TreeTraversal (builder, context, node->left,  currentNameTableIndex);
            TreeTraversal (builder, context, node->right, currentNameTableIndex);
            break;

        case NodeType::STRING: {
            AllocaInstruction *localVariable = context->localVariables [node->nodeData.content.nameTableIndex]; 

            if (localVariable)
                return Load (localVariable);
            else
                return Load (context->globalVariables [node->nodeData.content.nameTableIndex]);
        }

        NextCall (KEYWORD,              WriteKeyword);
        NextCall (FUNCTION_DEFINITION,  NewFunction);
        NextCall (VARIABLE_DECLARATION, NewVariable);
        NextCall (FUNCTION_CALL,        WriteFunctionCall);
    }

    #undef NextCall

    return nullptr;
}

static Value *WriteConstant (IRBuilder *builder, double constant) {
    assert (builder);

    int64_t intConstant = (int64_t) constant;

    return ConstantData::CreateConstant (builder->GetContext (), builder->GetInt64Ty (), &intConstant);
}

static Value *NewFunction (IRBuilder *builder, TranslationContext *context, Tree::Node <AstNode> *node, int currentNameTableIndex) {
    assert (builder);
    assert (node);

    int tableIndex = GetLocalNameTableIndex ((int) node->nodeData.content.nameTableIndex, &context->localTables);

    if (tableIndex < 0)
        return nullptr;

    FunctionType type = {
        .returnValue = builder->GetInt64Ty (),
        .params      = {},
    };

    //TODO: Ast type -> IR type function
    if (node->right && node->right->left) // node->right is "Parameters" node and node->right->left is first argument
       ConstructFunctionArguments (&type, builder, node->right->left);

    Function *function = Function::Create   (builder->GetContext (), &type, context->nameTable.data [node->nodeData.content.nameTableIndex].name);
    BasicBlock *block  = BasicBlock::Create ("Function begin", function, builder->GetContext ());

    builder->SetInsertPoint (block);

    if (node->right)
        TreeTraversal (builder, context, node->right, tableIndex);
    else
        return nullptr;

    context->localVariables.clear ();

    return function;
}

static TranslationError ConstructFunctionArguments (FunctionType *functionType, IRBuilder *builder, Tree::Node <AstNode> *node) {
    assert (functionType);
    assert (builder);

    if (!node)
        return TranslationError::NO_ERRORS;
 
    if (node->nodeData.type == NodeType::KEYWORD && node->nodeData.content.keyword == Keyword::ARGUMENT_SEPARATOR) {
        ConstructFunctionArguments (functionType, builder, node->left);
        ConstructFunctionArguments (functionType, builder, node->right);
    
        return TranslationError::NO_ERRORS;
    }

    if (node->nodeData.type == NodeType::VARIABLE_DECLARATION) {
            functionType->params.push_back (builder->GetInt64Ty ());

            return TranslationError::NO_ERRORS;
    }

    return TranslationError::TREE_ERROR;
}

static Value *NewVariable (IRBuilder *builder, TranslationContext *context, Tree::Node <AstNode> *node, int currentNameTableIndex) {
    assert (builder);
    assert (node);

    //TODO: node -> IR type
    AllocaInstruction *newVariable = (AllocaInstruction *) 
        builder->CreateAllocaInstruction (builder->GetInt64Ty ());

    context->localVariables [node->nodeData.content.nameTableIndex] = newVariable;

    if (!node->right)
        return nullptr;

    if (node->right->nodeData.type == NodeType::KEYWORD)
        TreeTraversal (builder, context, node->right, currentNameTableIndex);

    return newVariable; 
}

static Value *WriteFunctionCall (IRBuilder *builder, TranslationContext *context, Tree::Node <AstNode> *node, int currentNameTableIndex) {
    return nullptr;
}

static bool IsComparisonNode (Tree::Node <AstNode> *node) {
    if (node->nodeData.type != NodeType::KEYWORD)
        return false;

    Keyword keyword = node->nodeData.content.keyword;

    if (keyword == Keyword::LESS       || keyword == Keyword::GREATER       || keyword == Keyword::EQUAL || 
        keyword == Keyword::LESS_EQUAL || keyword == Keyword::GREATER_EQUAL || keyword == Keyword::NOT_EQUAL)
            return true;

    return false;
}

static Value *NodeToComparison (IRBuilder *builder, TranslationContext *context, Tree::Node <AstNode> *node, int currentNameTableIndex) {
    assert (node);
    assert (builder);
    assert (context);

    if (!IsComparisonNode (node)) {
        return builder->CreateCmpOperator (CmpOperatorId::CMP_NE, TreeTraversal (builder, context, node, currentNameTableIndex), 
                                           ConstantData::CreateZeroConstant (builder->GetContext (), builder->GetInt64Ty ()));
    }
    
    return TreeTraversal (builder, context, node, currentNameTableIndex);
}

static Value *WriteKeyword (IRBuilder *builder, TranslationContext *context, Tree::Node <AstNode> *node, int currentNameTableIndex) {
    assert (builder);
    assert (context);
    assert (node);

    #define AssemblyOperator(opcode, ...)   \
    case Keyword::opcode: {                 \
            return __VA_ARGS__              \
        }


    //TODO switch cases in separate functions
    switch (node->nodeData.content.keyword) {
        //TODO while and or not

        case Keyword::AND: {
            Value *leftComparison = NodeToComparison (builder, context, node, currentNameTableIndex);

        }

        case Keyword::IF: {
            Function *currentFunction = (Function *) builder->GetInsertBlock ()->parent;

            Value *condition = Traversal (left);
            IRContext *irContext = builder->GetContext ();

            BasicBlock *thenBlock  = BasicBlock::Create ("Then block (\"if\")", currentFunction, irContext);
            BasicBlock *mergeBlock = BasicBlock::Create ("Merge after \"if\"",  currentFunction, irContext);

            //TODO else branch
            builder->CreateBranchInstruction (condition, thenBlock, mergeBlock);

            //TODO special function for 'or', 'not' and 'and' 
            builder->SetInsertPoint (thenBlock);
            Traversal (right);

            builder->SetInsertPoint (mergeBlock);
            break;
        }

        case Keyword::ASSIGNMENT: {
            AllocaInstruction *localVariable = context->localVariables [node->right->nodeData.content.nameTableIndex];

            if (localVariable)
                Store (localVariable, Traversal (left));
            else
                Store (context->globalVariables [node->right->nodeData.content.nameTableIndex], Traversal (left));

            break;
        }

        AssemblyOperator (SIN,                UnaryOperation  (SIN))
        AssemblyOperator (COS,                UnaryOperation  (COS))
        AssemblyOperator (FLOOR,              UnaryOperation  (FLOOR))
        AssemblyOperator (SQRT,               UnaryOperation  (SQRT))
        AssemblyOperator (ADD,                BinaryOperation (ADD))
        AssemblyOperator (SUB,                BinaryOperation (SUB))
        AssemblyOperator (MUL,                BinaryOperation (MUL))
        AssemblyOperator (DIV,                BinaryOperation (DIV))
        AssemblyOperator (EQUAL,              CmpOperation    (CMP_E))
        AssemblyOperator (LESS,               CmpOperation    (CMP_L))
        AssemblyOperator (GREATER,            CmpOperation    (CMP_G))
        AssemblyOperator (LESS_EQUAL,         CmpOperation    (CMP_LE))
        AssemblyOperator (GREATER_EQUAL,      CmpOperation    (CMP_GE))
        AssemblyOperator (NOT_EQUAL,          CmpOperation    (CMP_NE))
        AssemblyOperator (ABORT,              StateChange     (HLT))
        AssemblyOperator (RETURN_OPERATOR,    ReturnOperation ())
        //AssemblyOperator (BREAK_OPERATOR,     )
        //AssemblyOperator (CONTINUE_OPERATOR,  )
        //AssemblyOperator (IN,                 )
        //AssemblyOperator (OUT,                )
        case Keyword::OPERATOR_SEPARATOR: {
            Traversal (left);
            Traversal (right);

            break;
        }

        case Keyword::ARGUMENT_SEPARATOR: {
            if (context->isParsingCallArguments) {
                //TODO load args into function call (after making CallInstruction)
                Traversal (left); Traversal (right);
            } else {
                Traversal (right); Traversal (left); 
                
                if (node->left)
                    Load (context->localVariables [node->left->nodeData.content.nameTableIndex]);
            }

            break;
        }

    }

    #undef AssemblyOperator

    return nullptr;
}
