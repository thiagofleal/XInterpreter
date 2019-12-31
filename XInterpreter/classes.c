#include <stdlib.h>
#include <string.h>
#include "header.h"
#include "util/util.h"

static uint_t count_classes;
static class_t classes[num_classes];

INLINE void initializeMethod(class_p pclass, visibility_mode visibility){
    initializeFunction((function_p)(pclass->methods + pclass->count_methods));
    pclass->methods[pclass->count_methods++].visibility = visibility;
}

void initializeClass(pointer_t buf, class_p pclass, uint_t identifier){
    visibility_mode current_visibility = mode_public;

    pclass->identifier = identifier;
    expectedToken(tok_punctuation, punctuation(L'{'), L"{");

    while((++ token)->intern != punctuation('}')){
        switch(token->type){
            case tok_reserved:
                switch(token->intern){
                    case key_boolean:
                    case key_character:
                    case key_integer:
                    case key_real:
                    case key_string:
                    case key_object:{
                        uint_t count = pclass->count_attributes;
                        initializeVariable(buf, pclass->attributes, &count, sizeof(attribute_t));
                        expectedToken(tok_punctuation, punctuation(L';'), L";");
                        while(pclass->count_attributes < count){
                            pclass->attributes[pclass->count_attributes].visibility = current_visibility;
                            ++ pclass->count_attributes;
                        }
                        break;
                    }
                    case key_extend:
                        break;
                    case key_public:
                    case key_protected:
                    case key_private:
                        current_visibility = (token->intern - key_public) + mode_public;
                        expectedToken(tok_punctuation, punctuation(L':'), L":");
                        break;
                    case key_constructor:
                    case key_destructor:
                        initializeMethod(pclass, current_visibility);
                        break;
                }
                break;
            case tok_identifier:
                initializeMethod(pclass, current_visibility);
                break;
            case tok_operator:
                break;
            default:
                printError(syntax_error, *token, NULL);
        }
    }
}

void declareClass(void){
    uint_t identifier;
    expectedToken(tok_punctuation, punctuation(L':'), L":");
    identifier = (++ token)->intern;
    initializeClass(NULL, classes + count_classes, identifier);
    ++ count_classes;
}

class_p findClass(uint_t id){
    register int i;
    for(i = count_classes; i >= 0; i--){
        if(classes[i].identifier == id){
            return classes + i;
        }
    }
    return NULL;
}

void deleteInstance(heap_p heap){
    object_p object = heap->memory;
    register uint_t i;
    result_t r = {
        .type = type_object,
        .value.getHeap = heap
    };

    callMethod(&r, key_destructor, NULL, mode_public, NULL);

    for(i = 0; i < object->pclass->count_attributes; i++){
        freeVariableMemory((variable_p)object->pclass->attributes + i);
    }
    free(object->pclass->attributes);
    free(object);
}

object_p instanceClass(class_p pclass){
    if(pclass){
        object_p object = malloc(sizeof(object_t));
        register uint_t i;

        check(object);

        object->pclass = pclass;
        object->super = instanceClass(pclass->super);
        object->attributes = malloc(pclass->count_attributes * sizeof(attribute_t));

        check(object->attributes);

        for(i = 0; i < pclass->count_attributes; i++){
            memcpy(object->attributes + i, pclass->attributes + i, sizeof(attribute_t));
            object->attributes[i].super.value = malloc(size_type[pclass->attributes[i].super.type]);
            check(object->attributes[i].super.value);

            switch(pclass->attributes[i].super.type){
                case type_string:
                    *(wstring_t*)object->attributes[i].super.value = new_wstring(
                        pclass->attributes[i].super.value
                    );
                    break;
                case type_object:
                case type_array:
                    assign_heap(
                        (heap_p*)object->attributes[i].super.value,
                        *(heap_p*)pclass->attributes[i].super.value
                    );
                    break;
                default:
                    memcpy(
                        object->attributes[i].super.value,
                        pclass->attributes[i].super.value,
                        size_type[pclass->attributes[i].super.type]
                    );
                    break;
            }
        }

        return object;
    }
    return NULL;
}

heap_p newObject(class_p pclass){
    heap_p heap = malloc(sizeof(heap_t));
    object_p object = instanceClass(pclass);

    check(heap && object);

    alloc_heap(heap, deleteInstance, object);
    return heap;
}

static struct str_stack {
    result_p value;
    struct str_stack *prev;
} *this_stack = NULL;

void pushThis(result_p value){
    struct str_stack *node = malloc(sizeof(struct str_stack));
    check(node);
    node->value = value;
    node->prev = this_stack;
    this_stack = node;
}

void popThis(void){
    struct str_stack *node = this_stack;
    this_stack = node->prev;
    free(node);
}

INLINE result_p getThis(void){
    return this_stack->value;
}

attribute_p findAttribute(object_p object, uint_t identifier, visibility_mode access){
    register int i;
    class_p pclass = object->pclass;
    for(i = pclass->count_attributes - 1; i >= 0; i--){
        if(access >= pclass->attributes[i].visibility){
            if(pclass->attributes[i].super.identifier == identifier){
                return object->attributes + i;
            }
        }
    }
    return NULL;
}

method_p findMethod(class_p pclass, uint_t id, uint_t count_arguments, visibility_mode access){
    register int i;

    for(i = pclass->count_methods - 1; i >= 0; i--){
        if(access >= pclass->methods[i].visibility){
            if(pclass->methods[i].super.identifier == id){
                if(pclass->methods[i].super.count_params == count_arguments){
                    return pclass->methods + i;
                }
                else if(pclass->methods[i].super.count_params >= default_arguments){
                    if(count_arguments >= pclass->methods[i].super.count_params - default_arguments){
                        return pclass->methods + i;
                    }
                }
            }
        }
    }
    return NULL;
}

INLINE method_p findConstructor(class_p pclass, uint_t num_arguments, visibility_mode access_mode){
    return findMethod(pclass, key_constructor, num_arguments, access_mode);
}

int callMethod(result_p src, uint_t identifier, result_p ret, visibility_mode access, pointer_t buf){
    result_t args[num_args];
    uint_t count_args = 0;
    function_p method = NULL;
    heap_p heap = src->value.getHeap;
    object_p object = heap->memory;

    check(object);

    expectedToken(tok_punctuation, punctuation(L'('), L"(");
    count_args = getArguments(args, buf);
    expectedToken(tok_punctuation, punctuation(L')'), L")");
    method = (function_p)findMethod(object->pclass, identifier, count_args, access);

    if(method){
        pushThis(src);
        executeFunction(method, args, count_args, ret, buf);
        popThis();
        manageHeap(heap);
        return 1;
    }
    return -count_args;
}
