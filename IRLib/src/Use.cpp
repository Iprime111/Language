#include "Use.h"

Use::Use (Value *operand) : operand (operand) {}

Value *Use::GetOperand () { return operand; }
