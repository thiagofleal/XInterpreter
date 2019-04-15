#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include "util.h"

const size_t size_type[] = {
    [type_boolean] = sizeof(boolean_t),
    [type_character] = sizeof(character_t),
    [type_integer] = sizeof(integer_t),
    [type_real] = sizeof(real_t),
    [type_string] = sizeof(wstring_t),
    [type_array] = sizeof(heap_p),
    [type_object] = sizeof(heap_p),
    [type_args] = sizeof(pointer_t)
};

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

void __check_fail__(string_t error, string_t file, int line){
    fprintf(stderr, "%s [%s => %i]\n", error, file, line);
    if(errno){
        fprintf(stderr, "errno: [%s]\n", strerror(errno));
    }
    exit(EXIT_FAILURE);
}

INLINE string_t new_string(char str[]){
    if(!str) return NULL;
    return strcpy(malloc((strlen(str) + 1) * sizeof(char)), str);
}

INLINE wstring_t new_wstring(wchar_t wstr[]){
    if(!wstr) return NULL;
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

void alloc_heap(heap_p heap, freeHeap function, pointer_t value){
    heap->count = 0;
    heap->destroy = function;
    heap->memory = value;
}

void manageHeap(heap_p *heap){
    if(heap){
        if(* heap){
            if(! -- (* heap)->count){
                if((* heap)->destroy){
                    (* heap)->destroy((* heap)->memory);
                }
            }
        }
    }
}

void assign_heap(heap_p *dest, heap_p src){
    manageHeap(dest);
    ++ src->count;
    * dest = src;
}

void assign_heap_null(heap_p* dest){
    manageHeap(dest);
    * dest = NULL;
}

void free_value(type_value type, pointer_t value){
    switch(type){
        case type_string:
            free(*(wstring_t*)value);
            break;
        case type_array:
        case type_object:
            assign_heap_null((heap_p*)value);
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

void assign_result(pointer_t src, result_t* dest, type_value type){
    if(dest->type == type_string){
        if(dest->value.getString){
            free(dest->value.getString);
        }
    }
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
            dest->value.getString = new_wstring(*(wstring_t*)src);
            break;
        case type_array:
        case type_object:
            dest->value.getHeap = *(heap_p*)src;
            break;
        case type_null:
            break;
        default:
            break;
    }
}

void assign_pointer(result_p src, pointer_t dest, type_value type){
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
            if(*(wstring_t*)dest){ free(*(wstring_t*)dest); }
            *(wstring_t*)dest = new_wstring(src->value.getString);
            break;
        case type_array:
            if(src->type == type_null){
                assign_heap(
                    (heap_p*)dest,
                    new_array_null(
                       ((array_p)(*(heap_p*)dest)->memory)->type,
                       ((array_p)(*(heap_p*)dest)->memory)->dimensions
                    )
                );
            }
            else{
                if(
                   ((array_p)(*(heap_p*)dest)->memory)->type ==
                   ((array_p)src->value.getHeap->memory)->type
                ){
                    if(
                       ((array_p)(*(heap_p*)dest)->memory)->dimensions ==
                       ((array_p)src->value.getHeap->memory)->dimensions
                    ){
                        assign_heap((heap_p*)dest, src->value.getHeap);
                    }
                    else{
                        printError(array_assignment_error, *token, L"Different dimensions");
                    }
                }else{
                    printError(array_assignment_error, *token, L"Different types");
                }
            }
            break;
        case type_object:
            if(src->type == type_null){
                assign_heap_null((heap_p*)dest);
            }
            else{
                assign_heap((heap_p*)dest, src->value.getHeap);
            }
            break;
        case type_null:
            break;
        default:
            break;
    }
}

void destroyArray(pointer_t _array){
    array_p array = _array;

    if(array->dimensions > 1){
        register uint_t i;

        for(i = 0; i < array->length; i++){
            destroyArray(
                (*(heap_p*)
                    (array->value + i * array->size)
                )->memory
            );
        }
    }
    free(array->value);
}

static heap_p new_array_node(type_value type, size_t size, uint_t length, int dimensions){
    heap_p heap = malloc(sizeof(heap_t));
    array_p array = malloc(sizeof(array_t));
    pointer_t memory = calloc(size, length);
    check(heap && array && memory);

    array->dimensions = dimensions;
    array->length = length;
    array->size = size;
    array->type = type;
    array->value = memory;

    alloc_heap(heap, destroyArray, array);
    return heap;
}

heap_p new_array(type_value type, uint_t dimensions, uint_t length[]){
    if(dimensions > 1){
        register int i;
        heap_p heap = new_array_node(type, sizeof(array_t), *length, dimensions);

        for(i = 0; i < *length; i++){
            *(pointer_t*)
                (((array_p)heap->memory)->value + i * sizeof(array_t))
                    = new_array(type, dimensions - 1, length + 1);
        }

        return heap;
    }
    else{
        return new_array_node(type, size_type[type], *length, 1);
    }
}

heap_p new_array_null(type_value type, uint_t dimensions){
    heap_p heap = malloc(sizeof(heap_t));
    array_p array = malloc(sizeof(array_t));
    check(heap && array);

    array->dimensions = dimensions;
    array->length = 0;
    array->size = size_type[type];
    array->type = type;
    array->value = NULL;

    heap->count = 0;
    heap->destroy = NULL;
    heap->memory = array;

    return heap;
}
