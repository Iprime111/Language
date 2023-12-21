#ifndef ASSEMBLY_GAYNERATOR_H_
#define ASSEMBLY_GAYNERATOR_H_

#include <stddef.h>
#include "BackendCore.h"

const size_t MAX_NUMBER_LENGTH = 32;
const int    INITIAL_ADDRESS   = 30000;

#define Traversal(direction)         TreeTraversal (context, node->direction, outputBuffer, currentNameTableIndex)
#define BothWayTraversal()           Traversal (left);    Traversal (right)
#define UnaryOperation(operation)    Traversal (right);   WriteString ("\t" operation "\n");
#define BinaryOperation(operation)   BothWayTraversal (); WriteString ("\t" operation "\n");
#define MemoryCell(direction)        WriteIdentifierMemoryCell (context, node->direction, outputBuffer, currentNameTableIndex)
#define Label(index)                 WriteLabel (context, outputBuffer, index)
#define NewLabel(index)              Label (index); WriteString (":\n")

#define LogicJump(operation)            \
    WriteString ("\t" operation " ");   \
    Label (context->labelIndex);        \
    WriteString ("\n\tpush 0\n\tjmp "); \
    Label (context->labelIndex + 1);    \
    WriteString ("\n");                 \
    NewLabel (context->labelIndex++);   \
    WriteString ("\tpush 1\n");         \
    NewLabel (context->labelIndex++)

#define JumpOperator(operation)      BothWayTraversal (); LogicJump (operation);
#define ToLogicExpression(direction) Traversal (direction); WriteString ("\tpush 0"); LogicJump ("jne")
#define LogicOperator(operation)     ToLogicExpression (left); ToLogicExpression (right); WriteString ("\t" operation "\n");

TranslationError GenerateAssembly (TranslationContext *context, FILE *outputStream);

#endif
