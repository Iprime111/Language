#include "IRContext.h"
#include "Buffer.h"

IRContext::IRContext () {
    InitBuffer (&functions);
}

IRContext::~IRContext () {
    // TODO: destroy basic blocks
    DestroyBuffer (&functions);
}
