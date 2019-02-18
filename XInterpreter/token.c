#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "header.h"
#include "util/util.h"

token_t *token;

static string_t pfile;
static wstring_t prog;

static list identifiers = NULL;

wchar_t getCharacter(void){
    wchar_t ret;
    if(* prog == L'\\'){
        ++ prog;
        switch(* prog ++){
            case L'0':
            case L'1':
            case L'2':
            case L'3':
            case L'4':
            case L'5':
            case L'6':
            case L'7':
            case L'8':
            case L'9':
                ret = (wchar_t)wcstol(prog - 1, &prog, 10);
                return ret;
            case L'a':
                ret = L'\a';
                return ret;
            case L'b':
                ret = L'\b';
                return ret;
            case L'n':
                ret = L'\n';
                return ret;
            case L'r':
                ret = L'\r';
                return ret;
            case L't':
                ret = L'\t';
                return ret;
            case L'v':
                ret = L'\v';
                return ret;
            case L'\'':
                ret = L'\'';
                return ret;
            case L'\"':
                ret = L'\"';
                return ret;
        }
    }
    return * prog ++;
}

static int getLine(wstring_t origin){
    static int line = 1;
    static wstring_t current = NULL;

    if(origin){
        current = origin;
        line = 1;
    }

    for(; current != prog; current ++){
        if(* current == L'\n'){
            ++ line;
        }
    }

    return line;
}

static wstring_t key_words[] = {
    [key_character] = L"",
    [key_boolean] = L"",
    [key_integer] = L"",
    [key_real] = L"",
    [key_string] = L"",
    [key_object] = L"",
    [key_true] = L"",
    [key_false] = L"",
    [key_null] = L"",
    [key_if] = L"",
    [key_else] = L"",
    [key_do] = L"",
    [key_while] = L"",
    [key_for] = L"",
    [key_each] = L"",
    [key_using] = L"",
    [key_return] = L"",
    [key_call] = L"",
    [key_include] = L"",
    [key_block] = L"",
    [key_try] = L"",
    [key_catch] = L"",
    [key_finally] = L"",
    [key_throw] = L"",
    [key_class] = L"",
    [key_extend] = L"",
    [key_public] = L"",
    [key_protected] = L"",
    [key_private] = L"",
    [key_constructor] = L"",
    [key_destructor] = L"",
    [key_new] = L"",
    [key_this] = L"",
    [key_base] = L"",
    [key_virtual] = L""
};

static uint_t keyWord(wstring_t word){
    register int i;
    for(i = key_character; i <= key_virtual; i++){
        if(!wcscmp(word, key_words[i])){
            return i + tok_reserved;
        }
    }
    return -1;
}

