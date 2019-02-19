#include <stdlib.h>
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

void list_add(list l, pointer_t value){
    node_t *node = new_node(value);
    if(l->size++){
        list_end(l)->next = node;
    }
    else{
        l->begin = node;
    }
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
        for(i = 0; node->next; i++, node = node->next)
            if(!memcmp(node->value, value, size))
                return i;
    return -1;
}

string_t new_string(char str[]){
    return strcpy(malloc((strlen(str) + 1) * sizeof(char)), str);
}

wstring_t new_wstring(wchar_t wstr[]){
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

void assign_result(pointer_t src, result_t* dest, int type){
    switch(dest->type = type){
        case type_boolean:
            dest->value.getBoolean = *(boolean_t*)src;
            break;
        case type_character:
            dest->value.getCharacter = *(character_t*)src;
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
    }
}

void assign_pointer(result_t src, pointer_t dest, int type){
    switch(type){
        case type_boolean:
            *(boolean_t*)dest = src.value.getBoolean;
            break;
        case type_character:
            *(character_t*)dest = src.value.getCharacter;
            break;
        case type_integer:
            *(integer_t*)dest = (integer_t)src.value.getReal;
            break;
        case type_real:
            *(real_t*)dest = src.value.getReal;
            break;
        case type_string:
            *(wstring_t*)dest = src.value.getString;
            break;
        case type_array:
            *(array_p*)dest = (array_t*)src.value.getPointer;
            break;
        case type_object:
            *(object_p*)dest = (object_t*)src.value.getPointer;
            break;
    }
}
