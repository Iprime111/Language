#include "IRContext.h"

namespace IR {
    IRContext::~IRContext () {
        for (std::list <Function *>::iterator functionIterator = functions.begin (); functionIterator != functions.end (); functionIterator++)
            delete *functionIterator;
    
        for (size_t constantIndex = 0; constantIndex < constants.size (); constantIndex++)
            delete constants [constantIndex];
    }
}
