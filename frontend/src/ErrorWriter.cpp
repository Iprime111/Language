#include <cassert>
#include <stdio.h>
#include <unistd.h>
#include <libgen.h>

#include "ErrorWriter.h"
#include "Buffer.h"
#include "FrontendCore.h"
#include "NameTable.h"

#define WriteToHtml(data)                                                                   \
    do {                                                                                    \
        if (WriteStringToBuffer (dataBuffer, data) != BufferErrorCode::NO_BUFFER_ERRORS) {  \
            return CompilationError::HTML_ERROR;                                            \
        }                                                                                   \
    } while (0)

const size_t MAX_INT_LENGTH = 32;

static CompilationError CopyResourceFileContent (const char *sourceFilename, FILE *outputFile);
static CompilationError WriteError              (Buffer <char> *dataBuffer, const ErrorData *error);
static CompilationError WriteErrorText          (Buffer <char> *dataBuffer, const ErrorData *error);
static CompilationError WriteProgramText        (CompilationContext *contex, Buffer <char> *dataBuffer);
static CompilationError WriteErrors             (Buffer <char> *dataBuffer, CompilationContext *context);

CompilationError GenerateErrorHtml (CompilationContext *context, const char *filename) {
    assert (context);
    assert (filename);

    FILE *errorsFile = fopen (filename, "w");

    if (!errorsFile) {
        return CompilationError::HTML_ERROR;
    }

    Buffer <char> dataBuffer = {};

    if (InitBuffer (&dataBuffer) != BufferErrorCode::NO_BUFFER_ERRORS) {
        return CompilationError::HTML_ERROR;
    }

    char *executablePath = (char *) calloc (FILENAME_MAX, sizeof (char));
    ssize_t readBytes = readlink ("/proc/self/exe", executablePath, FILENAME_MAX);
	if (readBytes >= 0) {
		executablePath [readBytes] = '\0';
	}

	dirname (executablePath);
	strcat  (executablePath, "/Resources/pattern_header.html");

    CopyResourceFileContent (executablePath, errorsFile);

    WriteProgramText (context, &dataBuffer);
    WriteErrors      (&dataBuffer, context);
   
    fwrite (dataBuffer.data, dataBuffer.currentIndex, 1, errorsFile);

    DestroyBuffer (&dataBuffer);

    dirname (executablePath);
    strcat  (executablePath, "/pattern_footer.html");
    
    CopyResourceFileContent (executablePath, errorsFile);

    free (executablePath);
    fclose (errorsFile);

    return CompilationError::NO_ERRORS;
}

static CompilationError CopyResourceFileContent (const char *sourceFilename, FILE *outputFile) {
    assert (sourceFilename);
    assert (outputFile);

    FILE *source = fopen (sourceFilename, "r");

    if (!source) {
        return CompilationError::HTML_ERROR;
    }

    fseek (source, 0, SEEK_END);
    size_t fileSize = (size_t) ftell (source);
    fseek (source, 0, SEEK_SET);

    char *fileData = (char *) calloc (fileSize, sizeof (char));
    fread (fileData, fileSize, 1, source);
    
    fclose (source);

    fwrite (fileData, fileSize, 1, outputFile);

    free (fileData);

    return CompilationError::NO_ERRORS;
}

static CompilationError WriteProgramText (CompilationContext *context, Buffer <char> *dataBuffer) {
    assert (context);
    assert (dataBuffer);

    WriteToHtml ("<div class=\"container\">\n<div class=\"post-content\">\n<span>\n");

    char *blockEnd = NULL;

    #define KEYWORD(NAME, NUMBER, KEYWORD, TYPE, ...)   \
        if (Keyword::NAME == Keyword::BLOCK_CLOSE) {    \
            blockEnd = KEYWORD;                         \
        }

    #include "Keywords.def"

    #undef KEYWORD

    size_t currentKwordSymbol = 0;
    size_t kwordLength        = strlen (blockEnd);

    for (size_t symbolIndex = 0; symbolIndex < context->fileLength; symbolIndex++) {
        WriteDataToBuffer (dataBuffer, &context->fileContent [symbolIndex], 1);

        if (blockEnd [currentKwordSymbol] == context->fileContent [symbolIndex]) {
            if (currentKwordSymbol == kwordLength - 1 && context->fileContent [symbolIndex + 1] == '.') {
                ++symbolIndex;
                WriteToHtml (".<br>");
                currentKwordSymbol = 0;
            } else {
                ++currentKwordSymbol;
            }
        }
    }

    WriteToHtml ("</span>\n</div>\n</div>");
    WriteToHtml ("<hr class=\"stylistic-element\">\n<div class=\"comments-title\">\n<h3>Комментарии:</h3>\n</div>\n");

    return CompilationError::NO_ERRORS;
}

