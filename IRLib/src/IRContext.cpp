#include "IRContext.h"

IRContext::~IRContext () {
    for (size_t functionIndex = 0; functionIndex < functions.size (); functionIndex++) {
        delete functions [functionIndex];
    }

    for (size_t constantIndex = 0; constantIndex < constants.size (); constantIndex++) {
        delete constants [constantIndex];
    }
}