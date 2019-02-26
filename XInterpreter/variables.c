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

void declareVariable(pointer_t buf){
    uint_t type = token->intern - tok_reserved;

    if(type >= type_boolean && type <= type_object){
        int d, count_dimensions = dimensions();
        expectedToken(tok_punctuation, punctuation(L':'), L":");
        ++ token;
        var[count_var].identifier = token->intern;

        do{
            d = count_dimensions + dimensions();

            if(d){
                var[count_var].type = type_array;
                var[count_var].value = calloc(sizeof(array_t), 1);
                check(var[count_var].value);
                ((array_p)var[count_var].value)->dimensions = d;
                ((array_p)var[count_var].value)->type = type;
                ((array_p)var[count_var].value)->size = size[type];
                ((array_p)var[count_var].value)->length = 0;
                ((array_p)var[count_var].value)->value = NULL;
            }
            else{
                var[count_var].type = type;
                var[count_var].value = calloc(size[type], 1);
                check(var[count_var].value);
            }

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

void destroyVariables(uint_t until){
    while(until < count_var){
        -- count_var;
        freeVariable(var + count_var);
    }
}
