#include <stdlib.h>
#include <setjmp.h>
#include "header.h"
#include "util/util.h"

extern wstring_t __key_words[];

static void executeIf(pointer_t);
static void executeDoWhile(pointer_t);
static void executeWhile(pointer_t);
static void executeFor(pointer_t);
static void executeReturn(pointer_t);

static result_t current_return = {};
static boolean_t exec = True;

INLINE result_t getReturn(void){
    return current_return;
}

INLINE void setExec(boolean_t value){
    exec = value;
}

int swprintr(result_t r, wstring_t str){
    switch(r.type){
        case type_boolean:
            swprintf(str, L"%s", r.value.getBoolean ? L"true" : L"false");
            break;
        case type_character:
            swprintf(str, L"%c", (character_t)r.value.getReal);
            break;
        case type_real:
            swprintf(str, L"%g", r.value.getReal);
            break;
        case type_string:
            swprintf(str, L"%s", r.value.getString);
            break;
        default:
            swprintf(str, L"");
            return 0;
    }
    return 1;
}

int executeCommand(pointer_t buf){
    if(!exec){
        return 0;
    }
    switch(token->intern){
        case key_boolean:
        case key_character:
        case key_integer:
        case key_real:
        case key_string:
        case key_object:
        case key_args:
            declareVariable(buf);
            expectedToken(tok_punctuation, punctuation(L';'), L";");
            ++ token;
            break;
        case key_if:
            ++ token;
            executeIf(buf);
            break;
        case key_do:
            ++ token;
            executeDoWhile(buf);
            break;
        case key_while:
            executeWhile(buf);
            break;
        case key_for:
            executeFor(buf);
            break;
        case key_each:
            break;
        case key_using:
            break;
        case key_return:
            //++ token;
            executeReturn(buf);
            return 0;
        case key_call:
            break;
        case key_try:
            break;
        case key_throw:
            break;
        #ifdef __TEST__
        case key_print:
            do{
                result_t r;
                wchar_t str[100];

                ++ token;
                r = expression(buf);
                swprintr(r, str);
                wprintf(str);

                free_result(r);
            }
            while((++ token) -> intern == punctuation(L','));

            -- token;
            expectedToken(tok_punctuation, punctuation(L';'), L";");
            ++ token;
            break;
        #endif // __TEST__
        default:
            free_result(expression(buf));
            expectedToken(tok_punctuation, punctuation(L';'), L";");
            ++ token;
    }

    return 1;
}

void executeBlock(pointer_t buf){
    register int count = 0;

    while(exec){
        if(token->intern == punctuation(L'{')){
            ++ count;
            ++ token;
        }
        if(token->intern == punctuation(L'}')){
            -- count;
            ++ token;
        }

        if(count){
            count *= executeCommand(buf);
        }
        if(!count){
            return;
        }
    }
}

void findEndOfBlock(void){
    if(token->intern == punctuation(L'{')){
        register int count = 1;
        do{
            ++ token;
            if(token->intern == punctuation('{')){
                ++ count;
            }
            if(token->intern == punctuation('}')){
                -- count;
            }
        }
        while(count);
    }
}

static void executeIf(pointer_t buf){
    result_t cond = expression(buf);
    if(cond.type == type_boolean){
        ++ token;
        if(cond.value.getBoolean){
            executeBlock(buf);

            if((++ token)->intern == key_else){
                ++ token;
                findEndOfBlock();
            }
            else{
                -- token;
            }
        }
        else{
            findEndOfBlock();

            if((++ token)->intern == key_else){
                ++ token;
                executeBlock(buf);
            }
        }
    }
    else{
        printError(not_boolean_expression, *token, NULL);
    }
}

static void executeDoWhile(pointer_t buf){
    const token_p start = token;
    result_t cond;
    while(exec){
        executeBlock(buf);
        -- token;
        expectedToken(tok_reserved, key_while, __key_words[key_while]);
        ++ token;
        cond = expression(buf);

        if(cond.type == type_boolean){
            if(cond.value.getBoolean){
                token = start;
            }
            else{
                expectedToken(tok_punctuation, punctuation(L';'), L";");
                ++ token;
                break;
            }
        }
        else{
            printError(not_boolean_expression, *token, NULL);
        }
    }
}

static void executeWhile(pointer_t buf){
    const token_p start = token;
    result_t cond;

    while(exec){
        ++ token;
        cond = expression(buf);

        if(cond.type == type_boolean){
            ++ token;
            if(cond.value.getBoolean){
                executeBlock(buf);
                token = start;
            }
            else{
                findEndOfBlock();
                return;
            }
        }
        else{
            printError(not_boolean_expression, *token, NULL);
        }
    }
}

static void executeFor(pointer_t buf){
    token_p p1, p2;
    result_t cond;
    register int count;
    register int count_var = countVariables();

    expectedToken(tok_punctuation, punctuation(L'('), L"(");
    if((++ token)->type == tok_reserved){
        declareVariable(buf);
    }
    else{
        free_result(expression(buf));
    }
    expectedToken(tok_punctuation, punctuation(L';'), L";");
    p1 = ++ token;

    while(exec){
        cond = expression(buf);
        expectedToken(tok_punctuation, punctuation(L';'), L";");
        p2 = ++ token;
        count = 1;

        while(count){
            ++ token;
            if(token->intern == punctuation(L'(')){
                ++ count;
            }
            if(token->intern == punctuation(L')')){
                -- count;
            }
        }

        if(cond.type == type_boolean){
            ++ token;
            if(cond.value.getBoolean){
                executeBlock(buf);
                token = p2;
                free_result(expression(buf));
                token = p1;
            }
            else{
                findEndOfBlock();
                ++ token;
                break;
            }
        }
        else{
            printError(not_boolean_expression, *token, NULL);
        }
    }

    destroyVariables(count_var);
}

static void executeReturn(pointer_t buf){
    exec = False;
    if((token + 1)->intern == punctuation(L':')){
        ++ token; ++ token;
        current_return = expression(buf);
    }
    expectedToken(tok_punctuation, punctuation(L';'), L";");
}
