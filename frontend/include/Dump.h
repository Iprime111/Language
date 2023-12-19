#ifndef DUMP_H_
#define DUMP_H_

#include <stddef.h>

#include "FrontendCore.h"

const size_t MAX_NODE_INDEX_LENGTH = 32;

#define DUMP_NODE_COLOR                     "#5e69db"

#define DUMP_CONSTANT_NODE_OUTLINE_COLOR    "#c95410"
#define DUMP_KEYWORD_NODE_OUTLINE_COLOR     "#10c929"
#define DUMP_IDENTIFIER_NODE_OUTLINE_COLOR  "#c224ce"
#define DUMP_SERVICE_NODE_OUTLINE_COLOR     "#ffffff"

#define DUMP_NEXT_CONNECTION_COLOR          "#10c94b"
#define DUMP_BACKGROUND_COLOR               "#393f87"
#define DUMP_HEADER_NODE_COLOR              "#dbd802"

CompilationError DumpSyntaxTree (CompilationContext *context, char *dumpFilename);

#endif
