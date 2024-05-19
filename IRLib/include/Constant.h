#ifndef CONSTANT_H_
#define CONSTANT_H_

#include <cstdint>

#include "Type.h"
#include "User.h"

namespace IR {
    class Constant : public User {
        public:
            virtual ~Constant () = default;
    
        protected:
            explicit Constant (const Type *type);
    };
    
    //TODO improve a way to set constant value
    class ConstantData : public Constant {
        public:
            ConstantData (const ConstantData &data);
    
            explicit ConstantData (const Type *type);
            explicit ConstantData (const Type *type, void *data);
            ~ConstantData ();
    
            const uint8_t *GetBytes () const;
    
            static ConstantData *CreateConstant     (IRContext *context, const Type *type, void *data);
            static ConstantData *CreateZeroConstant (IRContext *context, const Type *type);
    
        private:
            uint8_t *bytes = nullptr;
    };
}
#endif
