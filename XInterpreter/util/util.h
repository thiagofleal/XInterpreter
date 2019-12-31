#ifndef __INTERPRETER_UTIL_H__
#define __INTERPRETER_UTIL_H__

#include "../header.h"

typedef struct node node_t;

typedef void (* freeHeap)(heap_p);

struct node{
    pointer_t value;
    node_t *next;
};

typedef struct{
    node_t *begin;
    int size;
}list_t, *list;

extern const size_t size_type[];

list new_list(void);
void delete_list(list);
int list_add(list, pointer_t);
pointer_t list_access(list, int);
pointer_t list_remove(list, int);
int list_search(list, pointer_t, size_t);

extern void __check_fail__(string_t, string_t, int);

#define check(t)    ((t) ? (void)0 : __check_fail__(#t, __FILE__, __LINE__))

INLINE string_t new_string(char[]);
INLINE wstring_t new_wstring(wchar_t[]);

int string_near(string_t, string_t);
int wstring_near(wstring_t, wstring_t);

int swprintr(result_t, wstring_t);

void manageHeap(heap_p);
void decrement_heap(heap_p*);
void alloc_heap(heap_p, freeHeap, pointer_t);
void assign_heap(heap_p*, heap_p);
void assign_heap_null(heap_p*);

void free_value(type_value, pointer_t);
void free_result(result_t);

void assign_result(pointer_t, result_p, type_value);
void assign_pointer(result_p, pointer_t, type_value);

heap_p new_array(type_value, uint_t, uint_t[]);
heap_p new_array_null(type_value, uint_t);

#endif // __INTERPRETER_UTIL_H__
