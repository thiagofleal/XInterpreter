#include <math.h>
#include <string.h>
#include "header.h"
#include "util/util.h"

extern token_t *token;

static INLINE result_t evaluateAssignment(variable_t *var, result_t result){
    assign_pointer(result, var->value, var->type);
    return result;
}

static result_t evaluateNegativeAssignment(variable_t *var, int order){
    result_t result = {.type = type_boolean};
    if(var->type == type_boolean){
        if(order < 0){
            *(boolean_t*)var->value = *(boolean_t*)var->value ? False : True;
            result.value.getBoolean = *(boolean_t*)var->value;
        }
        else{
            result.value.getBoolean = *(boolean_t*)var->value;
            *(boolean_t*)var->value = *(boolean_t*)var->value ? False : True;
        }
    }
    return result;
}

static result_t evaluateIncrement(variable_t *var, int order){
    result_t result = {.type = type_real};
    if(var->type == type_integer){
        if(order < 0){
            *(real_t*)var->value = *(integer_t*)var->value + 1.0;
            result.value.getReal = *(integer_t*)var->value;
        }
        else{
            result.value.getReal = *(integer_t*)var->value;
            *(real_t*)var->value = *(integer_t*)var->value + 1.0;
        }
    }
    if(var->type == type_real){
        if(order < 0){
            *(real_t*)var->value = *(real_t*)var->value + 1.0;
            result.value.getReal = *(real_t*)var->value;
        }
        else{
            result.value.getReal = *(real_t*)var->value;
            *(real_t*)var->value = *(real_t*)var->value + 1.0;
        }
    }
    return result;
}

static result_t evaluateDecrement(variable_t *var, int order){
    result_t result = {.type = type_real};
    if(var->type == type_integer){
        if(order < 0){
            *(real_t*)var->value = *(integer_t*)var->value - 1.0;
            result.value.getReal = *(integer_t*)var->value;
        }
        else{
            result.value.getReal = *(integer_t*)var->value;
            *(real_t*)var->value = *(integer_t*)var->value - 1.0;
        }
    }
    if(var->type == type_real){
        if(order < 0){
            *(real_t*)var->value = *(real_t*)var->value - 1.0;
            result.value.getReal = *(real_t*)var->value;
        }
        else{
            result.value.getReal = *(real_t*)var->value;
            *(real_t*)var->value = *(real_t*)var->value - 1.0;
        }
    }
    return result;
}

static result_t evaluateAdditionAssignment(variable_t *var, result_t result){
    if(var->type == type_integer){
        result.value.getReal = *(integer_t*)var->value + result.value.getReal;
    }
    if(var->type == type_real){
        result.value.getReal = *(real_t*)var->value + result.value.getReal;
    }
    assign_pointer(result, var->value, var->type);
    return result;
}

static result_t evaluateSubtractionAssignment(variable_t *var, result_t result){
    if(var->type == type_integer){
        result.value.getReal = *(integer_t*)var->value - result.value.getReal;
    }
    if(var->type == type_real){
        result.value.getReal = *(real_t*)var->value - result.value.getReal;
    }
    assign_pointer(result, var->value, var->type);
    return result;
}

static result_t evaluateMultiplyAssignment(variable_t *var, result_t result){
    if(var->type == type_integer){
        result.value.getReal = *(integer_t*)var->value * result.value.getReal;
    }
    if(var->type == type_real){
        result.value.getReal = *(real_t*)var->value * result.value.getReal;
    }
    assign_pointer(result, var->value, var->type);
    return result;
}

static result_t evaluateDivisionAssignment(variable_t *var, result_t result){
    if(var->type == type_integer){
        result.value.getReal = *(integer_t*)var->value / result.value.getReal;
    }
    if(var->type == type_real){
        result.value.getReal = *(real_t*)var->value / result.value.getReal;
    }
    assign_pointer(result, var->value, var->type);
    return result;
}

static result_t evaluateModuleAssignment(variable_t *var, result_t result){
    if(var->type == type_integer){
        result.value.getReal = *(integer_t*)var->value % (int)result.value.getReal;
    }
    if(var->type == type_real){
        result.value.getReal = ((int)*(real_t*)var->value) % (int)result.value.getReal;
    }
    assign_pointer(result, var->value, var->type);
    return result;
}

