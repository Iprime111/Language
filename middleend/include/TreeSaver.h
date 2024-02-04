#ifndef TREE_SAVER_H_
#define TREE_SAVER_H_

#include <stddef.h>

#include "MiddleEndCore.h"
#include "TreeReader.h"

const size_t MAX_NUMBER_LENGTH = 32;

TranslationError SaveTree (TranslationContext *context, FILE *stream);

#endif
