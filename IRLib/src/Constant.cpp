#include <cstdint>
#include <cstring>

#include "Constant.h"
#include "FunctionType.h"
#include "IRContext.h"
#include "Value.h"

// TODO: you haven't allocated `bytes` array.
//       this means your class have invalid state after creation

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

// TODO it's probably not 'GetConstant' but 'CreateConstant'
//      it's also probably should be method of 'class Constant'
ConstantData *ConstantData::GetConstant (IRContext *context, const Type *type, void *data) {
    if (!context)
        return nullptr;

    ConstantData *newConstant = new ConstantData (type, data);

    context->constants.push_back (newConstant);

    return context->constants [context->constants.size () - 1]; // TODO use .back method of std::vector
}

