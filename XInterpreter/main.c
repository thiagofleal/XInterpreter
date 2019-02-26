#include <stdio.h>
#include <stdlib.h>
#include "header.h"
#include "util/util.h"

extern void initTokens(const string_t, const wstring_t);

static wstring_t wstr_error[] = {
    [expected_token] = L"",
    [non_terminated_single_quot] = L"",
    [non_terminated_double_quot] = L"",
    [non_terminated_commentary] = L"",
    [undeclared_variable] = L"",
    [undeclared_function] = L"",
    [undeclared_attribute] = L"",
    [undeclared_method] = L"",
    [undeclared_class] = L"",
    [illegal_number] = L"",
    [array_assignment_error] = L"",
    [array_bounds_error] = L"",
    [syntax_error] = L""
};

INLINE void printError(type_error error, token_t from, wstring_t message){
    fwprintf(
        stderr,
        message
            ? L"\n -> %hs : %d >> %ls\n -> %s\n"
            : L"\n -> %hs : %d >> %ls\n -> %s: %ls\n",
        from.file,
        from.line,
        from.value,
        wstr_error[error],
        message
    );
}

INLINE void expectedToken(type_token type, int intern, wstring_t value){
    if((++ token) -> intern != intern){
        printError(expected_token, * token, value);
    }
}

extern void registerKeyWord(int, wstring_t);

int main(int argc, string_t argv[]){
    return 0;
}
