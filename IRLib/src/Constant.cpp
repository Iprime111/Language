#include <cstdint>
#include <cstring>

#include "Constant.h"
#include "FunctionType.h"
#include "IRContext.h"
#include "Value.h"

namespace IR {
    Constant::Constant (const Type *type) : User (ValueId::CONSTANT, type) {}
    
    ConstantData::ConstantData (const ConstantData &data) : Constant (data.GetType ()) {
        size_t size = data.GetType ()->GetSize ();
    
        bytes = new uint8_t [size];
    
        memcpy (bytes, data.bytes, size);
    }
    
    ConstantData::ConstantData (const Type *type) : Constant (type) {
        if (!type)
            return;
    
        size_t typeSize = type->GetSize ();
    
        bytes = new uint8_t [typeSize];
        memset (bytes, 0, typeSize);
    }
    
    ConstantData::ConstantData (const Type *type, void *data) : Constant (type) {
        if (!type || !data)
            return;
        
        size_t typeSize = type->GetSize ();
    
        bytes = new uint8_t [typeSize];
        memcpy (bytes, data, typeSize);
    }
    
    ConstantData::~ConstantData () {
        delete [] bytes;
    }
    
    const uint8_t *ConstantData::GetBytes () const { return bytes; }
    
    ConstantData *ConstantData::CreateConstant (IRContext *context, const Type *type, void *data) {
        if (!context || !type || !data)
            return nullptr;
    
        ConstantData *newConstant = new ConstantData (type, data);
    
        context->constants.push_back (newConstant);
    
        return context->constants.back ();
    }
    
    ConstantData *ConstantData::CreateZeroConstant (IRContext *context, const Type *type) {
        //TODO type check
        if (!context || !type)
            return nullptr;
    
        ConstantData *newConstant = new ConstantData (type);
    
        context->constants.push_back (newConstant);
    
        return context->constants.back ();
    }
}
