#include <stdlib.h>
#include "header.h"
#include "util/util.h"

extern boolean_t findEndOfBlock(void);
extern INLINE result_t getReturn(void);
extern void declareParameters(function_p);
extern void allocateParameters(function_p, result_t[], uint_t);
extern uint_t backupVariables(uint_t, variable_p);
extern void restaureVariables(variable_p, uint_t);
extern INLINE void setExec(boolean_t);
extern void freeVariableMemory(variable_p);

static uint_t count_functions;
function_t functions[num_functions];

void initializeFunction(function_p func){
    func->identifier = token->intern;
    expectedToken(tok_punctuation, punctuation(L'('), L"(");
    if((token + 1) -> type == tok_reserved){
        declareParameters(func);
    }
    expectedToken(tok_punctuation, punctuation(L')'), L")");
    func->enter = ++ token;
    if(!findEndOfBlock()){
        printError(non_terminated_block, *token, NULL);
    }
}

INLINE void declareFunction(void){
    initializeFunction(functions + count_functions);
    ++ count_functions;
}

function_p findFunction(uint_t identifier, uint_t count_arguments){
    register int i;

    for(i = count_functions - 1; i >= 0; i--){
        if(functions[i].identifier == identifier){
            if(functions[i].count_params == count_arguments){
                return functions + i;
            }
            else if(functions[i].count_params >= default_arguments){
                if(count_arguments >= functions[i].count_params - default_arguments){
                    return functions + i;
                }
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

void executeFunction(function_p function, result_t args[], uint_t count_args, result_p ret, pointer_t buf){
    const uint_t count_vars = countVariables();
    const uint_t global = countGlobalVariables();
    const uint_t length = count_vars - global;
    const variable_p bk = malloc(length * sizeof(variable_t));
    const token_p bktoken = token;

    check(bk);

    backupVariables(global, bk);
    allocateParameters(function, args, count_args);
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

int callFunction(uint_t identifier, result_p ret, pointer_t buf){
    result_t args[num_args];
    uint_t count_args = 0;
    function_p function = NULL;

    expectedToken(tok_punctuation, punctuation(L'('), L"(");
    count_args = getArguments(args, buf);
    function = findFunction(identifier, count_args);
    expectedToken(tok_punctuation, punctuation(L')'), L")");

    if(function){
        executeFunction(function, args, count_args, ret, buf);
        return 1;
    }
    return -count_args;
}
