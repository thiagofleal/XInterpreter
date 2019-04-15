#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <setjmp.h>
#include "header.h"
#include "util/util.h"

static pointer_t __buf;

INLINE result_t evaluateAssignment(pointer_t value, type_value type, result_t result){
    assign_pointer(&result, value, type);
    return result;
}

static result_t evaluateNegativeAssignment(pointer_t value, type_value type, int order){
    result_t result = {.type = type_boolean};
    if(type == type_boolean){
        if(order < 0){
            *(boolean_t*)value = *(boolean_t*)value ? False : True;
            result.value.getBoolean = *(boolean_t*)value;
        }
        else{
            result.value.getBoolean = *(boolean_t*)value;
            *(boolean_t*)value = *(boolean_t*)value ? False : True;
        }
    }
    return result;
}

static result_t evaluateIncrement(pointer_t value, type_value type, int order){
    result_t result = {.type = type_real};
    if(type == type_character){
        if(order < 0){
            *(character_t*)value = *(character_t*)value + 1.0;
            result.value.getReal = *(character_t*)value;
        }
        else{
            result.value.getReal = *(character_t*)value;
            *(character_t*)value = *(character_t*)value + 1.0;
        }
    }
    if(type == type_integer){
        if(order < 0){
            *(integer_t*)value = *(integer_t*)value + 1.0;
            result.value.getReal = *(integer_t*)value;
        }
        else{
            result.value.getReal = *(integer_t*)value;
            *(integer_t*)value = *(integer_t*)value + 1.0;
        }
    }
    if(type == type_real){
        if(order < 0){
            *(real_t*)value = *(real_t*)value + 1.0;
            result.value.getReal = *(real_t*)value;
        }
        else{
            result.value.getReal = *(real_t*)value;
            *(real_t*)value = *(real_t*)value + 1.0;
        }
    }
    return result;
}

static result_t evaluateDecrement(pointer_t value, type_value type, int order){
    result_t result = {.type = type_real};
    if(type == type_character){
        if(order < 0){
            *(character_t*)value = *(character_t*)value - 1.0;
            result.value.getReal = *(character_t*)value;
        }
        else{
            result.value.getReal = *(character_t*)value;
            *(character_t*)value = *(character_t*)value - 1.0;
        }
    }
    if(type == type_integer){
        if(order < 0){
            *(integer_t*)value = *(integer_t*)value - 1.0;
            result.value.getReal = *(integer_t*)value;
        }
        else{
            result.value.getReal = *(integer_t*)value;
            *(integer_t*)value = *(integer_t*)value - 1.0;
        }
    }
    if(type == type_real){
        if(order < 0){
            *(real_t*)value = *(real_t*)value - 1.0;
            result.value.getReal = *(real_t*)value;
        }
        else{
            result.value.getReal = *(real_t*)value;
            *(real_t*)value = *(real_t*)value - 1.0;
        }
    }
    return result;
}

static result_t evaluateAdditionAssignment(pointer_t value, type_value type, result_t result){
    if(type == type_integer){
        result.value.getReal = *(integer_t*)value + result.value.getReal;
    }
    if(type == type_real){
        result.value.getReal = *(real_t*)value + result.value.getReal;
    }
    assign_pointer(&result, value, type);
    return result;
}

static result_t evaluateSubtractionAssignment(pointer_t value, type_value type, result_t result){
    if(type == type_integer){
        result.value.getReal = *(integer_t*)value - result.value.getReal;
    }
    if(type == type_real){
        result.value.getReal = *(real_t*)value - result.value.getReal;
    }
    assign_pointer(&result, value, type);
    return result;
}

static result_t evaluateMultiplyAssignment(pointer_t value, type_value type, result_t result){
    if(type == type_integer){
        result.value.getReal = *(integer_t*)value * result.value.getReal;
    }
    if(type == type_real){
        result.value.getReal = *(real_t*)value * result.value.getReal;
    }
    assign_pointer(&result, value, type);
    return result;
}

static result_t evaluateDivisionAssignment(pointer_t value, type_value type, result_t result){
    if(type == type_integer){
        result.value.getReal = *(integer_t*)value / result.value.getReal;
    }
    if(type == type_real){
        result.value.getReal = *(real_t*)value / result.value.getReal;
    }
    assign_pointer(&result, value, type);
    return result;
}

