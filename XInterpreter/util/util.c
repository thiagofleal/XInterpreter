#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include "util.h"

list new_list(void){
    list ret = malloc(sizeof(list_t));
    assert(ret);
    ret->size = 0;
    return ret;
}

void delete_list(list l){
    while(l->size){
        list_remove(l, 0);
    }
    free(l);
}

static node_t *list_end(list l){
    node_t *ret = l->begin;
    if(ret)
        while(ret->next)
            ret = ret->next;
    return ret;
}

static node_t *new_node(pointer_t value){
    node_t *node = malloc(sizeof(node_t));
    assert(node);
    node->next = NULL;
    node->value = value;
    return node;
}

static void delete_node(node_t *node){
    free(node);
}

int list_add(list l, pointer_t value){
    register int ret = l->size;
    node_t *node = new_node(value);
    if(ret){
        list_end(l)->next = node;
    }
    else{
        l->begin = node;
    }
    ++ l->size;
    return ret;
}

static node_t * access_node(list l, int index){
    register int i = 0;
    node_t *node = l->begin;
    for(;i < index && node->next; i++, node = node->next);
    return node;
}

pointer_t list_access(list l, int index){
    return access_node(l, index)->value;
}

pointer_t list_remove(list l, int index){
    pointer_t ret;
    node_t *node;

    if(index > 0){
        node_t *aux = access_node(l, index - 1);
        node = aux->next;
        aux->next = aux->next->next;
        ret = node->value;
    }
    else{
        node = l->begin;
        ret = node->value;
        l->begin = node->next;
    }

    delete_node(node);
    -- l->size;
    return ret;
}

int list_search(list l, pointer_t value, size_t size){
    register int i;
    node_t *node = l->begin;
    if(l->size)
        for(i = 0; i < l->size; i++, node = node->next)
            if(!memcmp(node->value, value, size))
                return i;
    return -1;
}

void __check_fail__(string_t error){
    fprintf(stderr, "%s\n", error);
    exit(EXIT_FAILURE);
}

INLINE string_t new_string(char str[]){
    return strcpy(malloc((strlen(str) + 1) * sizeof(char)), str);
}

INLINE wstring_t new_wstring(wchar_t wstr[]){
    return wcscpy(malloc((wcslen(wstr) + 1) * sizeof(wchar_t)), wstr);
}

int string_near(string_t str1, string_t str2){
    while(* str1 && * str2){
        if(* str1 != * str2){
            if(isupper(* str1)){
                if(tolower(* str1) != * str2){
                    return 0;
                }
            }
            else{
                if(toupper(* str1) != * str2){
                    return 0;
                }
            }
        }
        ++ str1; ++ str2;
    }
    return 1;
}

int wstring_near(wstring_t str1, wstring_t str2){
    while(* str1 && * str2){
        if(* str1 != * str2){
            if(iswupper(* str1)){
                if(towlower(* str1) != * str2){
                    return 0;
                }
            }
            else{
                if(towupper(* str1) != * str2){
                    return 0;
                }
            }
        }
        ++ str1; ++ str2;
    }
    return 1;
}

heap_p new_heap(size_t memory, void (* destroy)(pointer_t)){
    heap_p heap = calloc(sizeof(heap_t) + memory, 1);
    heap->count = 0;
    heap->destroy = destroy;
    return heap;
}

void assign_heap(heap_p *dest, heap_p *src){
    if(* dest){
        if(! -- (* dest)->count){
            if((*dest)->destroy){
                (*dest)->destroy(* dest);
            }
            else{
                free(* dest);
            }
        }
    }
    ++ (* src)->count;
    * dest = * src;
}

void assign_heap_null(heap_p *dest){
    if(! -- (* dest)->count){
        free(* dest);
    }
    * dest = NULL;
}

void free_value(type_value type, pointer_t value){
    switch(type){
        case type_string:
            free(*(wstring_t*)value);
            break;
        case type_array:
            break;
        case type_object:
            break;
        case type_args:
            break;
        default:
            break;
    }
}

void free_result(result_t result){
    switch(result.type){
        case type_string:
            free(result.value.getString);
            break;
        case type_array:
            break;
        case type_object:
            break;
        case type_args:
            break;
        default:
            break;
    }
}

void assign_result(pointer_t src, result_t* dest, int type){
    switch(dest->type = type){
        case type_boolean:
            dest->value.getBoolean = *(boolean_t*)src;
            break;
        case type_character:
            dest->value.getReal = (real_t)*(character_t*)src;
            break;
        case type_integer:
            dest->type = type_real;
            dest->value.getReal = (real_t)*(integer_t*)src;
            break;
        case type_real:
            dest->value.getReal = *(real_t*)src;
            break;
        case type_string:
            dest->value.getString = *(wstring_t*)src;
            break;
        case type_array:
            dest->value.getPointer = *(array_p*)src;
            break;
        case type_object:
            dest->value.getPointer = *(object_p*)src;
            break;
        default:
            break;
    }
}

void assign_pointer(result_p src, pointer_t dest, int type){
    switch(type){
        case type_boolean:
            *(boolean_t*)dest = src->value.getBoolean;
            break;
        case type_character:
            *(character_t*)dest = (character_t)src->value.getReal;
            break;
        case type_integer:
            *(integer_t*)dest = (integer_t)src->value.getReal;
            break;
        case type_real:
            *(real_t*)dest = src->value.getReal;
            break;
        case type_string:
            *(wstring_t*)dest = src->value.getString;
            break;
        case type_array:
            if((array_p)dest){
                if(((array_p)dest)->dimensions == ((array_p)src->value.getPointer)->dimensions){
                    assign_heap(&((array_p)dest)->value, &((array_p)src->value.getPointer)->value);
                    ((array_p)dest)->length = ((array_p)src->value.getPointer)->length;
                }
                else{
                    printError(array_assignment_error, *token, NULL);
                }
            }
            else{
                assign_heap_null(&((array_p)dest)->value);
                ((array_p)dest)->length = 0;
            }
            break;
        case type_object:
            *(object_p*)dest = (object_t*)src->value.getPointer;
            break;
    }
}
