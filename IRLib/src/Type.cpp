#include <climits>

#include "Type.h"

namespace IR {
    //------------------------------------------------------------------------------------------------------------------------------
    //-------------------------------------------------Type-------------------------------------------------------------------------
    //------------------------------------------------------------------------------------------------------------------------------
    
    Type::Type (size_t bitsCount, TypeId typeId) : bitsCount (bitsCount), size ((bitsCount + CHAR_BIT - 1) / CHAR_BIT), typeId (typeId) {}
    
    size_t Type::GetSize   () const { return size; }
    TypeId Type::GetTypeId () const { return typeId; }
    
    //------------------------------------------------------------------------------------------------------------------------------
    //-------------------------------------------------Integer----------------------------------------------------------------------
    //------------------------------------------------------------------------------------------------------------------------------
    
    IntegerType::IntegerType (size_t size, bool isUnsigned) : Type (size, TypeId::INTEGER_TYPE), isUnsigned (isUnsigned) {}
    
    bool IntegerType::IsUnsigned () const { return isUnsigned; }
    
    const IntegerType *TypesImplementation::GetInt64Ty () const { return &int64Type; }
    const IntegerType *TypesImplementation::GetInt1Ty  () const { return &int1Type;  }
}
