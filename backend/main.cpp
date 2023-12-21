#include "AssemblyGaynerator.h"
#include "BackendCore.h"
#include "TreeReader.h"
#include "Dump.h"

static char *GetTreeFileContent (const char *filename);

int main (int argc, char **argv) {

    TranslationContext context = {};

    if (argc < 2) {
        printf ("Console argument (tree file name) expected\n");
        return 0;
    }

    char *treeData = GetTreeFileContent (argv [1]);

    if (!treeData)
        return 0;

    InitTranslationContext (&context);

    ReadSyntaxTree (&context, treeData);
    DumpSyntaxTree (&context, "tree_dump.dot");
    
    GenerateAssembly (&context, stdout);

    DestroyTranslationContext (&context);
    free (treeData);
}

static char *GetTreeFileContent (const char *filename) {
    PushLog (4);

    FILE *treeFile = fopen (filename, "r");

    if (!treeFile) {
        printf ("Can not open tree file\n");
        return NULL;
    }
    
    fseek (treeFile, 0, SEEK_END);
    size_t fileSize = (size_t) ftell (treeFile);
    fseek (treeFile, 0, SEEK_SET);

    char *treeData = (char *) calloc (fileSize + 1, sizeof (char));
    fread (treeData, fileSize, 1, treeFile);

    treeData [fileSize] = '\0';
    
    fclose (treeFile);

    RETURN treeData;
}
