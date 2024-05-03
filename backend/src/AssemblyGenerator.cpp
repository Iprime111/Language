#include <cstdint>
#include <cstdio>
#include <cassert>

#include "AssemblyGenerator.h"
#include "BackendCore.h"
#include "Buffer.h"
#include "Constant.h"
#include "FunctionType.h"
#include "IRBuilder.h"
#include "NameTable.h"
#include "SyntaxTree.h"
#include "TreeReader.h"

#define WriteString(data)                                                                       \
    do {                                                                                        \
        if (WriteStringToBuffer (outputBuffer, data) != BufferErrorCode::NO_BUFFER_ERRORS) {    \
            return TranslationError::OUTPUT_FILE_ERROR;                                         \
        }                                                                                       \
    } while (0)

static TranslationError TreeTraversal             (IRBuilder *builder, TranslationContext *context, Tree::Node <AstNode> *node, int currentNameTableIndex);
static Value *WriteConstant               (IRBuilder *builder, double constant);
static Value *WriteIdentifier             (IRBuilder *builder, TranslationContext *context, Tree::Node <AstNode> *node, int currentNameTableIndex);
static TranslationError NewFunction       (IRBuilder *builder, TranslationContext *context, Tree::Node <AstNode> *node, int currentNameTableIndex);
static TranslationError NewVariable       (IRBuilder *builder, TranslationContext *context, Tree::Node <AstNode> *node, int currentNameTableIndex);
static TranslationError WriteFunctionCall (IRBuilder *builder, TranslationContext *context, Tree::Node <AstNode> *node, int currentNameTableIndex);
static TranslationError WriteKeyword      (IRBuilder *builder, TranslationContext *context, Tree::Node <AstNode> *node, int currentNameTableIndex);

TranslationError GenerateAssembly (IRBuilder *builder, TranslationContext *context) {
    assert (builder);

    Buffer <char> outputBuffer = {};

    if (InitBuffer (&outputBuffer) != BufferErrorCode::NO_BUFFER_ERRORS)
        return TranslationError::OUTPUT_FILE_ERROR;

    TreeTraversal (builder, context, context->abstractSyntaxTree.root, 0);

    DestroyBuffer (&outputBuffer);

    return TranslationError::NO_ERRORS;
}

static TranslationError TreeTraversal (IRBuilder *builder, TranslationContext *context, Tree::Node <AstNode> *node, int currentNameTableIndex) {
    assert (builder);
    assert (context);

    if (!node)
        return TranslationError::NO_ERRORS;

    #define NextCall(enumMember, function)                              \
        case NodeType::enumMember: {                                    \
            function (builder, context, node, currentNameTableIndex);   \
            break;                                                      \
        }

    switch (node->nodeData.type) {

        case NodeType::TERMINATOR:
            return TranslationError::TREE_ERROR;

        case NodeType::CONSTANT:
            WriteConstant (node->nodeData.content.number);
            break;

        case NodeType::FUNCTION_ARGUMENTS:
            context->isParsingCallArguments = false;

            TreeTraversal (builder, context, node->left,  currentNameTableIndex);
            TreeTraversal (builder, context, node->right, currentNameTableIndex);
            break;

        NextCall (STRING,               WriteIdentifier);
        NextCall (KEYWORD,              WriteKeyword);
        NextCall (FUNCTION_DEFINITION,  NewFunction);
        NextCall (VARIABLE_DECLARATION, NewVariable);
        NextCall (FUNCTION_CALL,        WriteFunctionCall);
    }

    #undef NextCall

    return TranslationError::NO_ERRORS;
}

static Value *WriteConstant (IRBuilder *builder, double constant) {
    assert (builder);

    int64_t intConstant = (int64_t) constant;

    IRContext *irContext = builder->GetContext ();
    return ConstantData::GetConstant (irContext, ConstantData (Type::GetInt64Ty (irContext), &intConstant));
}

static TranslationError WriteIdentifier (IRBuilder *builder, Tree::Node <AstNode> *node, int currentNameTableIndex) {
    assert (builder);
    assert (node);

    //TODO

    return TranslationError::NO_ERRORS;
}

static TranslationError NewFunction (IRBuilder *builder, TranslationContext *context, Tree::Node <AstNode> *node, int currentNameTableIndex) {
    assert (builder);
    assert (node);

    int tableIndex = GetLocalNameTableIndex ((int) node->nodeData.content.nameTableIndex, &context->localTables);

    if (tableIndex < 0)
        return TranslationError::NAME_TABLE_ERROR;

    FunctionType type = {
        .returnValue = Type::GetInt64Ty (builder->GetContext ()),
        .params      = {},
    };

    InitBuffer (&type.params);

    context->isParsingCallArguments = true;
    if (node->left)
        TreeTraversal (builder, context, node->left, tableIndex);
    else
        return TranslationError::TREE_ERROR;

    context->isParsingCallArguments = false;

    Function *function = Function::Create   (&type, context->nameTable.data [node->nodeData.content.nameTableIndex].name, builder->GetContext ());
    BasicBlock *block  = BasicBlock::Create ("Function begin", function);

    builder->SetInsertPoint (block);

    if (node->right)
        TreeTraversal (builder, context, node->right, tableIndex);
    else
        return TranslationError::TREE_ERROR;

    return TranslationError::NO_ERRORS;
}

static TranslationError NewVariable (IRBuilder *builder, Tree::Node <AstNode> *node, int currentNameTableIndex) {
    assert (builder);
    assert (node);

    //TODO: variable size
    builder->context->localTables.data [currentNameTableIndex].tableSize += 1;

    if (node->right && node->right->nodeData.type == NodeType::KEYWORD)
        TreeTraversal (builder, node->right, currentNameTableIndex);

    return TranslationError::NO_ERRORS; 
}