static token_t getToken(void){
    token_t ret = {};
    wchar_t wstr[2000] = L"";
    wstring_t pwstr = wstr;

    if(!identifiers){
        identifiers = new_list();
    }

    ret.file = pfile;

    /* Ignore white spaces */
    while(iswspace(* prog)){
        ++ prog;
    }

    /* Ignore commentaries */
    if(* prog == L'/'){
        if(* (prog + 1) == L'*'){
            ++ prog;
            do{
                while(* ++ prog != L'*')
                    if(! * prog)
                        printError(non_terminated_commentary, ret, NULL);
            }while(* ++ prog != L'/');
            return getToken();
        }
        else if(* (prog + 1) == L'/'){
            while(* ++ prog != L'\n');
            return getToken();
        }
    }

    ret.line = getLine(NULL);

    /* End of program */
    if(! * prog){
        ret.type = tok_end;
        ret.value = NULL;
        ret.intern = tok_end;
        return ret;
    }

    /* Constants */
    if(iswdigit(* prog)){
        while(iswdigit(* prog) || * prog == L'.'){
            * pwstr ++ = * prog ++;
        }
        * pwstr = 0;
        ret.type = tok_constant;
        ret.value = new_wstring(wstr);
        ret.intern = tok_constant;
        return ret;
    }
    if(* prog == L'\''){
        ++ prog;
        * pwstr ++ = getCharacter();
        if(* prog ++ != L'\''){
            printError(non_terminated_single_quot, ret, NULL);
            exit(EXIT_FAILURE);
        }
        * pwstr = 0;
        ret.type = tok_constant;
        ret.value = new_wstring(wstr);
        ret.intern = tok_constant;
        return ret;
    }

    /* Strings */
    if(* prog == L'\"'){
        ++ prog;
        while(* prog != L'\"'){
            * pwstr ++ = getCharacter();
            if(! * prog)
                printError(non_terminated_double_quot, ret, NULL);
        }
        * pwstr = 0;
        ret.type = tok_string;
        ret.value = new_wstring(wstr);
        wprintf(L" -> %s\n", ret.value);
        ret.intern = tok_string;
        ++ prog;
        return ret;
    }

    /* Punctuation */
    if(wcschr(L"(){},;:", * prog)){
        * pwstr ++ = * prog ++;
        * pwstr = 0;
        ret.type = tok_punctuation;
        ret.value = new_wstring(wstr);
        ret.intern = * wstr + tok_punctuation;
        return ret;
    }

    /* Operators */
    if(wcschr(L"=+-*/%^\\><&|!~@#$&?.[]", * prog)){
        ret.type = tok_operator;
        switch(* prog ++){
            case L'=':
                switch(* prog ++){
                    case L'>':
                        ret.value = new_wstring(L"=>");
                        ret.intern = op_arrow;
                        return ret;
                    case L'=':
                        ret.value = new_wstring(L"==");
                        ret.intern = op_equal;
                        return ret;
                    default:
                        -- prog;
                        ret.value = new_wstring(L"=");
                        ret.intern = op_assignment;
                        return ret;
                }
                break;

            case L'+':
                switch(* prog ++){
                    case L'+':
                        ret.value = new_wstring(L"++");
                        ret.intern = op_increment;
                        return ret;
                    case L'=':
                        ret.value = new_wstring(L"+=");
                        ret.intern = op_addition_assignment;
                        return ret;
                    default:
                        -- prog;
                        ret.value = new_wstring(L"+");
                        ret.intern = op_addition;
                        return ret;
                }
                break;

            case L'-':
                switch(* prog ++){
                    case L'-':
                        ret.value = new_wstring(L"--");
                        ret.intern = op_decrement;
                        return ret;
                    case L'=':
                        ret.value = new_wstring(L"-=");
                        ret.intern = op_subtraction_assignment;
                        return ret;
                    case L'>':
                        ret.value = new_wstring(L"->");
                        ret.intern = op_if_then;
                        return ret;
                    default:
                        -- prog;
                        ret.value = new_wstring(L"-");
                        ret.intern = op_subtraction;
                        return ret;
                }
                break;

            case L'*':
                switch(* prog ++){
                    case L'=':
                        ret.value = new_wstring(L"*=");
                        ret.intern = op_multiply_assignment;
                        return ret;
                    default:
                        -- prog;
                        ret.value = new_wstring(L"*");
                        ret.intern = op_multiply;
                        return ret;
                }
                break;

            case L'/':
                switch(* prog ++){
                    case L'=':
                        ret.value = new_wstring(L"/=");
                        ret.intern = op_division_assignment;
                        return ret;
                    default:
                        -- prog;
                        ret.value = new_wstring(L"/");
                        ret.intern = op_division;
                        return ret;
                }
                break;

            case L'%':
                switch(* prog ++){
                    case L'=':
                        ret.value = new_wstring(L"%=");
                        ret.intern = op_module_assignment;
                        return ret;
                    default:
                        -- prog;
                        ret.value = new_wstring(L"%");
                        ret.intern = op_module;
                        return ret;
                }
                break;

            case L'^':
                switch(* prog ++){
                    case L'=':
                        ret.value = new_wstring(L"^=");
                        ret.intern = op_pow_assignment;
                        return ret;
                    default:
                        -- prog;
                        ret.value = new_wstring(L"^");
                        ret.intern = op_pow;
                        return ret;
                }
                break;

            case L'\\':
                switch(* prog ++){
                    case L'=':
                        ret.value = new_wstring(L"\\=");
                        ret.intern = op_radix_assignment;
                        return ret;
                    default:
                        -- prog;
                        ret.value = new_wstring(L"\\");
                        ret.intern = op_radix;
                        return ret;
                }
                break;

            case L'>':
                switch(* prog ++){
                    case L'=':
                        ret.value = new_wstring(L">=");
                        ret.intern = op_larger_or_equal;
                        return ret;
                    case L'>':
                        ret.value = new_wstring(L">>");
                        ret.intern = op_double_right;
                        return ret;
                    default:
                        -- prog;
                        ret.value = new_wstring(L">");
                        ret.intern = op_larger;
                        return ret;
                }
                break;

            case L'<':
                switch(* prog ++){
                    case L'=':
                        ret.value = new_wstring(L"<=");
                        ret.intern = op_less_or_equal;
                        return ret;
                    case L'<':
                        ret.value = new_wstring(L"<<");
                        ret.intern = op_double_left;
                        return ret;
                    case L'>':
                        ret.value = new_wstring(L"<>");
                        ret.intern = op_only_if;
                        return ret;
                    default:
                        -- prog;
                        ret.value = new_wstring(L"<");
                        ret.intern = op_less;
                        return ret;
                }
                break;

            case L'&':
                switch(* prog ++){
                    case L'&':
                        ret.value = new_wstring(L"&&");
                        ret.intern = op_and;
                        return ret;
                    default:
                        -- prog;
                        ret.value = new_wstring(L"&");
                        ret.intern = op_ampersand;
                        return ret;
                }
                break;

            case L'|':
                switch(* prog ++){
                    case L'|':
                        ret.value = new_wstring(L"||");
                        ret.intern = op_or;
                        return ret;
                    case L'&':
                        ret.value = new_wstring(L"|&");
                        ret.intern = op_xor;
                        return ret;
                    default:
                        -- prog;
                        ret.value = new_wstring(L"|");
                        ret.intern = op_pipe;
                        return ret;
                }
                break;

            case L'!':
                switch(* prog ++){
                    case L'!':
                        ret.value = new_wstring(L"!!");
                        ret.intern = op_negative_assignment;
                        return ret;
                    case L'=':
                        ret.value = new_wstring(L"!=");
                        ret.intern = op_different;
                        return ret;
                    case L'~':
                        ret.value = new_wstring(L"!~");
                        ret.intern = op_near_or_identical;
                        return ret;
                    default:
                        -- prog;
                        ret.value = new_wstring(L"!");
                        ret.intern = op_not;
                        return ret;
                }
                break;

            case L'~':
                switch(* prog ++){
                    case L'~':
                        ret.value = new_wstring(L"~~");
                        ret.intern = op_near;
                        return ret;
                    case L'=':
                        ret.value = new_wstring(L"~=");
                        ret.intern = op_near_or_identical;
                        return ret;
                    default:
                        -- prog;
                        ret.value = new_wstring(L"~");
                        ret.intern = op_tilde;
                        return ret;
                }
                break;

            case L'?':
                switch(* prog ++){
                    case L'?':
                        ret.value = new_wstring(L"??");
                        ret.intern = op_double_query;
                        return ret;
                    case L'.':
                        ret.value = new_wstring(L"?.");
                        ret.intern = op_query_dot;
                        return ret;
                    default:
                        -- prog;
                        ret.value = new_wstring(L"?");
                        ret.intern = op_query;
                        return ret;
                }
                break;

            case L'.':
                ret.value = new_wstring(L".");
                ret.intern = op_dot;
                return ret;

            case L'@':
                ret.value = new_wstring(L"@");
                ret.intern = op_at;
                return ret;

            case L'#':
                ret.value = new_wstring(L"#");
                ret.intern = op_fence;
                return ret;

            case L'$':
                ret.value = new_wstring(L"$");
                ret.intern = op_dollar;
                return ret;

            case L'[':
                ret.value = new_wstring(L"[");
                ret.intern = op_bracket_open;
                return ret;

            case L']':
                ret.value = new_wstring(L"]");
                ret.intern = op_bracket_close;
                return ret;
        }
    }

    while(iswalnum(* prog) || * prog == L'_'){
        * pwstr ++ = * prog ++;
    }

    * pwstr = 0;
    ret.value = new_wstring(wstr);

    /* Key words and identifiers */
    if((ret.intern = keyWord(wstr)) != -1){
        ret.type = tok_reserved;
    }
    else{
        ret.type = tok_identifier;
        ret.intern = list_search(identifiers, wstr, wcslen(wstr) + sizeof(wchar_t));
        if(ret.intern == -1){
            list_add(identifiers, new_wstring(wstr));
            ret.intern = list_search(identifiers, wstr, wcslen(wstr) + sizeof(wchar_t));
        }
        ret.intern += tok_identifier;
    }

    return ret;
}

INLINE void registerKeyWord(int index, wstring_t value){
    key_words[index] = value;
}

void initTokens(const string_t file, const wstring_t content){
    register int i, count;
    token_t tok;

    pfile = file;
    prog = content;
    getLine(prog);

    for(count = 0, tok = getToken(); tok.type != tok_end; count++){
        if(tok.value)
            free(tok.value);
        tok = getToken();
    }

    prog = content;
    getLine(prog);

    token = malloc(count * sizeof(token_t));
    assert(token);

    for(i = 0; i <= count; i++){
        token[i] = getToken();
    }
}
