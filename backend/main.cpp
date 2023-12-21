#include "AssemblyGaynerator.h"
#include "BackendCore.h"
#include "TreeReader.h"
#include "Dump.h"

int main (int argc, char **argv) {

    TranslationContext context = {};

    if (argc < 2) {
        printf ("Console argument (tree file name) expected\n");
    }

    FILE *treeFile = fopen (argv [1], "r");

    if (!treeFile) {
        printf ("Can not open tree file\n");
        return 0;
    }
    
    fseek (treeFile, 0, SEEK_END);
    size_t fileSize = (size_t) ftell (treeFile);
    fseek (treeFile, 0, SEEK_SET);

    char *treeData = (char *) calloc (fileSize + 1, sizeof (char));
    fread (treeData, fileSize, 1, treeFile);

    treeData [fileSize] = '\0';
    
    fclose (treeFile);

    InitTranslationContext (&context);

    ReadSyntaxTree (&context, treeData);
    DumpSyntaxTree (&context, "tree_dump.dot");
    
    GenerateAssembly (&context, stdout);

    DestroyTranslationContext (&context);
    free (treeData);
}
