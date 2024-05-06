#include "FunctionType.h"
#include "IRContext.h"

//------------------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------Type-------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------

Type::Type (size_t size, TypeId typeId) : size (size), typeId (typeId) {}

size_t Type::GetSize   () const { return size; }
TypeId Type::GetTypeId () const { return typeId; }

const IntegerType *Type::GetInt64Ty (IRContext *context) { return &context->types.int64Type; }

//------------------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------Integer----------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------

IntegerType::IntegerType (size_t size, bool isUnsigned) : Type (size, TypeId::INTEGER_TYPE), isUnsigned (isUnsigned) {}

bool IntegerType::IsUnsigned () const { return isUnsigned; }


