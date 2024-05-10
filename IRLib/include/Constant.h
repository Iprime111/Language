#ifndef CONSTANT_H_
#define CONSTANT_H_

#include <cstdint>

#include "FunctionType.h" // TODO: unknown type size_t. Did you mean std::size_t?
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
        ConstantData (const ConstantData &data); // TODO: do you really need copy ctor? 
                                                 // Read about "Rule of 0" and "Rule of 5"

        ConstantData (const Type *type);
        ConstantData (const Type *type, void *data); // TODO: you usually doesn't want to use void* in C++.
        ~ConstantData ();

        const uint8_t *GetBytes () const;

        static ConstantData *GetConstant (IRContext *context, const Type *type, void *data);

    private:
        uint8_t *bytes = nullptr;
};

#endif