static result_t evaluatePowAssignment(variable_t *var, result_t result){
    if(var->type == type_integer){
        result.value.getReal = pow((double)*(integer_t*)var->value, result.value.getReal);
    }
    if(var->type == type_real){
        result.value.getReal = pow(*(real_t*)var->value, result.value.getReal);
    }
    assign_pointer(result, var->value, var->type);
    return result;
}

static result_t evaluateRadixAssignment(variable_t *var, result_t result){
    if(var->type == type_integer){
        result.value.getReal = pow((double)*(integer_t*)var->value, 1.0 / result.value.getReal);
    }
    if(var->type == type_real){
        result.value.getReal = pow(*(real_t*)var->value, 1.0 / result.value.getReal);
    }
    assign_pointer(result, var->value, var->type);
    return result;
}

static result_t evaluateAt(result_t left, result_t right){
    result_t result;
    return result;
}

static result_t evaluateFence(result_t left, result_t right){
    result_t result;
    return result;
}

static result_t evaluateDollar(result_t left, result_t right){
    result_t result;
    return result;
}

static result_t evaluateAmpersand(result_t left, result_t right){
    result_t result = {.type = type_real};
    if(left.type == right.type && left.type == type_real){
        result.value.getReal = (real_t)((int)left.value.getReal & (int)right.value.getReal);
    }
    return result;
}

static result_t evaluatePipe(result_t left, result_t right){
    result_t result = {.type = type_real};
    if(left.type == right.type && left.type == type_real){
        result.value.getReal = (real_t)((int)left.value.getReal | (int)right.value.getReal);
    }
    return result;
}

static result_t evaluateQuery(result_t left, result_t right){
}

static result_t evaluateDoubleQuery(result_t left, result_t right){
}

static result_t evaluateDoubleLeft(result_t left, result_t right){
}

static result_t evaluateDoubleRight(result_t left, result_t right){
}

static result_t evaluateNot(result_t left, result_t right){
}

static result_t evaluateAnd(result_t left, result_t right){
    result_t ret = {.type = type_boolean, .value.getBoolean = False};
    if(left.type == right.type && left.type == type_boolean){
        boolean_t lvalue = left.value.getBoolean;
        boolean_t rvalue = right.value.getBoolean;
        ret.value.getBoolean = lvalue && rvalue ? True : False;
    }
    return ret;
}

static result_t evaluateOr(result_t left, result_t right){
    result_t ret = {.type = type_boolean, .value.getBoolean = False};
    if(left.type == right.type && left.type == type_boolean){
        boolean_t lvalue = left.value.getBoolean;
        boolean_t rvalue = right.value.getBoolean;
        ret.value.getBoolean = lvalue || rvalue ? True : False;
    }
    return ret;
}

static result_t evaluateXor(result_t left, result_t right){
    result_t ret = {.type = type_boolean, .value.getBoolean = False};
    if(left.type == right.type && left.type == type_boolean){
        boolean_t lvalue = left.value.getBoolean;
        boolean_t rvalue = right.value.getBoolean;
        ret.value.getBoolean =  (lvalue || rvalue) && !(lvalue && rvalue) ? True : False;
    }
    return ret;
}

static result_t evaluateIfThen(result_t left, result_t right){
    result_t ret = {.type = type_boolean, .value.getBoolean = False};
    if(left.type == right.type && left.type == type_boolean){
        boolean_t lvalue = left.value.getBoolean;
        boolean_t rvalue = right.value.getBoolean;
        ret.value.getBoolean = lvalue && !rvalue ? False : True;
    }
    return ret;
}

static result_t evaluateOnlyIf(result_t left, result_t right){
    result_t ret = {.type = type_boolean, .value.getBoolean = False};
    if(left.type == right.type && left.type == type_boolean){
        boolean_t lvalue = left.value.getBoolean;
        boolean_t rvalue = right.value.getBoolean;
        ret.value.getBoolean = lvalue == rvalue ? True : False;
    }
    return ret;
}

