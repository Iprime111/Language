#ifndef TREE_SAVER_H_
#define TREE_SAVER_H_

#include <stddef.h>
#include "FrontendCore.h"

const size_t MAX_NUMBER_LENGTH = 32;

CompilationError SaveTree (CompilationContext *context, FILE *stream);

#endif
