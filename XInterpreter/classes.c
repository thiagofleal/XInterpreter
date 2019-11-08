#include <stdlib.h>
#include <string.h>
#include "header.h"
#include "util/util.h"

extern boolean_t findEndOfBlock(void);
extern INLINE result_t getReturn(void);
extern void declareParameters(function_p);
extern void allocateParameters(function_p, result_t[], uint_t);
extern uint_t backupVariables(uint_t, variable_p);
extern void restaureVariables(variable_p, uint_t);
extern void initializeVariable(pointer_t, variable_p, uint_t*);
extern void initializeFunction(function_p);
extern void freeVariableMemory(variable_p);
extern uint_t getArguments(result_t[], pointer_t);

static uint_t count_classes;
static class_t classes[num_classes];
static result_t this_object;

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
                        initializeVariable(buf, (variable_p)pclass->attributes, &count);
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

void deleteInstance(pointer_t instance){
    object_p object = instance;
    register uint_t i;

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

INLINE void setThis(result_t value){
    this_object = value;
}

INLINE result_t getThis(void){
    return this_object;
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

int callMethod(class_p pclass, uint_t identifier, result_p ret, visibility_mode access, pointer_t buf){
    result_t args[num_args];
    uint_t count_args = 0;
    method_p method = NULL;

    expectedToken(tok_punctuation, punctuation(L'('), L"(");
    count_args = getArguments(args, buf);
    method = findMethod(pclass, identifier, count_args, access);
    expectedToken(tok_punctuation, punctuation(L')'), L")");

    if(method){
        executeFunction((function_p)method, args, count_args, ret, buf);
        return 1;
    }
    return -count_args;
}
