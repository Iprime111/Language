#include "Buffer.h"
#include "FrontendCore.h"
#include "Lexer.h"
#include "Parser.h"
#include "Dump.h"
#include "ErrorWriter.h"

int main () {
    CompilationContext context = {};

    InitCompilationContext (&context);

    const char *testProgram =  "Сап конфешнс\n"
                               "Физтех-школа брс ФИВТ ( ) заебало уже\n"
                               "брс матан = 0 .\n"
                               "Физтехи постоянно ноют что матан < 10 что мне делать заебало уже\n"
                               "матан = матан + 1 .\n"
                               "каждый раз это . \n"
                               "матан = матан - 1 .\n"
                               "каждый раз это .\n";

    LexicalAnalysis (&context, testProgram);

    DumpTokenTable (&context);
    
    ParseCode (&context);

    GenerateErrorHtml (&context, "CompilationReport.html", testProgram);
    DumpSyntaxTree (&context, "tree_dump.dot");

    DestroyCompilationContext (&context);

    return 0;
}
