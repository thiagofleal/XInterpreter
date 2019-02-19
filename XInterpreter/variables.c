#include <stdlib.h>
#include "header.h"

uint_t count_var;

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

void declareVariable(void){

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

    uint_t type = token->intern - tok_reserved;

    if(type >= type_boolean && type <= type_object){
        int d, dim = dimensions();
        expectedToken(tok_punctuation, L':' + tok_punctuation, L":");
        ++ token;
        var[count_var].identifier = token->intern;

        do{
            d = dim + dimensions();

            if(d){
                // Array
            }
            else{
                var[count_var].type = type;
                var[count_var].value = calloc(size[type], 1);
            }

            ++ token;
            ++ count_var;
        }
        while(token->intern == L',' + tok_punctuation);

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
