#include "Argument.h"
#include "Value.h"

namespace IR {
    Argument::Argument (const Type *type, size_t argumentIndex, const char *name) : 
        Value (ValueId::ARGUMENT, type), argumentIndex (argumentIndex), argumentName (name) {}

    void Argument::SetName (const char *name) {
        argumentName = name;
    }

               size_t Argument::GetIndex () const { return argumentIndex; }
    const std::string &Argument::GetName () const { return argumentName;  }
}
