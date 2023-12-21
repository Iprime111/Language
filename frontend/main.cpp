#include "Buffer.h"
#include "FrontendCore.h"
#include "Lexer.h"
#include "Logger.h"
#include "Parser.h"
#include "Dump.h"
#include "ErrorWriter.h"
#include "TreeSaver.h"

static char *GetSourceFileContent (const char *filename);

int main (int argc, char **argv) {
    CompilationContext context = {};

    if (argc < 2) {
        printf ("Console argument (source file name) expected\n");
        return 0;
    }

    char *sourceData = GetSourceFileContent (argv [1]);

    if (!sourceData)
        return 0;

    InitCompilationContext (&context);

    LexicalAnalysis (&context, sourceData);

    ParseCode (&context);

    GenerateErrorHtml (&context, "CompilationReport.html", sourceData);

    SaveTree (&context, stdout);

    DumpSyntaxTree (&context, "tree_dump.dot");

    DestroyCompilationContext (&context);

    free (sourceData);

    return 0;
}

static char *GetSourceFileContent (const char *filename) {
    PushLog (4);

    FILE *sourceFile = fopen (filename, "r");

    if (!sourceFile) {
        printf ("Can not open source file\n");
        return NULL;
    }
    
    fseek (sourceFile, 0, SEEK_END);
    size_t fileSize = (size_t) ftell (sourceFile);
    fseek (sourceFile, 0, SEEK_SET);

    char *sourceData = (char *) calloc (fileSize + 1, sizeof (char));
    fread (sourceData, fileSize, 1, sourceFile);

    sourceData [fileSize] = '\0';
    
    fclose (sourceFile);

    RETURN sourceData;
}
