#ifndef CONSTANT_H_
#define CONSTANT_H_

#include <cstdint>

#include "FunctionType.h"
#include "User.h"

//TODO Think whether I really need this...
class Constant : public User {
    public:
        virtual ~Constant () = default;

    protected:
        Constant (const Type *type);

};

//TODO improve a way to set constant value
class ConstantData : public Constant {
    public:
        ConstantData (const ConstantData &data);

        ConstantData (const Type *type);
        ConstantData (const Type *type, void *data);
        ~ConstantData ();

        const uint8_t *GetBytes () const;

        static ConstantData *GetConstant (IRContext *context, const Type *type, void *data);

    private:

        uint8_t *bytes = nullptr;
};

#endif
