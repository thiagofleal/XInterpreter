#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>
#include "header.h"
#include "util/util.h"

static wstring_t wstr_error[] = {
    #ifdef __TEST__
        [expected_token] = L"expected token",
        [non_terminated_single_quot] = L"non terminated single quotes",
        [non_terminated_double_quot] = L"non terminated double quotes",
        [non_terminated_commentary] = L"non terminated commentary",
        [undeclared_variable] = L"undeclared variable",
        [undeclared_function] = L"undeclared function",
        [undeclared_attribute] = L"undeclared attribute",
        [undeclared_method] = L"undeclared method",
        [undeclared_class] = L"undeclared class",
        [illegal_number] = L"illegal number",
        [array_assignment_error] = L"array assignment error",
        [array_bounds_error] = L"array bounds error",
        [not_boolean_expression] = L"not boolean expression",
        [syntax_error] = L"syntax error"
    #else
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
        [not_boolean_expression] = L"",
        [syntax_error] = L""
    #endif // __TEST__
};

INLINE void printError(type_error error, token_t from, wstring_t message){
    static wstring_t before
    #ifdef __TEST__
        = L"Before"
    #else
        = L"";
    #endif // __TEST__
    ;
    fwprintf(
        stderr,
        message
            ? L"\n -> %s: %i\n\t>> %s <%s>: %s: %s\n"
            : L"\n -> %s: %i\n\t>> %s <%s>: %s\n",
        from.file,
        from.line,
        before,
        from.value,
        wstr_error[error],
        message
    );
}

INLINE void expectedToken(type_token type, int intern, wstring_t value){
    if((++ token) -> intern != intern){
        wchar_t str[100];
        swprintf(str, L"<%s>", value);
        printError(expected_token, * token, str);
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
