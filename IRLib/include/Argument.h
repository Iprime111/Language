#ifndef ARGUMENT_H_
#define ARGUMENT_H_

#include "Value.h"

namespace IR {
    
    class Argument : public Value {
        public:
            explicit Argument (const Type *argumentType, size_t argumentIndex);
            
            size_t GetIndex () const;

        private:
            size_t argumentIndex;
    };
};

#endif