static result_t evaluateModuleAssignment(pointer_t value, type_value type, result_t result){
    if(type == type_integer){
        result.value.getReal = *(integer_t*)value % (int)result.value.getReal;
    }
    if(type == type_real){
        result.value.getReal = ((int)*(real_t*)value) % (int)result.value.getReal;
    }
    assign_pointer(&result, value, type);
    return result;
}

static result_t evaluatePowAssignment(pointer_t value, type_value type, result_t result){
    if(type == type_integer){
        result.value.getReal = pow((double)*(integer_t*)value, result.value.getReal);
    }
    if(type == type_real){
        result.value.getReal = pow(*(real_t*)value, result.value.getReal);
    }
    assign_pointer(&result, value, type);
    return result;
}

static result_t evaluateRadixAssignment(pointer_t value, type_value type, result_t result){
    if(type == type_integer){
        result.value.getReal = pow((double)*(integer_t*)value, 1.0 / result.value.getReal);
    }
    if(type == type_real){
        result.value.getReal = pow(*(real_t*)value, 1.0 / result.value.getReal);
    }
    assign_pointer(&result, value, type);
    return result;
}

static result_t evaluateAt(result_t result){
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

static result_t evaluateTilde(result_t result){
    if(result.type == type_real){
        result.value.getReal = (real_t)( ~ (int)result.value.getReal);
    }
    return result;
}

static result_t evaluateDoubleQuery(result_t left, result_t right){
    if(left.type == type_array || left.type == type_object){
        return left.value.getPointer ? left : right;
    }
    if(left.type == type_boolean){
        return left.value.getBoolean ? left : right;
    }
    if(left.type == type_real){
        return left.value.getReal ? left : right;
    }
    if(left.type == type_string){
        return * left.value.getString ? left : right;
    }
    return left;
}

static result_t evaluateDoubleLeft(result_t left, result_t right){
    result_t result;
    if(left.type == type_real && right.type == type_real){
        result.value.getReal = (real_t)((int)left.value.getReal << (int)right.value.getReal);
    }
    if(left.type == type_string){
        static wstring_t wstr1 = NULL;
        static wstring_t wstr2 = NULL;

        size_t size1 = 0;
        size_t size2 = 0;

        if(wstr1){ free(wstr1); }
        if(wstr2){ free(wstr2); }

        size1 = wcslen(left.value.getString) + 1;

        if(right.type == type_string){
            size2 = wcslen(right.value.getString) + 1;
        }
        else{
            size2 = 100;
        }

        size1 *= sizeof(wchar_t);
        size2 *= sizeof(wchar_t);

        wstr1 = malloc(size1);
        wstr2 = malloc(size2);
        check(wstr1 && wstr2);

        size1 = swprintr(left, wstr1) * sizeof(wchar_t);
        size2 = swprintr(right, wstr2) * sizeof(wchar_t);

        result.value.getString = malloc(size1 + size2 + 1);
        check(result.value.getString);

        swprintf(result.value.getString, L"%s%s", wstr1, wstr2);
        result.type = type_string;
    }
    return result;
}

static result_t evaluateDoubleRight(result_t left, result_t right){
    result_t result;
    if(left.type == type_real && right.type == type_real){
        result.value.getReal = (real_t)((int)left.value.getReal >> (int)right.value.getReal);
    }
    if(left.type == type_string){
        // Future implementation
    }
    return result;
}

static result_t evaluateNot(result_t result){
    if(result.type == type_boolean){
        result.value.getBoolean = result.value.getBoolean ? False : True;
    }
    return result;
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
        if(left.type == type_array && right.type == type_null){
            ret.value.getBoolean = ((array_p)left.value.getHeap->memory)->value ? False : True;
        }
        else if(left.type == type_object && right.type == type_null){
            ret.value.getBoolean = left.value.getHeap->memory ? False : True;
        }
        else if(left.type == type_null && right.type == type_array){
            ret.value.getBoolean = ((array_p)right.value.getHeap->memory)->value ? False : True;
        }
        else if(left.type == type_null && right.type == type_object){
            ret.value.getBoolean = right.value.getHeap->memory ? False : True;
        }
        else ret.value.getBoolean = False;
    }
    else{
        switch(left.type){
            case type_boolean:
                ret.value.getBoolean = left.value.getBoolean == right.value.getBoolean ? True : False;
                break;
            case type_character:
            case type_real:
                ret.value.getBoolean = left.value.getReal == right.value.getReal ? True : False;
                break;
            case type_string:
                ret.value.getBoolean = wcscmp(left.value.getString, right.value.getString) ? False : True;
                break;
            case type_array:
                ret.value.getBoolean = left.value.getHeap == right.value.getHeap ? True : False;
                break;
            case type_object:
                ret.value.getBoolean = left.value.getHeap == right.value.getHeap ? True : False;
                break;
            default:
                break;
        }
    }
    return ret;
}

