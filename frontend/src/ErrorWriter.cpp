#include <stdio.h>
#include <unistd.h>
#include <libgen.h>

#include "ErrorWriter.h"
#include "Buffer.h"
#include "FrontendCore.h"
#include "Logger.h"

#define WriteToHtml(data)                                                                   \
    do {                                                                                    \
        if (WriteStringToBuffer (dataBuffer, data) != BufferErrorCode::NO_BUFFER_ERRORS) {  \
            RETURN CompilationError::HTML_ERROR;                                            \
        }                                                                                   \
    } while (0)

const size_t MAX_INT_LENGTH = 32;

static CompilationError CopyResourceFileContent (const char *sourceFilename, FILE *outputFile);
static CompilationError WriteError              (Buffer <char> *dataBuffer, const ErrorData *error);
static CompilationError WriteErrorText          (Buffer <char> *dataBuffer, const ErrorData *error);
static CompilationError WriteProgramText        (Buffer <char> *dataBuffer, const char *programText);
static CompilationError WriteErrors             (Buffer <char> *dataBuffer, CompilationContext *context);

CompilationError GenerateErrorHtml (CompilationContext *context, const char *filename, const char *programText) {
    PushLog (3);

    FILE *errorsFile = fopen (filename, "w");

    if (!errorsFile) {
        RETURN CompilationError::HTML_ERROR;
    }

    Buffer <char> dataBuffer = {};

    if (InitBuffer (&dataBuffer) != BufferErrorCode::NO_BUFFER_ERRORS) {
        RETURN CompilationError::HTML_ERROR;
    }

    char *executablePath = (char *) calloc (FILENAME_MAX, sizeof (char));
    ssize_t readBytes = readlink ("/proc/self/exe", executablePath, FILENAME_MAX);
	if (readBytes >= 0) {
		executablePath [readBytes] = '\0';
	}

	dirname (executablePath);
	strcat  (executablePath, "/Resources/pattern_header.html");

    CopyResourceFileContent (executablePath, errorsFile);

    WriteProgramText (&dataBuffer, programText);
    WriteErrors      (&dataBuffer, context);
   
    fwrite (dataBuffer.data, dataBuffer.currentIndex, 1, errorsFile);

    DestroyBuffer (&dataBuffer);

    dirname (executablePath);
    strcat  (executablePath, "/pattern_footer.html");
    
    CopyResourceFileContent (executablePath, errorsFile);

    free (executablePath);
    fclose (errorsFile);

    RETURN CompilationError::NO_ERRORS;
}

static CompilationError CopyResourceFileContent (const char *sourceFilename, FILE *outputFile) {
    PushLog (4);

    FILE *source = fopen (sourceFilename, "r");

    if (!source) {
        RETURN CompilationError::HTML_ERROR;
    }

    fseek (source, 0, SEEK_END);
    size_t fileSize = (size_t) ftell (source);
    fseek (source, 0, SEEK_SET);

    char *fileData = (char *) calloc (fileSize, sizeof (char));
    fread (fileData, fileSize, 1, source);
    
    fclose (source);

    fwrite (fileData, fileSize, 1, outputFile);

    free (fileData);

    RETURN CompilationError::NO_ERRORS;
}

static CompilationError WriteProgramText (Buffer <char> *dataBuffer, const char *programText) {
    PushLog (4);

    WriteToHtml ("<div class=\"container\">\n<div class=\"post-content\">\n<span>\n");
    WriteToHtml (programText);
    WriteToHtml ("</span>\n</div>\n</div>");
    WriteToHtml ("<hr class=\"stylistic-element\">\n<div class=\"comments-title\">\n<h3>Комментарии:</h3>\n</div>\n");

    RETURN CompilationError::NO_ERRORS;
}

static CompilationError WriteErrors (Buffer <char> *dataBuffer, CompilationContext *context) {
    PushLog (4);

    WriteToHtml ("<ul class=\"comments-block\">");

    for (size_t errorIndex = 0; errorIndex < context->errorBuffer.currentIndex; errorIndex++) {
        WriteError (dataBuffer, &context->errorBuffer.data [errorIndex]);
    }

    WriteToHtml ("</ul>");

    RETURN CompilationError::NO_ERRORS;
}

