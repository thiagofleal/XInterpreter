#include <math.h>
#include <string.h>
#include "header.h"
#include "util/util.h"

extern token_t *token;

static result_t evaluateLogic(result_t left, result_t right, int op){
    result_t ret = {.type = type_boolean, .value.getBoolean = False};
    switch(op){
        case op_and:
            if(left.type == right.type && left.type == type_boolean){
                boolean_t lvalue = left.value.getBoolean;
                boolean_t rvalue = right.value.getBoolean;
                ret.value.getBoolean = lvalue && rvalue ? True : False;
            }
            break;
        case op_or:
            if(left.type == right.type && left.type == type_boolean){
                boolean_t lvalue = left.value.getBoolean;
                boolean_t rvalue = right.value.getBoolean;
                ret.value.getBoolean = lvalue || rvalue ? True : False;
            }
            break;
        case op_xor:
            if(left.type == right.type && left.type == type_boolean){
                boolean_t lvalue = left.value.getBoolean;
                boolean_t rvalue = right.value.getBoolean;
                ret.value.getBoolean =  (lvalue || rvalue) && !(lvalue && rvalue) ? True : False;
            }
            break;
        case op_if_then:
            if(left.type == right.type && left.type == type_boolean){
                boolean_t lvalue = left.value.getBoolean;
                boolean_t rvalue = right.value.getBoolean;
                ret.value.getBoolean = lvalue && !rvalue ? False : True;
            }
            break;
        case op_only_if:
            if(left.type == right.type && left.type == type_boolean){
                boolean_t lvalue = left.value.getBoolean;
                boolean_t rvalue = right.value.getBoolean;
                ret.value.getBoolean = lvalue == rvalue ? True : False;
            }
            break;
    }
    return ret;
}

static result_t evaluateRelational(result_t left, result_t right, int op){
    result_t ret = {.type = type_boolean, .value.getBoolean = False};
    switch(op){
        case op_equal:
            if(left.type != right.type){
                ret.value.getBoolean = False;
            }
            else{
                switch(left.type){
                    case type_boolean:
                        ret.value.getBoolean = left.value.getBoolean == right.value.getBoolean ? True : False;
                        break;
                    case type_character:
                        ret.value.getBoolean = left.value.getCharacter == right.value.getCharacter ? True : False;
                        break;
                    case type_real:
                        ret.value.getBoolean = left.value.getReal == right.value.getReal ? True : False;
                        break;
                    case type_string:
                        ret.value.getBoolean = wcscmp(left.value.getString, right.value.getString) ? False : True;
                        break;
                    case type_file:
                    case type_array:
                        ret.value.getBoolean = left.value.getPointer == right.value.getPointer ? True : False;
                        break;
                    case type_object:
                        ret.value.getBoolean = left.value.getPointer == right.value.getPointer ? True : False;
                        break;
                }
            }
            break;
        case op_different:
            if(left.type != right.type){
                ret.value.getBoolean = True;
            }
            else{
                switch(left.type){
                    case type_boolean:
                        ret.value.getBoolean = left.value.getBoolean != right.value.getBoolean ? True : False;
                        break;
                    case type_character:
                        ret.value.getBoolean = left.value.getCharacter != right.value.getCharacter ? True : False;
                        break;
                    case type_real:
                        ret.value.getBoolean = left.value.getReal != right.value.getReal ? True : False;
                        break;
                    case type_string:
                        ret.value.getBoolean = wcscmp(left.value.getString, right.value.getString) ? True : False;
                        break;
                    case type_file:
                    case type_array:
                        ret.value.getBoolean = left.value.getPointer != right.value.getPointer ? True : False;
                        break;
                    case type_object:
                        ret.value.getBoolean = left.value.getPointer != right.value.getPointer ? True : False;
                        break;
                }
            }
            break;
        case op_near:
            if(left.type != right.type){
                ret.value.getBoolean = False;
            }
            else{
                if(left.type == type_string){
                    ret.value.getBoolean =
                        wcscmp(left.value.getString, right.value.getString) &&
                        wstring_near(left.value.getString, right.value.getString)
                            ? True
                            : False;
                }
            }
            break;
        case op_not_near:
            if(left.type != right.type){
                ret.value.getBoolean = False;
            }
            else{
                if(left.type == type_string){
                    ret.value.getBoolean =
                        wstring_near(left.value.getString, right.value.getString)
                            ? False
                            : True;
                }
            }
            break;
        case op_near_or_identical:
            if(left.type != right.type){
                ret.value.getBoolean = False;
            }
            else{
                if(left.type == type_string){
                    ret.value.getBoolean =
                        wstring_near(left.value.getString, right.value.getString)
                            ? True
                            : False;
                }
            }
            break;
        case op_larger:
            if(left.type != right.type){
                ret.value.getBoolean = False;
            }
            else{
                if(left.type == type_real){
                    ret.value.getBoolean =
                        left.value.getReal > right.value.getReal
                            ? True
                            : False;
                }
            }
            break;
        case op_larger_or_equal:
            if(left.type != right.type){
                ret.value.getBoolean = False;
            }
            else{
                if(left.type == type_real){
                    ret.value.getBoolean =
                        left.value.getReal >= right.value.getReal
                            ? True
                            : False;
                }
            }
            break;
        case op_less:
            if(left.type != right.type){
                ret.value.getBoolean = False;
            }
            else{
                if(left.type == type_real){
                    ret.value.getBoolean =
                        left.value.getReal < right.value.getReal
                            ? True
                            : False;
                }
            }
            break;
        case op_less_or_equal:
            if(left.type != right.type){
                ret.value.getBoolean = False;
            }
            else{
                if(left.type == type_real){
                    ret.value.getBoolean =
                        left.value.getReal <= right.value.getReal
                            ? True
                            : False;
                }
            }
            break;
    }
    return ret;
}

result_t expression(void){
    result_t result;
    return result;
}