static result_t evaluateDifferent(result_t left, result_t right){
    result_t ret = {.type = type_boolean, .value.getBoolean = False};
    if(left.type != right.type){
        if(left.type == type_array && right.type == type_null){
            ret.value.getBoolean = ((array_p)left.value.getHeap->memory)->value ? True : False;
        }
        else if(left.type == type_object && right.type == type_null){
            ret.value.getBoolean = left.value.getHeap->memory ? True : False;
        }
        else if(left.type == type_null && right.type == type_array){
            ret.value.getBoolean = ((array_p)right.value.getHeap->memory)->value ? True : False;
        }
        else if(left.type == type_null && right.type == type_object){
            ret.value.getBoolean = right.value.getHeap->memory ? True : False;
        }
        else ret.value.getBoolean = True;
    }
    else{
        switch(left.type){
            case type_boolean:
                ret.value.getBoolean = left.value.getBoolean != right.value.getBoolean ? True : False;
                break;
            case type_character:
            case type_real:
                ret.value.getBoolean = left.value.getReal != right.value.getReal ? True : False;
                break;
            case type_string:
                ret.value.getBoolean = wcscmp(left.value.getString, right.value.getString) ? True : False;
                break;
            case type_array:
                ret.value.getBoolean = left.value.getHeap != right.value.getHeap ? True : False;
                break;
            case type_object:
                ret.value.getBoolean = left.value.getHeap != right.value.getHeap ? True : False;
                break;
            default:
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
    if(left.type == type_real && right.type == type_real){
        ret.value.getBoolean =
            left.value.getReal > right.value.getReal
                ? True
                : False;
    }
    return ret;
}

static result_t evaluateLargerOrEqual(result_t left, result_t right){
    result_t ret = {.type = type_boolean, .value.getBoolean = False};
    if(left.type == type_real && right.type == type_real){
        ret.value.getBoolean =
            left.value.getReal >= right.value.getReal
                ? True
                : False;
    }
    return ret;
}

static result_t evaluateLess(result_t left, result_t right){
    result_t ret = {.type = type_boolean, .value.getBoolean = False};
    if(left.type == type_real && right.type == type_real){
        ret.value.getBoolean =
            left.value.getReal < right.value.getReal
                ? True
                : False;
    }
    return ret;
}

static result_t evaluateLessOrEqual(result_t left, result_t right){
    result_t ret = {.type = type_boolean, .value.getBoolean = False};
    if(left.type == type_real && right.type == type_real){
        ret.value.getBoolean =
            left.value.getReal <= right.value.getReal
                ? True
                : False;
    }
    return ret;
}

static result_t evaluateAddition(result_t left, result_t right){
    result_t result;
    if(left.type == type_real && right.type == type_real){
        result.type = type_real;
        result.value.getReal = left.value.getReal + right.value.getReal;
    }
    return result;
}

static result_t evaluateSubtraction(result_t left, result_t right){
    result_t result;
    if(left.type == type_real && right.type == type_real){
        result.type = type_real;
        result.value.getReal = left.value.getReal - right.value.getReal;
    }
    return result;
}

static result_t evaluateMultiply(result_t left, result_t right){
    result_t result;
    if(left.type == type_real && right.type == type_real){
        result.type = type_real;
        result.value.getReal = left.value.getReal * right.value.getReal;
    }
    return result;
}

static result_t evaluateDivision(result_t left, result_t right){
    result_t result;
    if(left.type == type_real && right.type == type_real){
        result.type = type_real;
        result.value.getReal = left.value.getReal / right.value.getReal;
    }
    return result;
}

static result_t evaluateModule(result_t left, result_t right){
    result_t result;
    if(left.type == type_real && right.type == type_real){
        result.type = type_real;
        result.value.getReal = (int)left.value.getReal % (int)right.value.getReal;
    }
    return result;
}

static result_t evaluatePow(result_t left, result_t right){
    result_t result;
    if(left.type == type_real && right.type == type_real){
        result.type = type_real;
        result.value.getReal = pow(left.value.getReal, right.value.getReal);
    }
    return result;
}

static result_t evaluateRadix(result_t left, result_t right){
    result_t result;
    if(left.type == type_real && right.type == type_real){
        result.type = type_real;
        result.value.getReal = pow(left.value.getReal, 1.0 / right.value.getReal);
    }
    return result;
}

static void brackets(result_p result, result_t element, pointer_t value, type_value type){
    if(result->type == type_array && element.type == type_real){
        heap_p heap = result->value.getHeap;
        array_p array = heap->memory;
        type_value type = array->dimensions > 1 ? type_array : array->type;
        uint_t index = (uint_t)element.value.getReal;

        expectedToken(tok_operator, op_bracket_close, L"]");

        if(index >= 0 && index < array->length){
            pointer_t value = array->value + index * array->size;
            assign_result(value, result, type);
            assign_value(result, value, type);
        }
        else{
            printError(array_bounds_error, *token, NULL);
        }
    }
}

boolean_t assign_value(result_p result, pointer_t value, type_value type){
    if((token + 1) -> type == tok_operator){
        switch((++ token) -> intern){
            case op_assignment:
                ++ token;
                *result = evaluateAssignment(value, type, expression(__buf));
                return True;
            case op_negative_assignment:
                *result = evaluateNegativeAssignment(value, type, 1);
                return True;
            case op_increment:
                *result = evaluateIncrement(value, type, 1);
                return True;
            case op_decrement:
                *result = evaluateDecrement(value, type, 1);
                return True;
            case op_addition_assignment:
                ++ token;
                *result = evaluateAdditionAssignment(value, type, expression(__buf));
                return True;
            case op_subtraction_assignment:
                ++ token;
                *result = evaluateSubtractionAssignment(value, type, expression(__buf));
                return True;
            case op_multiply_assignment:
                ++ token;
                *result = evaluateMultiplyAssignment(value, type, expression(__buf));
                return True;
            case op_division_assignment:
                ++ token;
                *result = evaluateDivisionAssignment(value, type, expression(__buf));
                return True;
            case op_module_assignment:
                ++ token;
                *result = evaluateModuleAssignment(value, type, expression(__buf));
                return True;
            case op_pow_assignment:
                ++ token;
                *result = evaluatePowAssignment(value, type, expression(__buf));
                return True;
            case op_radix_assignment:
                ++ token;
                *result = evaluateRadixAssignment(value, type, expression(__buf));
                return True;
            case op_bracket_open:
                ++ token;
                brackets(result, expression(__buf), value, type);
                return True;
            default:
                -- token;
        }
    }
    return False;
}

static result_t term(pointer_t buf){
    result_t result = {};
    wstring_t check = NULL;
    switch(token -> type){
        case tok_reserved:
            switch(token->intern){
                case key_true:
                    result.type = type_boolean;
                    result.value.getBoolean = True;
                    break;
                case key_false:
                    result.type = type_boolean;
                    result.value.getBoolean = False;
                    break;
                case key_null:
                    result.type = type_null;
                    result.value.getPointer = NULL;
                    break;
                case key_new:
                    switch((++ token) -> type){
                        case tok_reserved:{
                            uint_t type = token->intern - tok_reserved;
                            uint_t length[num_array_dimensions];
                            register int i = 0;

                            while((++ token)->intern == op_bracket_open){
                                ++ token;
                                length[i++] = (uint_t)expression(buf).value.getReal;
                                expectedToken(tok_operator, op_bracket_close, L"]");
                            }

                            -- token;
                            result.type = type_array;
                            result.value.getHeap = new_array(type, i, length);
                            break;
                        }
                        case tok_identifier:
                            // Class
                            break;
                        default:
                            printError(syntax_error, *token, NULL);
                    }
                    break;
            }
            break;
        case tok_identifier:
            if((token + 1) -> intern == punctuation(L'(')){
                callFunction(token, &result, buf);
            }
            else{
                variable_p variable = findVariable(token->intern);

                if(!variable){
                    printError(undeclared_variable, *token, token->value);
                    exit(EXIT_FAILURE);
                }
                assign_result(variable->value, &result, variable->type);
                assign_value(&result, variable->value, variable->type);
            }
            break;
        case tok_punctuation:
            if(* token -> value == L'('){
                ++ token;
                result = expression(__buf);
                expectedToken(tok_punctuation, punctuation(L')'), L")");
            }
            break;
        case tok_operator:
            switch((token ++)->intern){
                case op_negative_assignment:{
                    variable_p variable = findVariable(token->intern);
                    if(!variable){
                        printError(undeclared_variable, *token, token->value);
                        exit(EXIT_FAILURE);
                    }
                    result = evaluateNegativeAssignment(variable->value, variable->type, -1);
                    break;
                }
                case op_increment:{
                    variable_p variable = findVariable(token->intern);
                    if(!variable){
                        printError(undeclared_variable, *token, token->value);
                        exit(EXIT_FAILURE);
                    }
                    result = evaluateIncrement(variable->value, variable->type, -1);
                    break;
                }
                case op_decrement:{
                    variable_p variable = findVariable(token->intern);
                    if(!variable){
                        printError(undeclared_variable, *token, token->value);
                        exit(EXIT_FAILURE);
                    }
                    result = evaluateDecrement(variable->value, variable->type, -1);
                    break;
                }
                case op_addition:
                    result = term(buf);
                    break;
                case op_subtraction:
                    result = term(buf);
                    result.value.getReal *= -1.0;
                    break;
                case op_not:
                    result = evaluateNot(term(buf));
                    break;
                case op_tilde:
                    result = evaluateTilde(term(buf));
                    break;
                case op_at:
                    result = evaluateAt(term(buf));
                    break;
            }
            break;
        case tok_constant:
            result.type = type_real;
            result.value.getReal = wcstod(token->value, &check);
            if(!check){
                printError(illegal_number, *token, token->value);
            }
            break;
        case tok_string:
            result.type = type_string;
            result.value.getString = new_wstring(token->value);
            break;
        default:
            break;
    }
    return result;
}

static void evaluateMultiPurpose(result_t*);
static void evaluateLogic(result_t*);
static void evaluateRelational(result_t*);
static void evaluateArithmeticSum(result_t*);
static void evaluateArithmeticMul(result_t*);
static void evaluateArithmeticPow(result_t*);
static void evaluateValue(result_t*);

result_t expression(pointer_t buf){
    result_t result;
    pointer_t backup = __buf;
    wchar_t str[100];

    __buf = buf;
    if(!setjmp(buf)){
        swprintf(str, L"<%s> ", token->value);
        evaluateMultiPurpose(&result);
        swprintr(result, str + wcslen(str));
        //wprintf(L" {%s}\n", str);
    }
    __buf = backup;
    return result;
}

static void evaluateMultiPurpose(result_t *current){
    result_t lvalue, rvalue;
    operator_types op;

    evaluateLogic(current);
    ++ token;

    while(token->intern > multi_purpose_operators && token->intern < logic_operators){
        lvalue = * current;
        op = token->intern;
        ++ token;
        evaluateLogic(&rvalue);

        switch(op){
            case op_fence:
                * current = evaluateFence(lvalue, rvalue);
                break;
            case op_dollar:
                * current = evaluateDollar(lvalue, rvalue);
                break;
            case op_ampersand:
                * current = evaluateAmpersand(lvalue, rvalue);
                break;
            case op_pipe:
                * current = evaluatePipe(lvalue, rvalue);
                break;
            case op_double_query:
                * current = evaluateDoubleQuery(lvalue, rvalue);
                break;
            case op_double_left:
                * current = evaluateDoubleLeft(lvalue, rvalue);
                break;
            case op_double_right:
                * current = evaluateDoubleRight(lvalue, rvalue);
                break;
            default:
                break;
        }
        ++ token;
    }
    -- token;
}

static void evaluateLogic(result_t *current){
    result_t lvalue, rvalue;
    operator_types op;

    evaluateRelational(current);
    ++ token;

    while(token->intern > logic_operators && token->intern < relational_operators){
        lvalue = * current;
        op = token->intern;
        ++ token;
        evaluateRelational(&rvalue);

        switch(op){
            case op_and:
                * current = evaluateAnd(lvalue, rvalue);
                break;
            case op_or:
                * current = evaluateOr(lvalue, rvalue);
                break;
            case op_xor:
                * current = evaluateXor(lvalue, rvalue);
                break;
            case op_if_then:
                * current = evaluateIfThen(lvalue, rvalue);
                break;
            case op_only_if:
                * current = evaluateOnlyIf(lvalue, rvalue);
                break;
            default:
                break;
        }
        ++ token;
    }
    -- token;
}

static void evaluateRelational(result_t *current){
    result_t lvalue, rvalue;
    operator_types op;

    evaluateArithmeticSum(current);
    ++ token;

    while(token->intern > relational_operators && token->intern < arithmetic_operators){
        lvalue = * current;
        op = token->intern;
        ++ token;
        evaluateArithmeticSum(&rvalue);

        switch(op){
            case op_equal:
                * current = evaluateEqual(lvalue, rvalue);
                break;
            case op_different:
                * current = evaluateDifferent(lvalue, rvalue);
                break;
            case op_near:
                * current = evaluateNear(lvalue, rvalue);
                break;
            case op_not_near:
                * current = evaluateNotNear(lvalue, rvalue);
                break;
            case op_near_or_identical:
                * current = evaluateNearOrIdentical(lvalue, rvalue);
                break;
            case op_larger:
                * current = evaluateLarger(lvalue, rvalue);
                break;
            case op_larger_or_equal:
                * current = evaluateLargerOrEqual(lvalue, rvalue);
                break;
            case op_less:
                * current = evaluateLess(lvalue, rvalue);
                break;
            case op_less_or_equal:
                * current = evaluateLessOrEqual(lvalue, rvalue);
                break;
            default:
                break;
        }
        ++ token;
    }
    -- token;
}

static void evaluateArithmeticSum(result_t *current){
    result_t lvalue, rvalue;
    operator_types op;

    evaluateArithmeticMul(current);
    ++ token;

    while(token->intern >= op_addition && token->intern <= op_subtraction){
        lvalue = * current;
        op = token->intern;
        ++ token;
        evaluateArithmeticMul(&rvalue);

        switch(op){
            case op_addition:
                * current = evaluateAddition(lvalue, rvalue);
                break;
            case op_subtraction:
                * current = evaluateSubtraction(lvalue, rvalue);
                break;
            default:
                break;
        }
        ++ token;
    }
    -- token;
}

static void evaluateArithmeticMul(result_t *current){
    result_t lvalue, rvalue;
    operator_types op;

    evaluateArithmeticPow(current);
    ++ token;

    while(token->intern >= op_multiply && token->intern <= op_module){
        lvalue = * current;
        op = token->intern;
        ++ token;
        evaluateArithmeticPow(&rvalue);

        switch(op){
            case op_multiply:
                * current = evaluateMultiply(lvalue, rvalue);
                break;
            case op_division:
                * current = evaluateDivision(lvalue, rvalue);
                break;
            case op_module:
                * current = evaluateModule(lvalue, rvalue);
                break;
            default:
                break;
        }
        ++ token;
    }
    -- token;
}

static void evaluateArithmeticPow(result_t *current){
    result_t lvalue, rvalue;
    operator_types op;

    evaluateValue(current);
    ++ token;

    while(token->intern >= op_pow && token->intern <= op_radix){
        lvalue = * current;
        op = token->intern;
        ++ token;
        evaluateValue(&rvalue);

        switch(op){
            case op_pow:
                * current = evaluatePow(lvalue, rvalue);
                break;
            case op_radix:
                * current = evaluateRadix(lvalue, rvalue);
                break;
            default:
                break;
        }
        ++ token;
    }
    -- token;
}

static void evaluateValue(result_t *current){
    *current = term(__buf);
}
