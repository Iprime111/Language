#ifndef FUNCTION_TYPE_H_
#define FUNCTION_TYPE_H_

#include "Buffer.h"

enum class TypeName {
    Int32   = 0,
    Int64   = 1,
    Pointer = 2,
};

struct Type {
    TypeName name = TypeName::Int32;
    size_t   size = 0;
};

struct FunctionType {
    Type           *returnValue = {};
    Buffer <Type *> params      = {}; 
};

#endif
