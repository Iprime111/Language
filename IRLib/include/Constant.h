#ifndef CONSTANT_H_
#define CONSTANT_H_

#include <cstdint>

#include "FunctionType.h"
#include "User.h"

//TODO Think whether I really need this...
class Constant : public User {
    public:
        virtual ~Constant () = 0;

    protected:
        Constant ();

};

//TODO improve a way to set constant value
class ConstantData : public Constant {
    public:
        ~ConstantData ();

        ConstantData (Type *type);
        ConstantData (Type *type, void *data);

        uint8_t *GetBytes ();

        static ConstantData *GetConstant (IRContext *context, ConstantData constant);

    private:
        Type    *type  = nullptr;
        uint8_t *bytes = nullptr;
};

#endif
