#include "Use.h"

// TODO why do you what to have use without operands
Use::Use () : operand (nullptr) {}
Use::Use (Value *operand) : operand (operand) {}

Value *Use::GetOperand () const { return operand; }
