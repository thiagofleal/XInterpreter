#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>
#include "header.h"
#include "util/util.h"

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
            ? L"\n -> %s: %i\n >> Before \"%s\" -> %s: %s\n"
            : L"\n -> %s: %i\n >> Before \"%s\" -> %s\n",
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

wstring_t open_file(wstring_t fileName){
    register int count = 0;
    wchar_t wc;
    wstring_t ret, aux;
    FILE *f = _wfopen(fileName, L"r");

    check(f);

    while(!feof(f)){
        fwscanf(f, L"%c", &wc);
        ++ count;
    }

    rewind(f);
    ret = aux = malloc(count * sizeof(wchar_t));
    check(ret);

    while(!feof(f)){
        fwscanf(f, L"%c", &wc);
        * aux ++ = wc;
    }

    * -- aux = L'\0';
    return ret;
}

int main(int argc, string_t argv[]){
    return 0;
}
