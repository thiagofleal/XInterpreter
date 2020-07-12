#ifndef __INTERPRETER_HEADER_H__
#define __INTERPRETER_HEADER_H__

#include <wchar.h>

#if (defined __cplusplus || defined __GNUC_GNU_INLINE__)
#   define INLINE inline
#else
#   define INLINE
#endif // defined

#define True 0x1
#define False 0x0

typedef unsigned char boolean_t;
typedef wchar_t character_t;
typedef int integer_t;
typedef double real_t;

typedef unsigned int uint_t;
typedef void* pointer_t;
typedef char* string_t;
typedef wchar_t* wstring_t;

typedef struct str_heap heap_t, *heap_p;

struct str_heap{
    int count;
    void (* destroy)(heap_p);
    pointer_t memory;
};

enum number{
    num_args = 50,
    num_variables = 1000,
    num_functions = 500,
    num_classes = 200,
    num_attributes = 100,
    num_methods = 300,
    num_array_dimensions = 10
};

enum default_values{
    default_arguments = 100,
};

typedef enum{
    tok_reserved = 10,
    tok_identifier = 500,
    tok_constant = 0,
    tok_string = 30,
    tok_punctuation = 100,
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
    key_virtual,

    #ifdef __TEST__
        key_print,
    #endif // __TEST__

    count_key_words
};

#define punctuation(c)  (c + tok_punctuation)

typedef struct{
    type_token type;
    wstring_t value;
    int intern;
    int line;
    wstring_t file;
}token_t, *token_p;

typedef enum{
    type_boolean = 0,
    type_character,
    type_integer,
    type_real,
    type_string,
    type_object,
    type_args,
    type_array,
    type_null,
    num_types
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
    real_t getReal;
    wstring_t getString;
    pointer_t getPointer;
    heap_p getHeap;
}value_t, *value_p;

typedef struct{
    type_value type;
    value_t value;
}result_t, *result_p;

typedef struct{
    pointer_t value;
    type_value type;
    size_t size;
    int dimensions;
    uint_t length;
}array_t, *array_p;

typedef struct{
    result_p values;
    uint_t count;
}argument_t, *argument_p;

typedef struct{
    uint_t identifier;
    type_value type;
    pointer_t value;
}variable_t, *variable_p;

typedef struct{
    uint_t identifier;
    uint_t count_params;
    variable_t param[num_args];
    token_t *enter;
}function_t, *function_p;

typedef struct{
    uint_t count_actions;
    function_t actions[30];
}type_t, *type_p;

typedef enum{
    mode_public,
    mode_protected,
    mode_private
}visibility_mode;

typedef struct{
    variable_t super;
    visibility_mode visibility;
}attribute_t, *attribute_p;

typedef struct{
    function_t super;
    visibility_mode visibility;
}method_t, *method_p;

typedef struct str_class class_t, *class_p;

struct str_class{
    uint_t identifier;
    class_p super;
    uint_t count_attributes;
    attribute_t attributes[num_attributes];
    uint_t count_methods;
    method_t methods[num_methods];
};

typedef struct str_object object_t, *object_p;

struct str_object{
    heap_p super;
    class_p pclass;
    attribute_p attributes;
};

typedef enum{
    expected_token,
    non_terminated_single_quot,
    non_terminated_double_quot,
    non_terminated_commentary,
    non_terminated_block,
    undeclared_variable,
    undeclared_function,
    undeclared_attribute,
    undeclared_method,
    undeclared_class,
    illegal_number,
    array_assignment_error,
    array_bounds_error,
    not_boolean_expression,
    syntax_error
}type_error;

extern token_p token;

extern INLINE void printError(type_error error, token_t from, wstring_t message);
extern INLINE void expectedToken(type_token type, int intern, wstring_t value);

extern INLINE uint_t countVariables(void);
extern INLINE uint_t countGlobalVariables(void);
extern INLINE uint_t internIdentifier(wstring_t);
extern INLINE uint_t setCountVariables(uint_t);

extern void initTokens(const wstring_t, const wstring_t);
extern void declareVariable(pointer_t);
extern void destroyVariables(uint_t);
extern void declareFunction(void);
extern void executeBlock(pointer_t);
extern void preScan(pointer_t);
extern void executeFunction(function_p, result_t[], uint_t, result_p, pointer_t);
extern int callFunction(uint_t, result_p, pointer_t);
extern int callMethod(result_p, uint_t, result_p, visibility_mode, pointer_t);
extern void declareClass(void);

extern boolean_t assign_value(result_p, pointer_t, type_value);
extern int swprintr(result_t, wstring_t);

extern result_t expression(pointer_t);
extern variable_p findVariable(uint_t);
extern function_p findFunction(uint_t, uint_t);
extern class_p findClass(uint_t);
extern attribute_p findAttribute(object_p, uint_t, visibility_mode);

extern void pushThis(result_t);
extern void popThis(void);
extern void pushVirtual(result_t);
extern void popVirtual(void);
extern INLINE result_t getThis(void);
extern INLINE result_t getBase(void);
extern INLINE result_t getVirtual(void);

extern void initializeFunction(function_p);
extern void initializeVariable(pointer_t, pointer_t, uint_t*, size_t);
extern void freeVariableMemory(variable_p);
extern void declareParameters(function_p);
extern void allocateParameters(function_p, result_t[], uint_t);
extern void restaureVariables(variable_p, uint_t);
extern void setExec(boolean_t);
extern boolean_t findEndOfBlock(void);
extern uint_t getArguments(result_t[], pointer_t);
extern uint_t backupVariables(uint_t, variable_p);
extern heap_p instanceClass(class_p);
extern result_t getReturn(void);
extern result_t evaluateAssignment(pointer_t, type_value, result_t);
extern method_p findMethod(class_p, uint_t, uint_t, visibility_mode, uint_t*);

#endif // __INTERPRETER_HEADER_H__
