#include <cstdint>

#include "Constant.h"
#include "IRContext.h"
#include "Buffer.h"
#include "Value.h"

Constant::Constant () : User (ValueType::CONSTANT) {}

ConstantData::ConstantData (Type *type) : type (type) {
    if (!type)
        return;

    bytes = new uint8_t [type->GetSize ()];
}

ConstantData::ConstantData (Type *type, void *data) : ConstantData (type) {
    if (!type || !bytes)
        return;

    memcpy (bytes, data, type->GetSize ());
}

ConstantData::~ConstantData () {
    delete bytes;
}

ConstantData *ConstantData::GetConstant (IRContext *context, ConstantData constant) {
    if (!context)
        return nullptr;

    if (WriteDataToBuffer (&context->constants, &constant, 1) != BufferErrorCode::NO_BUFFER_ERRORS)
        return nullptr;

    return &context->constants.data [context->constants.currentIndex - 1];
}