static CompilationError WriteErrorText (Buffer <char> *dataBuffer, const ErrorData *error) {
    PushLog (4);

    #define WriteErrorDescription(ERROR, MESSAGE) \
    case CompilationError::ERROR:                 \
            WriteToHtml (MESSAGE);                \
            break;

    switch (error->error) {
        WriteErrorDescription (NO_ERRORS,                       "Нет ошибок. (Это сообщение возникло по ошибке)")
        WriteErrorDescription (CONTEXT_ERROR,                   "")
        WriteErrorDescription (DERIVATIVE_EXPECTED,             "DERIVATIVE_EXPECTED")
        WriteErrorDescription (TOKEN_BUFFER_ERROR,              "Какие же физтехи слабые, не могут написать пост без нытья")
        WriteErrorDescription (IDENTIFIER_EXPECTED,             "Опять посты пишут отсталые школьники из ФТЛ, которые даже нормально название придумать не могут")
        WriteErrorDescription (INITIAL_OPERATOR_EXPECTED,       "Почему автор не поздоровался? Это невежливо ((")
        WriteErrorDescription (OPERATOR_SEPARATOR_EXPECTED,     "Опять ФИВТы посты пишут? Никаких знаков препинания нет, читать невозможно")
        WriteErrorDescription (TYPE_NAME_EXPECTED,              "Я вообще не понял, что это за предмет, о котором пишет автор")
        WriteErrorDescription (BRACKET_EXPECTED,                "Я кстати люблю скобочки...")
        WriteErrorDescription (CODE_BLOCK_EXPECTED,             "Желаю автору научиться нормально отделять мысли друг от друга")
        WriteErrorDescription (FUNCTION_EXPECTED,               "И что оно должно по-твоему делать? Отчислись пж, не занимай чужое место")
        WriteErrorDescription (ASSIGNMENT_EXPECTED,             "Неужели учась на физтехе человек не может понять, что переменной нужно присвоить значение?")
        WriteErrorDescription (CONDITION_SEPARATOR_EXPECTED,    "Подскажите, где тут кончается одна мысль и начинается другая?")
        WriteErrorDescription (IF_EXPECTED,                     "Всегда думал, что будет, если вдруг в такой ситуации сложатся другие условия...")
        WriteErrorDescription (WHILE_EXPECTED,                  "Я не знаю что тут писать")
        WriteErrorDescription (OPERATION_EXPECTED,              "И что мне делать? Смеяться? Дрочить? Плакать?")
        WriteErrorDescription (CONSTANT_EXPECTED,               "Какие же физтехи жалкие, не могут даже число написать")
        WriteErrorDescription (DUMP_ERROR,                      "JAMclub")
        WriteErrorDescription (HTML_ERROR,                      "Какой долбоеб написал сайт конфешнса так, что он не может отобразить мой комментарий")

        WriteErrorDescription (ARGUMENT_SEPARATOR_EXPECTED,     "ARGUMENT_SEPARATOR_EXPECTED")
        WriteErrorDescription (OUTPUT_FILE_ERROR,               "OUTPUT_FILE_ERROR")
        WriteErrorDescription (RETURN_EXPECTED,                 "RETURN_EXPECTED")
        WriteErrorDescription (BREAK_EXPECTED,                  "BREAK_EXPECTED")
        WriteErrorDescription (CONTINUE_EXPECTED,               "CONTINUE_EXPECTED")
        WriteErrorDescription (IN_EXPECTED,                     "IN_EXPECTED")
        WriteErrorDescription (OUT_EXPECTED,                    "OUT_EXPECTED")
        WriteErrorDescription (ABORT_EXPECTED,                  "ABORT_EXPECTED")
        WriteErrorDescription (FUNCTION_REDEFINITION,           "FUNCTION_REDEFINITION")
        WriteErrorDescription (VARIABLE_REDECLARATION,          "VARIABLE_REDECLARATION")
        WriteErrorDescription (FUNCTION_NOT_DECLARED,           "FUNCTION_NOT_DECLARED")
        WriteErrorDescription (VARIABLE_NOT_DECLARED,           "VARIABLE_NOT_DECLARED")
        WriteErrorDescription (OPERATOR_NOT_FOUND,              "OPERATOR_NOT_FOUND")
        WriteErrorDescription (FUNCTION_CALL_EXPECTED,          "FUNCTION_CALL_EXPECTED")
    };

    #undef WriteErrorDescription

    WriteToHtml ("<br> (Строка ");
   
    char lineNumber [MAX_INT_LENGTH] = "";
    snprintf (lineNumber, MAX_INT_LENGTH, "%d", error->line);

    WriteToHtml (lineNumber);
    WriteToHtml (")");

    RETURN CompilationError::NO_ERRORS;
}

static CompilationError WriteError (Buffer <char> *dataBuffer, const ErrorData *error) {
    PushLog (4);

    WriteToHtml ("<li class=\"comment\"><span>");

    WriteErrorText (dataBuffer, error);

    WriteToHtml ("</span></li>\n<hr class=\"comment-splitter\">\n");

    RETURN CompilationError::NO_ERRORS;
}
