#include <cstdint>
#include <cstring>

#include "Constant.h"
#include "FunctionType.h"
#include "IRContext.h"
#include "Value.h"

Constant::Constant (const Type *type) : User (ValueId::CONSTANT, type) {}

ConstantData::ConstantData (const ConstantData &data) : Constant (data.GetType ()) {
    size_t size = data.GetType ()->GetSize ();

    bytes = new uint8_t [size];

    memcpy (bytes, data.bytes, size);
}

ConstantData::ConstantData (const Type *type) : Constant (type) {
    if (!type)
        return;

    bytes = new uint8_t [type->GetSize ()];
}

ConstantData::ConstantData (const Type *type, void *data) : ConstantData (type) {
    if (!type || !bytes)
        return;

    memcpy (bytes, data, type->GetSize ());
}

ConstantData::~ConstantData () {
    delete [] bytes;
}

const uint8_t *ConstantData::GetBytes () const { return bytes; }

ConstantData *ConstantData::GetConstant (IRContext *context, const Type *type, void *data) {
    if (!context)
        return nullptr;

    context->constants.emplace_back (type, data);

    return &context->constants [context->constants.size () - 1];
}

