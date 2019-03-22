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

static result_t current_return;

INLINE result_t getReturn(void){
    return current_return;
}

int executeCommand(pointer_t buf){
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
            ++ token;
            executeWhile(buf);
            break;
        case key_for:
            ++ token;
            executeFor(buf);
            break;
        case key_each:
            break;
        case key_using:
            break;
        case key_return:
            executeReturn(buf);
            return 0;
        case key_call:
            break;
        case key_try:
            break;
        case key_throw:
            break;
        #ifdef __TEST__
        case key_print:{
            result_t r;
            ++ token;
            r = expression(buf);

            switch(r.type){
                case type_character:
                    wprintf(L"%c", (character_t)r.value.getReal);
                    break;
                case type_real:
                    wprintf(L"%g", r.value.getReal);
                    break;
                case type_string:
                    wprintf(L"%s", r.value.getString);
                    break;
                default:
                    printError(syntax_error, *token, NULL);
            }
            free_result(r);
            expectedToken(tok_punctuation, punctuation(L';'), L";");
            ++ token;
            break;
        }
        #endif // __TEST__
        default:
            free_result(expression(buf));
    }

    return 1;
}

void executeBlock(pointer_t buf){
    register int count = 0;

    for(;;){
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
        else{
            break;
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
        if(cond.value.getBoolean){
            executeBlock(buf);

            if((++ token)->intern == key_else){
                findEndOfBlock();
            }
            else{
                -- token;
            }
        }
        else{
            findEndOfBlock();

            if((++ token)->intern == key_else){
                executeBlock(buf);
            }
            else{
                -- token;
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
    for(;;){
        executeBlock(buf);
        expectedToken(tok_reserved, key_while, __key_words[key_while]);
        cond = expression(buf);

        if(cond.type == type_boolean){
            if(cond.value.getBoolean){
                token = start;
            }
            else{
                expectedToken(tok_punctuation, punctuation(L';'), L";");
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

    for(;;){
        cond = expression(buf);

        if(cond.type == type_boolean){
            if(cond.value.getBoolean){
                executeBlock(buf);
                token = start;
            }
            else{
                findEndOfBlock();
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
    register int count = 1;
    register int count_var = countVariables();

    expectedToken(tok_punctuation, punctuation(L'('), L"(");
    executeCommand(buf);
    expectedToken(tok_punctuation, punctuation(L';'), L";");
    p1 = ++ token;

    for(;;){
        cond = expression(buf);
        expectedToken(tok_punctuation, punctuation(L';'), L";");
        p2 = ++ token;

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
            if(cond.value.getBoolean){
                executeBlock(buf);
                token = p2;
                free_result(expression(buf));
                token = p1;
            }
            else{
                findEndOfBlock();
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
    if((token + 1)->intern == punctuation(L':')){
        ++ token;
        current_return = expression(buf);
        expectedToken(tok_punctuation, punctuation(L';'), L";");
    }
}
