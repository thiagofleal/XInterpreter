#include "header.h"
#include "util/util.h"

extern void findEndOfBlock(void);
extern void declareParameters(function_p);
extern void allocateParameters(function_p, result_t[]);

static int count_functions;
function_t functions[num_functions];

void declareFunction(void){
    functions[count_functions].identifier = token->intern;
    expectedToken(tok_punctuation, punctuation(L'('), L"(");
    declareParameters(functions + count_functions);
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

    for(i = count_arguments - 1; i >= 0; i--){
        if(functions[i].identifier == identifier){
            if(functions[i].count_params - count_arguments >= default_arguments){
                return functions + i;
            }
            else if(functions[i].count_params == count_arguments){
                return functions + i;
            }
        }
    }

    return NULL;
}

uint_t getArguments(result_t dest[], pointer_t buf){
    register uint_t count = 0;

    do{
        dest[count++] = expression(buf);
        ++ token;
    }
    while(token->intern == punctuation(L','));

    -- token;
    return count;
}
