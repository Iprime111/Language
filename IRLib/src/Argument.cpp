#include "Argument.h"
#include "Value.h"

namespace IR {
    Argument::Argument (const Type *type, size_t argumentIndex) : Value (ValueId::ARGUMENT, type), argumentIndex (argumentIndex) {}

    size_t Argument::GetIndex () const { return argumentIndex; }
}
