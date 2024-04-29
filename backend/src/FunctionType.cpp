#include "FunctionType.h"
#include "BackendCore.h"

//------------------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------Type-------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------

Type::Type (size_t size, TypeId typeId) : size (size), typeId (typeId) {}

size_t Type::GetSize   () { return size; }
TypeId Type::GetTypeId () { return typeId; }

IntegerType *Type::GetInt64Ty (TranslationContext *context) { return &context->types.int64Type; }

//------------------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------Integer----------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------

IntegerType::IntegerType (size_t size, bool isUnsigned) : Type (size, TypeId::INTEGER_TYPE), isUnsigned (isUnsigned) {}

bool IntegerType::IsUnsigned () { return isUnsigned; }

