#include <stdlib.h>
#include <setjmp.h>
#include "header.h"
#include "util/util.h"

extern INLINE result_t evaluateAssignment(pointer_t, type_value, result_t);

static uint_t count_var;
variable_t var[num_variables];

static const size_t size[] = {
    [type_boolean] = sizeof(boolean_t),
    [type_character] = sizeof(character_t),
    [type_integer] = sizeof(integer_t),
    [type_real] = sizeof(real_t),
    [type_string] = sizeof(wstring_t),
    [type_array] = sizeof(array_p),
    [type_object] = sizeof(object_p),
    [type_args] = sizeof(pointer_t)
};

static uint_t dimensions(void){
    register int count = 0;
    while((token + 1)->intern == op_bracket_open){
        ++ token;
        expectedToken(tok_operator, op_bracket_close, L"]");
        ++ count;
    }
    return count;
}

static void declare(variable_p var, type_value type, uint_t identifier, int count_dimensions){
    var->identifier = token->intern;
    if(count_dimensions){
        var->type = type_array;
        var->value = calloc(sizeof(array_t), 1);
        check(var->value);
        ((array_p)var->value)->dimensions = count_dimensions;
        ((array_p)var->value)->type = type;
        ((array_p)var->value)->size = size[type];
        ((array_p)var->value)->length = 0;
        ((array_p)var->value)->value = NULL;
    }
    else{
        var->type = type;
        var->value = calloc(size[type], 1);
        check(var->value);
    }
}

void declareVariable(pointer_t buf){
    uint_t type = token->intern - tok_reserved;

    if(type >= type_boolean && type <= type_object){
        int count_dimensions = dimensions(), identifier;
        expectedToken(tok_punctuation, punctuation(L':'), L":");
        ++ token;
        identifier = token->intern;

        do{
            declare(var + count_var, type, identifier, count_dimensions + dimensions());

            ++ token;
            ++ count_var;

            if(token->intern == op_assignment){
                ++ token;
                evaluateAssignment(var[count_var - 1].value, var[count_var - 1].type, expression(buf));
                ++ token;
            }
        }
        while(token->intern == punctuation(L','));

        -- token;
    }
}

variable_p findVariable(uint_t identifier){
    register int i;
    for(i = count_var - 1; i >= 0; i--){
        if(var[i].identifier == identifier){
            return var + i;
        }
    }
    return NULL;
}

INLINE uint_t countVariables(void){
    return count_var;
}

static void freeVariable(variable_p variable){
    free_value(variable->type, variable->value);
    free(variable->value);
}

void destroyVariables(uint_t since){
    while(count_var > since){
        -- count_var;
        freeVariable(var + count_var);
    }
}

uint_t backupVariables(uint_t begin, variable_p bk){
    register int i;
    register uint_t length = count_var - begin;

    for(i = 0; i < length; i++){
        bk[i] = var[i + length];
    }

    count_var = begin;
    return length;
}

void restaureVariables(variable_p bk, uint_t length){
    register int i;
    check(bk);

    for(i = 0; i < length; i++){
        var[i + length] = bk[i];
    }
}

void declareParameters(function_p pfunction){
    register int count_parameters = 0;

    do{
        uint_t type = token->intern - tok_reserved;
        if(type >= type_boolean && type <= type_object){
            int d = dimensions(), identifier;
            expectedToken(tok_punctuation, punctuation(L':'), L":");
            ++ token;
            identifier = token->intern;

            declare(pfunction->param + count_parameters, type, identifier, d);

            ++ token;
            ++ count_parameters;
        }
    }while(token->intern == punctuation(L','));

    pfunction->count_params = count_parameters;
    -- token;
}

void allocateParameters(function_p func, result_t arguments[]){
    register int i;

    for(i = 0; i < func->count_params; i++){
        var[count_var] = func->param[i];
        assign_pointer(arguments + i, var[count_var].value, var[count_var].type);
        ++ count_var;
    }
}
