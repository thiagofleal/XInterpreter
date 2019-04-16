#include <stdlib.h>
#include "header.h"
#include "util/util.h"

extern void findEndOfBlock(void);
extern result_t getReturn(void);
extern void declareParameters(function_p);
extern void allocateParameters(function_p, result_t[]);
extern uint_t backupVariables(uint_t, variable_p);
extern void restaureVariables(variable_p, uint_t);
extern INLINE void setExec(boolean_t);
extern void freeVariableMemory(variable_p);

static uint_t count_functions;
function_t functions[num_functions];

void declareFunction(void){
    functions[count_functions].identifier = token->intern;
    expectedToken(tok_punctuation, punctuation(L'('), L"(");
    if((token + 1) -> type == tok_reserved){
        declareParameters(functions + count_functions);
    }
    expectedToken(tok_punctuation, punctuation(L')'), L")");
    functions[count_functions].enter = ++ token;
    ++ count_functions;
    findEndOfBlock();
}

boolean_t compatible_types(variable_p param, result_p arg){
    switch(param->type){
        case type_boolean:
            return arg->type == type_boolean ? True : False;
        case type_character:
        case type_integer:
        case type_real:
            return arg->type == type_character || arg->type == type_real ? True : False;
        case type_string:
            return arg->type == type_string ? True : False;
        case type_array:
            if(arg->type == type_array){
                return
                    ((array_p)param->value)->type == ((array_p)arg->value.getPointer)->type &&
                    ((array_p)param->value)->dimensions == ((array_p)arg->value.getPointer)->dimensions
                        ? True : False;
            }
            return arg->type == type_null ? True : False;
        case type_object:
            return arg->type == type_object || arg->type == type_null ? True : False;
        default:
            return False;
    }
}

function_p findFunction(uint_t identifier, int count_arguments){
    register int i;

    for(i = count_functions - 1; i >= 0; i--){
        if(functions[i].identifier == identifier){
            if(functions[i].count_params == count_arguments){
                return functions + i;
            }
            else if(functions[i].count_params >= default_arguments + count_arguments){
                return functions + i;
            }
        }
    }

    return NULL;
}

uint_t getArguments(result_t dest[], pointer_t buf){
    register uint_t count = 0;

    if((token + 1)->intern != punctuation(L')')){
        do{
            ++ token;
            dest[count ++] = expression(buf);
            ++ token;
        }
        while(token->intern == punctuation(L','));

        -- token;
    }

    return count;
}

void freeParameters(function_p function){
    register int i;
    for(i = 0; i < function->count_params; i++){
        freeVariableMemory(function->param + i);
    }
}

void executeFunction(function_p function, result_t args[], result_p ret, pointer_t buf){
    const uint_t count_vars = countVariables();
    const uint_t global = countGlobalVariables();
    const uint_t length = count_vars - global;
    const variable_p bk = malloc(length * sizeof(variable_t));
    const token_p bktoken = token;

    check(bk);

    backupVariables(global, bk);
    allocateParameters(function, args);
    token = function->enter;
    executeBlock(buf);
    * ret = getReturn();
    destroyVariables(count_vars + function->count_params);
    setCountVariables(global);
    freeParameters(function);
    restaureVariables(bk, length);
    free(bk);
    token = bktoken;
    setExec(True);
}

void callFunction(token_p identifier, result_p ret, pointer_t buf){
    result_t args[num_args];
    uint_t count_args = 0;
    function_p function = NULL;

    expectedToken(tok_punctuation, punctuation(L'('), L"(");
    count_args = getArguments(args, buf);
    function = findFunction(identifier->intern, count_args);
    expectedToken(tok_punctuation, punctuation(L')'), L")");

    if(function){
        executeFunction(function, args, ret, buf);
    }
    else{
        wchar_t str[50];
        swprintf(str, L"%s(<%d>)", identifier->value, count_args);
        printError(undeclared_function, *identifier, str);
    }
}
