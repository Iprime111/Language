#ifndef ARGUMENT_H_
#define ARGUMENT_H_

#include <string>

#include "Value.h"

namespace IR {
    
    class Argument : public Value {
        public:
            explicit Argument (const Type *argumentType, size_t argumentIndex, const char *name);
            
            void SetName (const char *name);

            size_t             GetIndex () const;
            const std::string &GetName  () const;

        private:
            size_t argumentIndex;
            std::string argumentName;
    };
};

#endif
