#include "Value.h"

namespace IR {
    Value::Value (ValueId valueId, const Type *valueType) : valueId (valueId), valueType (valueType) {}
    
        ValueId Value::GetValueId () const { return valueId; }
    const Type *Value::GetType    () const { return valueType; }
}
