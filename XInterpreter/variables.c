#include <stdlib.h>
#include <setjmp.h>
#include "header.h"
#include "util/util.h"

extern INLINE result_t evaluateAssignment(pointer_t, type_value, result_t);

static uint_t count_var;
variable_t var[num_variables];

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
        heap_p heap = malloc(sizeof(heap_t));
        var->type = type_array;
        var->value = calloc(sizeof(heap_p), 1);
        check(heap && var->value);

        heap->count = 0;
        heap->destroy = NULL;
        heap->memory = malloc(sizeof(array_t));
        check(heap->memory);

        ((array_p)heap->memory)->dimensions = count_dimensions;
        ((array_p)heap->memory)->type = type;
        ((array_p)heap->memory)->size = size_type[type];
        ((array_p)heap->memory)->length = 0;
        ((array_p)heap->memory)->value = NULL;

        *(heap_p*)var->value = heap;
    }
    else{
        var->type = type;
        var->value = calloc(size_type[type], 1);
        check(var->value);
    }
}

void declareVariable(pointer_t buf){
    uint_t type = token->intern - tok_reserved, dim;

    if(type >= type_boolean && type <= type_object){
        int count_dimensions = dimensions(), identifier;
        expectedToken(tok_punctuation, punctuation(L':'), L":");

        do{
            dim = dimensions();
            ++ token;
            identifier = token->intern;
            declare(var + count_var, type, identifier, count_dimensions + dim);

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

INLINE uint_t setCountVariables(uint_t value){
    if(value < count_var && value >= 0){
        count_var = value;
    }
    return count_var;
}

static void freeVariable(variable_p variable){
    switch(variable->type){
        case type_string:
            free(*(wstring_t*)variable->value);
            break;
        case type_array:
        case type_object:
            assign_heap_null((heap_p*)variable->value);
            break;
        default:
            break;
    }
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
    register const uint_t length = count_var - begin;

    for(i = length - 1; i >= 0; i--){
        bk[i] = var[-- count_var];
    }

    return length;
}

void restaureVariables(variable_p bk, uint_t length){
    register int i;
    check(bk);

    for(i = 0; i < length; i++){
        var[count_var ++] = bk[i];
    }
}

void declareParameters(function_p pfunction){
    register int count_parameters = 0;
    uint_t type;

    do{
        ++ token;
        type = token->intern - tok_reserved;
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
