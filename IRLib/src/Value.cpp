#include "Value.h"

Value::Value (ValueType valueType) : valueType (valueType) {}

ValueType Value::GetType () { return valueType; }
