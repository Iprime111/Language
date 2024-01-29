
#include "ReverseFrontendCore.h"
#include "TreeReader.h"
#include "TreeTranslator.h"

int main () {

    TranslationContext context = {};

    ReadSyntaxTree (&context.abstractSyntaxTree, "( 3 41 ( 4 0  ( 3 51 _ _ ) ( 5  _ ( 3 41 ( 6 1  ( 3 51 _ _ ) ( 3 13 ( 1 1 _ _ ) ( 2 1 _ _ ) ) ) _ ) ) ) _ )");
    ReadNameTables (&context.nameTable, &context.localTables, "2\nФИВТ\nффф\n2\n1\n0 1\n1\n1 2");

    TranslateTree (&context, stdout);

    DestroyTranslationContext (&context);
}
