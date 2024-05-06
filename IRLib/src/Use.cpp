#include "Use.h"

Use::Use () : operand (nullptr) {}
Use::Use (Value *operand) : operand (operand) {}

Value *Use::GetOperand () const { return operand; }