static result_t evaluateEqual(result_t left, result_t right){
    result_t ret = {.type = type_boolean, .value.getBoolean = False};
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
            case type_array:
                ret.value.getBoolean = left.value.getPointer == right.value.getPointer ? True : False;
                break;
            case type_object:
                ret.value.getBoolean = left.value.getPointer == right.value.getPointer ? True : False;
                break;
        }
    }
    return ret;
}

static result_t evaluateDifferent(result_t left, result_t right){
    result_t ret = {.type = type_boolean, .value.getBoolean = False};
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
            case type_array:
                ret.value.getBoolean = left.value.getPointer != right.value.getPointer ? True : False;
                break;
            case type_object:
                ret.value.getBoolean = left.value.getPointer != right.value.getPointer ? True : False;
                break;
        }
    }
    return ret;
}

static result_t evaluateNear(result_t left, result_t right){
    result_t ret = {.type = type_boolean, .value.getBoolean = False};
    if(left.type == type_string && right.type == type_string){
        ret.value.getBoolean =
            wcscmp(left.value.getString, right.value.getString) &&
            wstring_near(left.value.getString, right.value.getString)
                ? True
                : False;
    }
    return ret;
}

static result_t evaluateNotNear(result_t left, result_t right){
    result_t ret = {.type = type_boolean, .value.getBoolean = False};
    if(left.type == type_string && right.type == type_string){
        ret.value.getBoolean =
            wstring_near(left.value.getString, right.value.getString)
                ? False
                : True;
    }
    return ret;
}

static result_t evaluateNearOrIdentical(result_t left, result_t right){
    result_t ret = {.type = type_boolean, .value.getBoolean = False};
    if(left.type == type_string && right.type == type_string){
        ret.value.getBoolean =
            wstring_near(left.value.getString, right.value.getString)
                ? True
                : False;
    }
    return ret;
}

static result_t evaluateLarger(result_t left, result_t right){
    result_t ret = {.type = type_boolean, .value.getBoolean = False};
    if(left.type == type_string && right.type == type_string){
        ret.value.getBoolean =
            left.value.getReal > right.value.getReal
                ? True
                : False;
    }
    return ret;
}

static result_t evaluateLargerOrEqual(result_t left, result_t right){
    result_t ret = {.type = type_boolean, .value.getBoolean = False};
    if(left.type == type_string && right.type == type_string){
        ret.value.getBoolean =
            left.value.getReal >= right.value.getReal
                ? True
                : False;
    }
    return ret;
}

static result_t evaluateLess(result_t left, result_t right){
    result_t ret = {.type = type_boolean, .value.getBoolean = False};
    if(left.type == type_string && right.type == type_string){
        ret.value.getBoolean =
            left.value.getReal < right.value.getReal
                ? True
                : False;
    }
    return ret;
}

static result_t evaluateLessOrEqual(result_t left, result_t right){
    result_t ret = {.type = type_boolean, .value.getBoolean = False};
    if(left.type == type_string && right.type == type_string){
        ret.value.getBoolean =
            left.value.getReal <= right.value.getReal
                ? True
                : False;
    }
    return ret;
}

static result_t evaluateAddition(result_t left, result_t right){
}

static result_t evaluateSubtraction(result_t left, result_t right){
}

static result_t evaluateMultiply(result_t left, result_t right){
}

static result_t evaluateDivision(result_t left, result_t right){
}

static result_t evaluateModule(result_t left, result_t right){
}

static result_t evaluatePow(result_t left, result_t right){
}

static result_t evaluateRadix(result_t left, result_t right){
}

static result_t evaluateDot(result_t left, token_t right){
}

static result_t evaluateQueryDot(result_t left, token_t right){
}

static result_t evaluateBrackets(result_t stream, result_t element){
    result_t ret = {};
    if(stream.type == type_array && element.type == type_real){
        array_t *array = stream.value.getPointer;
        int type = array->dimensions - 1 ? type_array : array->type;
        assign_result(
            array->value.memory +
                array->size * (size_t)element.value.getReal,
            &ret,
            type
        );
    }
    return ret;
}


result_t expression(void){
    result_t result;
    return result;
}
