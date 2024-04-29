#include <cassert>

#include "BackendCore.h"
#include "Buffer.h"
#include "NameTable.h"
#include "TreeDefinitions.h"
#include "TreeReader.h"

TranslationError InitTranslationContext (TranslationContext *context) {
    assert (context);

    if (InitNameTable (&context->nameTable, false) != BufferErrorCode::NO_BUFFER_ERRORS)
        return TranslationError::NAME_TABLE_ERROR;

    if (InitBuffer (&context->localTables) != BufferErrorCode::NO_BUFFER_ERRORS)
        return TranslationError::NAME_TABLE_ERROR;

    if (InitBuffer (&context->functions) != BufferErrorCode::NO_BUFFER_ERRORS)
        return TranslationError::CONTEXT_ERROR;

    AddLocalNameTable (0, &context->localTables);

    context->error = TranslationError::NO_ERRORS;

    return TranslationError::NO_ERRORS;
}

TranslationError DestroyTranslationContext (TranslationContext *context) {
    assert (context);

    Tree::DestroySubtreeNode (&context->abstractSyntaxTree, context->abstractSyntaxTree.root);

    for (size_t nameIndex = 0; nameIndex < context->nameTable.currentIndex; nameIndex++) {
        if (context->nameTable.data [nameIndex].keyword == Keyword::NOT_KEYWORD)
            free (const_cast <char *> (context->nameTable.data [nameIndex].name));
    }

    DestroyBuffer (&context->nameTable);

    for (size_t localTableIndex = 0; localTableIndex < context->localTables.currentIndex; localTableIndex++)
        DestroyBuffer (&context->localTables.data [localTableIndex].items);

    DestroyBuffer (&context->localTables);
    DestroyBuffer (&context->functions);
    //TODO destroy all basic blocks

    return TranslationError::NO_ERRORS;
}
