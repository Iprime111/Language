#include <stdio.h>

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

    if (!sourceData) {
        printf ("Can not read source data");
        return 0;
    }
    
    printf ("Reading code...\n");

    InitCompilationContext (&context, sourceData);

    LexicalAnalysis (&context);

    printf ("Parsing tokens...\n");

    ParseCode (&context);

    printf ("Generating report...\n");

    GenerateErrorHtml (&context, "CompilationReport.html");

    FILE *nameTableFile = fopen ("NameTables.tmp", "w");
    FILE *treeFile      = fopen ("SyntaxTree.tmp", "w");

    if (!nameTableFile || !treeFile) {
        printf ("Error while opening output file\n");
        return 0;
    }

    SaveTree          (&context, treeFile);
    SaveNameTables    (&context, nameTableFile);

    fclose (nameTableFile);
    fclose (treeFile);

    DumpSyntaxTree (&context, "TreeDumpFrontend.dot");

    DestroyCompilationContext (&context);

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