static TranslationError WriteFunctionCall (IRBuilder *builder, Tree::Node <AstNode> *node, int currentNameTableIndex) {
    assert (builder);
    assert (node);

    size_t blockIndex = ++builder->context->operatorsCount.callCount;
    char  *blockName  = "CALL OPERATOR";

    if (!node->right)
        return TranslationError::TREE_ERROR;

    builder->context->areCallArguments = true;

    if (node->left) {
        TreeTraversal (context, node->left, outputBuffer, currentNameTableIndex);
    }

    char stackFrameSizeBuffer [MAX_NUMBER_LENGTH] = "";
    snprintf (stackFrameSizeBuffer, MAX_NUMBER_LENGTH, "%lu", context->localTables.data [currentNameTableIndex].tableSize);
    

    if (node->parent && !(node->parent->nodeData.type == NodeType::KEYWORD && node->parent->nodeData.content.keyword == Keyword::OPERATOR_SEPARATOR)) 
        WriteString ("\tpush rax\n");

    return TranslationError::NO_ERRORS;
}

static TranslationError WriteKeyword (IRBuilder *builder, Tree::Node <AstNode> *node, int currentNameTableIndex) {
    assert (builder);
    assert (node);

    #define AssemblyOperator(opcode, ...)   \
    case Keyword::opcode: {                 \
            __VA_ARGS__                     \
            break;                          \
        }

    switch (node->nodeData.content.keyword) {
        AssemblyOperator (IF, {
            size_t blockIndex = ++context->operatorsCount.ifCount;
            char  *blockName   = "IF STATEMENT";

            Source (blockIndex, "IF CONDITION", blockName);
            Traversal (left);
            WriteString ("\tpush 0\n\tje ");

            Label ("IF_END", blockIndex);
            WriteString ("\n");
            Source (blockIndex, "IF BODY", blockName);
            Traversal (right);
            NewLabel ("IF_END", blockIndex);
        })

        AssemblyOperator (WHILE, {
            size_t blockIndex = ++context->operatorsCount.whileCount;
            char  *blockName  = "WHILE CYCLE"; 

            Source (blockIndex, "WHILE CONDITION", blockName);
            NewLabel ("WHILE_BEGIN", blockIndex);
            Traversal (left);
            WriteString ("\tpush 0\n\tje ");
            Label ("WHILE_END", blockIndex);
            WriteString ("\n");

            Source (blockIndex, "WHILE BODY", blockName);
            Traversal (right);

            Source (blockIndex, "WHILE END JUMP", blockName);
            WriteString ("\tjmp ");
            Label ("WHILE_BEGIN", blockIndex);
            WriteString ("\n");
            NewLabel ("WHILE_END", blockIndex);
        })

        AssemblyOperator (ASSIGNMENT, {
            size_t blockIndex = ++context->operatorsCount.assignmentCount;
            char  *blockName  = "ASSIGNMENT OPERATOR";

            Source (blockIndex, "ASSIGNMENT EXPRESSION", blockName);
            Traversal (left);
            Source (blockIndex, "ASSIGNMENT CELL", blockName);
            WriteString ("\tpop ");
            MemoryCell (right);
        })

        AssemblyOperator (SIN,                UnaryOperation  ("sin"))
        AssemblyOperator (COS,                UnaryOperation  ("cos"))
        AssemblyOperator (FLOOR,              UnaryOperation  ("floor"))
        AssemblyOperator (SQRT,               UnaryOperation  ("sqrt"))
        AssemblyOperator (ADD,                CreateAdd (builder))
        AssemblyOperator (SUB,                CreateSub (builder))
        AssemblyOperator (MUL,                CreateMul (builder))
        AssemblyOperator (DIV,                CreateDiv (builder))
        AssemblyOperator (EQUAL,              JumpOperator    ("je"))
        AssemblyOperator (LESS,               JumpOperator    ("jb"))
        AssemblyOperator (GREATER,            JumpOperator    ("ja"))
        AssemblyOperator (LESS_EQUAL,         JumpOperator    ("jbe"))
        AssemblyOperator (GREATER_EQUAL,      JumpOperator    ("jae"))
        AssemblyOperator (NOT_EQUAL,          JumpOperator    ("jne"))
        AssemblyOperator (AND,                LogicOperator   ("mul"))
        AssemblyOperator (OR,                 LogicOperator   ("add"))
        AssemblyOperator (ABORT,              WriteString     ("\thlt\n");)
        AssemblyOperator (NOT,                ToLogicExpression (left); LogicJump ("je");)
        AssemblyOperator (RETURN_OPERATOR,    Traversal (right); WriteString ("\tpop rax\n\tret\n");)
        AssemblyOperator (BREAK_OPERATOR,     WriteString ("\tjmp "); Label ("WHILE_END", context->operatorsCount.whileCount); WriteString ("\n");) //FIXME: inner breaks
        AssemblyOperator (CONTINUE_OPERATOR,  WriteString ("\tjmp "); Label ("WHILE_END", context->operatorsCount.whileCount); WriteString ("\n");)
        AssemblyOperator (IN,                 WriteString ("\tin\n");)
        AssemblyOperator (OUT,                Traversal (right); WriteString ("\tout\n");)
        AssemblyOperator (OPERATOR_SEPARATOR, BothWayTraversal ();)
        AssemblyOperator (ARGUMENT_SEPARATOR, {
            if (context->areCallArguments) {
                BothWayTraversal ();
            } else {
                Traversal (right); 
                Traversal (left); 
                if (node->left) { 
                    WriteString ("\tpop "); 
                    MemoryCell  (left);
                }
            }
        })

        default: {
            return TranslationError::TREE_ERROR;
        }

    }

    #undef AssemblyOperator

    return TranslationError::NO_ERRORS;
}