static CompilationError WriteErrors (Buffer <char> *dataBuffer, CompilationContext *context) {
    assert (dataBuffer);
    assert (context);

    WriteToHtml ("<ul class=\"comments-block\">");

    for (size_t errorIndex = 0; errorIndex < context->errorBuffer.currentIndex; errorIndex++) {
        WriteError (dataBuffer, &context->errorBuffer.data [errorIndex]);
    }

    WriteToHtml ("</ul>");

    return CompilationError::NO_ERRORS;
}

static CompilationError WriteErrorText (Buffer <char> *dataBuffer, const ErrorData *error) {
    assert (dataBuffer);
    assert (error);

    #define WriteErrorDescription(ERROR, MESSAGE) \
    case CompilationError::ERROR:                 \
            WriteToHtml (MESSAGE);                \
            break;

    switch (error->error) {
        WriteErrorDescription (NO_ERRORS,                       "Нет ошибок. (Это сообщение не должно было возникнуть)")
        WriteErrorDescription (CONTEXT_ERROR,                   "Ошибка контекста данных")
        WriteErrorDescription (DERIVATIVE_EXPECTED,             "Ожидался оператор производной")
        WriteErrorDescription (TOKEN_BUFFER_ERROR,              "Ошибка буффера лексем")
        WriteErrorDescription (IDENTIFIER_EXPECTED,             "Ожидался идентификатор")
        WriteErrorDescription (INITIAL_OPERATOR_EXPECTED,       "Ожидался начальный оператор")
        WriteErrorDescription (OPERATOR_SEPARATOR_EXPECTED,     "Ожидался символ '.'")
        WriteErrorDescription (TYPE_NAME_EXPECTED,              "Ожидалось имя типа")
        WriteErrorDescription (BRACKET_EXPECTED,                "Ожидалась скобка")
        WriteErrorDescription (CODE_BLOCK_EXPECTED,             "Ожидался символ начала/конца блока кода")
        WriteErrorDescription (FUNCTION_EXPECTED,               "Ожидалось объявление функции")
        WriteErrorDescription (ASSIGNMENT_EXPECTED,             "Ожидалось присваивание (оператор '=')")
        WriteErrorDescription (CONDITION_SEPARATOR_EXPECTED,    "Ожидался разделитель условия цикла")
        WriteErrorDescription (IF_EXPECTED,                     "Ожидалось выражение с условным оператором")
        WriteErrorDescription (WHILE_EXPECTED,                  "Ожидалось выражение с циклическим оператором")
        WriteErrorDescription (OPERATION_EXPECTED,              "Ожидалась операция")
        WriteErrorDescription (CONSTANT_EXPECTED,               "Ожидалась константа")
        WriteErrorDescription (DUMP_ERROR,                      "Ошибка дампа синтаксического дерева")
        WriteErrorDescription (HTML_ERROR,                      "Ошибка html файла")

        WriteErrorDescription (ARGUMENT_SEPARATOR_EXPECTED,     "Ожидался символ ','")
        WriteErrorDescription (OUTPUT_FILE_ERROR,               "Ошибка выходного файла")
        WriteErrorDescription (RETURN_EXPECTED,                 "Ожидался оператор возврата значения")
        WriteErrorDescription (BREAK_EXPECTED,                  "Ожидался оператор прерывания цикла")
        WriteErrorDescription (CONTINUE_EXPECTED,               "Ожидался оператор пропуска итерации цикла")
        WriteErrorDescription (IN_EXPECTED,                     "Ожидался оператор ввода значения")
        WriteErrorDescription (OUT_EXPECTED,                    "Ожидался оператор вывода значения")
        WriteErrorDescription (ABORT_EXPECTED,                  "Ожидался оператор окончания программы")
        WriteErrorDescription (FUNCTION_REDEFINITION,           "Функция с одним и тем же именем объявлена более одного раза")
        WriteErrorDescription (VARIABLE_REDECLARATION,          "Переменная с одним и тем же именем объявлена более одного раза")
        WriteErrorDescription (FUNCTION_NOT_DECLARED,           "Используемая функция не была объявлена")
        WriteErrorDescription (VARIABLE_NOT_DECLARED,           "Используемая переменная не была объявлена")
        WriteErrorDescription (OPERATOR_NOT_FOUND,              "Подходящий оператор не был найден")
        WriteErrorDescription (FUNCTION_CALL_EXPECTED,          "Ожидался вызов функции")
    };

    #undef WriteErrorDescription

    WriteToHtml ("<br> (Строка ");
   
    char lineNumber [MAX_INT_LENGTH] = "";
    snprintf (lineNumber, MAX_INT_LENGTH, "%d", error->line);

    WriteToHtml (lineNumber);
    WriteToHtml (")");

    return CompilationError::NO_ERRORS;
}

static CompilationError WriteError (Buffer <char> *dataBuffer, const ErrorData *error) {
    assert (dataBuffer);
    assert (error);

    WriteToHtml ("<li class=\"comment\"><span>");

    WriteErrorText (dataBuffer, error);

    WriteToHtml ("</span></li>\n<hr class=\"comment-splitter\">\n");

    return CompilationError::NO_ERRORS;
}
