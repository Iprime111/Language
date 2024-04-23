#ifndef ASSEMBLY_GAYNERATOR_H_
#define ASSEMBLY_GAYNERATOR_H_

#include <stddef.h>
#include "BackendCore.h"

const size_t MAX_NUMBER_LENGTH = 32;
const int    INITIAL_ADDRESS   = 30000;

struct CreationData {
    size_t blockIndex  = 0;
    char  *blockName   = nullptr;
    char  *blockSource = nullptr;
};

#define Traversal(direction)         TreeTraversal (context, node->direction, outputBuffer, currentNameTableIndex)
#define BothWayTraversal()           Traversal (left);    Traversal (right)
#define UnaryOperation(operation)    Traversal (right);   WriteString ("\t" operation "\n");
#define BinaryOperation(operation)   BothWayTraversal (); WriteString ("\t" operation "\n");
#define MemoryCell(direction)        WriteIdentifierMemoryCell (context, node->direction, outputBuffer, currentNameTableIndex)
#define Label(name, index)           WriteLabel (context, outputBuffer, name, index)
#define NewLabel(name, index)        Label (name, index); WriteString (":\n")

#define LogicJump(operation)                                                            \
    context->operatorsCount.logicExpressionCount++;                                     \
    WriteString ("\t" operation " ");                                                   \
    Label       ("LOGIC_TRUE_BRANCH", context->operatorsCount.logicExpressionCount);    \
    WriteString ("\n\tpush 0\n\tjmp ");                                                 \
    Label       ("LOGIC_JUMP_END",    context->operatorsCount.logicExpressionCount);    \
    WriteString ("\n");                                                                 \
    NewLabel    ("LOGIC_TRUE_BRANCH", context->operatorsCount.logicExpressionCount);    \
    WriteString ("\tpush 1\n");                                                         \
    NewLabel    ("LOGIC_JUMP_END",    context->operatorsCount.logicExpressionCount)

#define JumpOperator(operation)      BothWayTraversal (); LogicJump (operation);
#define ToLogicExpression(direction) Traversal (direction); WriteString ("\tpush 0"); LogicJump ("jne")
#define LogicOperator(operation)     ToLogicExpression (left); ToLogicExpression (right); WriteString ("\t" operation "\n");

#define Comment(str) WriteComment (context, outputBuffer, str);
#define Source(index, name, source) WriteCreationSource (context, outputBuffer, CreationData {.blockIndex = index, .blockName = name, .blockSource = source})

TranslationError GenerateAssembly (TranslationContext *context, FILE *outputStream);

#endif
