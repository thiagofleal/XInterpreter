#ifndef __INTERPRETER_HEADER_H__
#define __INTERPRETER_HEADER_H__

#include <wchar.h>

#if (defined __cplusplus || defined __GNUC_GNU_INLINE__)
#   define INLINE inline
#else
#   define INLINE
#endif // defined

#define True 0X1
#define False 0X0

typedef unsigned char boolean_t;
typedef wchar_t character_t;
typedef int integer_t;
typedef double real_t;

typedef unsigned int uint_t;
typedef void* pointer_t;
typedef char* string_t;
typedef wchar_t* wstring_t;

typedef struct{
    uint_t count;
    char memory[0];
}heap_t, *heap_p;

enum number{
    num_args = 50,
    num_variables = 1000,
    num_functions = 500,
    num_classes = 200,
    num_attributes = 100,
    num_methods = 200,
    num_constructors = 20,
    num_operators = 100
};

enum default_values{
    default_arguments = 100,
};

typedef enum{
    tok_reserved = 100,
    tok_identifier = 200,
    tok_constant = 0,
    tok_string = 30,
    tok_punctuation = 10,
    tok_operator = 20,
    tok_end = -1
}type_token;

enum key_words{
    /* Types */
    key_boolean = tok_reserved,
    key_character,
    key_integer,
    key_real,
    key_string,
    key_object,
    key_args,
    /* Values */
    key_true,
    key_false,
    key_null,
    /* Statements */
    key_if,
    key_else,
    key_do,
    key_while,
    key_for,
    key_each,
    key_using,
    key_return,
    key_call,
    /* Global */
    key_include,
    key_block,
    /* Exceptions */
    key_try,
    key_catch,
    key_finally,
    key_throw,
    /* Classes */
    key_class,
    key_extend,
    key_public,
    key_protected,
    key_private,
    key_constructor,
    key_destructor,
    /* Objects */
    key_new,
    key_this,
    key_base,
    key_virtual
};

typedef struct{
    type_token type;
    wstring_t value;
    int intern;
    int line;
    string_t file;
}token_t;

typedef enum{
    type_boolean = 0,
    type_character,
    type_integer,
    type_real,
    type_string,
    type_array,
    type_object,
    type_args,
    type_null
}type_value;

typedef enum{
    /*
    *   Attribution
    *
    *   =   assignment
    *   =>  arrow
    *   !!  negative assignment
    *   ++  simple increment
    *   --  simple decrement
    *   +=  addition assignment
    *   -=  subtraction assignment
    *   *=  multiply assignment
    *   /=  division assignment
    *   %=  module assignment
    *   ^=  pow assignment
    *   \=  radix assignment
    */
    assignment_operators = tok_operator,
    op_assignment,
    op_arrow,
    op_negative_assignment,
    op_increment,
    op_decrement,
    op_addition_assignment,
    op_subtraction_assignment,
    op_multiply_assignment,
    op_division_assignment,
    op_module_assignment,
    op_pow_assignment,
    op_radix_assignment,
    /*
    *   Multi-purpose
    *
    *   @   at
    *   #   fence
    *   $   dollar
    *   &   ampersand
    *   |   pipe
    *   ~   tilde
    *   ?   query
    *   ??  double query
    *   <<  double left
    *   >>  double right
    */
    multi_purpose_operators,
    op_at,
    op_fence,
    op_dollar,
    op_ampersand,
    op_pipe,
    op_tilde,
    op_query,
    op_double_query,
    op_double_left,
    op_double_right,
    /*
    *   Logic
    *
    *   !   not
    *   &&  and
    *   ||  or
    *   |&  exclusive or
    *   ->  if then
    *   <>  only if
    */
    logic_operators,
    op_not,
    op_and,
    op_or,
    op_xor,
    op_if_then,
    op_only_if,
    /*
    *   Relational
    *
    *   ==  equal
    *   !=  different
    *   ~~  near
    *   !~  not near
    *   ~=  near or identical
    *   >   larger than
    *   >=  larger or equal than
    *   <   less than
    *   <=  less or equal than
    */
    relational_operators,
    op_equal,
    op_different,
    op_near,
    op_not_near,
    op_near_or_identical,
    op_larger,
    op_larger_or_equal,
    op_less,
    op_less_or_equal,
    /*
    *   Arithmetic
    *
    *   +   addition
    *   -   subtraction
    *   *   multiply
    *   /   division
    *   %   module
    *   ^   pow
    *   \   radix
    */
    arithmetic_operators,
    op_addition,
    op_subtraction,
    op_multiply,
    op_division,
    op_module,
    op_pow,
    op_radix,
    /*
    *   Element access
    *
    *   .   dot
    *   ?.  query dot
    *   [   bracket open
    *   ]   bracket close
    */
    element_access_operators,
    op_dot,
    op_query_dot,
    op_bracket_open,
    op_bracket_close,

    end_operators
}operator_types;

typedef union{
    boolean_t getBoolean;
    character_t getCharacter;
    real_t getReal;
    wstring_t getString;
    pointer_t getPointer;
}value_t;

typedef struct{
    type_value type;
    value_t value;
}result_t;

typedef struct{
    heap_p value;
    type_value type;
    size_t size;
    int dimensions;
    uint_t length;
}array_t, *array_p;

typedef struct{
    uint_t identifier;
    type_value type;
    pointer_t value;
}variable_t, *variable_p;

typedef struct{
    uint_t identifier;
    int count_params;
    int param_type[num_args];
    token_t *enter;
}function_t, *function_p;

typedef struct{
    function_t *pfunction;
    result_t params[num_args];
    result_t result;
}function;

typedef enum{
    mode_public,
    mode_protected,
    mode_private
}visibility_mode;

typedef struct{
    variable_t super;
    visibility_mode visibility;
}attribute_t;

typedef struct{
    function_t super;
    visibility_mode visibility;
}method_t, *method_p;

typedef struct str_class class_t, *class_p;

struct str_class{
    uint_t identifier;
    class_t *super;
    int count_constructors;
    method_t constructor[num_constructors];
    method_t destructor;
    int count_attributes;
    attribute_t attributes[num_attributes];
    int count_methods;
    method_t methods[num_methods];
    int count_operators;
    method_t operators[num_operators];
};

typedef struct{
    class_p pclass;
    heap_p value;
}object_t, *object_p;

typedef enum{
    expected_token,
    non_terminated_single_quot,
    non_terminated_double_quot,
    non_terminated_commentary,
    undeclared_variable,
    undeclared_function,
    undeclared_attribute,
    undeclared_method,
    undeclared_class,
    illegal_number,
    array_assignment_error,
    array_bounds_error,
    syntax_error
}type_error;

extern token_t *token;

extern INLINE void printError(type_error error, token_t from, wstring_t message);
extern INLINE void expectedToken(type_token type, int intern, wstring_t value);

extern result_t expression(void);
extern variable_p findVariable(uint_t);

#endif // __INTERPRETER_HEADER_H__
