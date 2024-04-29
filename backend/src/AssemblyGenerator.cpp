#include <cstdio>
#include <cassert>

#include "AssemblyGenerator.h"
#include "BackendCore.h"
#include "Buffer.h"
#include "FunctionType.h"
#include "Ir.h"
#include "IrBuilder.h"
#include "NameTable.h"
#include "SyntaxTree.h"
#include "TreeReader.h"

#define WriteString(data)                                                                       \
    do {                                                                                        \
        if (WriteStringToBuffer (outputBuffer, data) != BufferErrorCode::NO_BUFFER_ERRORS) {    \
            return TranslationError::OUTPUT_FILE_ERROR;                                         \
        }                                                                                       \
    } while (0)

static TranslationError SetupProgram              (IRBuilder *builder);
static TranslationError TreeTraversal             (IRBuilder *builder, Tree::Node <AstNode> *node, int currentNameTableIndex);
static TranslationError WriteConstant             (double constant);
static TranslationError WriteIdentifier           (IRBuilder *builder, Tree::Node <AstNode> *node, int currentNameTableIndex);
static TranslationError NewFunction               (IRBuilder *builder, Tree::Node <AstNode> *node, int currentNameTableIndex);
static TranslationError NewVariable               (IRBuilder *builder, Tree::Node <AstNode> *node, int currentNameTableIndex);
static TranslationError WriteFunctionCall         (IRBuilder *builder, Tree::Node <AstNode> *node, int currentNameTableIndex);
static TranslationError WriteKeyword              (IRBuilder *builder, Tree::Node <AstNode> *node, int currentNameTableIndex);
static TranslationError WriteIdentifierMemoryCell (IRBuilder *builder, Tree::Node <AstNode> *node, int currentNameTableIndex);

static TranslationError WriteCreationSource       (IRBuilder *builder, Buffer <char> *outputBuffer, CreationData creationData);

static TranslationError WriteLabel                (IRBuilder *builder, Buffer <char> *outputBuffer, char *labelName, size_t labelIndex);

TranslationError GenerateAssembly (IRBuilder *builder) {
    assert (builder);

    Buffer <char> outputBuffer = {};

    if (InitBuffer (&outputBuffer) != BufferErrorCode::NO_BUFFER_ERRORS)
        return TranslationError::OUTPUT_FILE_ERROR;

    SetupProgram  (builder);
    TreeTraversal (builder, builder->context->abstractSyntaxTree.root, 0);

    DestroyBuffer (&outputBuffer);

    return TranslationError::NO_ERRORS;
}

static TranslationError TreeTraversal (IRBuilder *builder, Tree::Node <AstNode> *node, int currentNameTableIndex) {
    assert (builder);
    assert (builder->context);

    if (!node)
        return TranslationError::NO_ERRORS;

    #define NextCall(enumMember, function)                      \
        case NodeType::enumMember: {                            \
            function (builder, node, currentNameTableIndex);    \
            break;                                                          \
        }

    switch (node->nodeData.type) {

        case NodeType::TERMINATOR:
            return TranslationError::TREE_ERROR;

        case NodeType::CONSTANT:
            WriteConstant (node->nodeData.content.number);
            break;

        case NodeType::FUNCTION_ARGUMENTS:
            builder->context->areCallArguments = false;

            TreeTraversal (builder, node->left,  currentNameTableIndex);
            TreeTraversal (builder, node->right, currentNameTableIndex);
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

static TranslationError WriteConstant (IRBuilder *builder, double constant) {
    assert (builder);

    //TODO    

    return TranslationError::NO_ERRORS;
}

static TranslationError WriteIdentifier (IRBuilder *builder, Tree::Node <AstNode> *node, int currentNameTableIndex) {
    assert (builder);
    assert (node);

    //TODO

    return TranslationError::NO_ERRORS;
}

static TranslationError NewFunction (IRBuilder *builder, Tree::Node <AstNode> *node, int currentNameTableIndex) {
    assert (builder);
    assert (node);

    int tableIndex = GetLocalNameTableIndex ((int) node->nodeData.content.nameTableIndex, &builder->context->localTables);

    FunctionType type = {
        .returnValue = INT64,
        .params      = {},
    };

    InitBuffer (&type.params);

    Function *function = Function::Create   (&type, builder->context->nameTable.data [node->nodeData.content.nameTableIndex].name, builder->context);
    BasicBlock *block  = BasicBlock::Create ("Function begin", function);

    if (tableIndex < 0)
        return TranslationError::NAME_TABLE_ERROR;

    if (node->right)
        TreeTraversal (builder, node->right, tableIndex);
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
