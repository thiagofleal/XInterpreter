#ifndef __INTERPRETER_UTIL_H__
#define __INTERPRETER_UTIL_H__

#include "../header.h"

typedef struct node node_t;

struct node{
    pointer_t value;
    node_t *next;
};

typedef struct{
    node_t *begin;
    int size;
}list_t, *list;

list new_list(void);
void delete_list(list);
void list_add(list, pointer_t);
pointer_t list_access(list, int);
pointer_t list_remove(list, int);
int list_search(list, pointer_t, size_t);

string_t new_string(char[]);
wstring_t new_wstring(wchar_t[]);

int string_near(string_t, string_t);
int wstring_near(wstring_t, wstring_t);

void assign_result(pointer_t, result_t*, int);
void assign_pointer(result_t, pointer_t, int);

#endif // __INTERPRETER_UTIL_H__
