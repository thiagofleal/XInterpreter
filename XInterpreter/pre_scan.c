#include "header.h"
#include "util/util.h"

static uint_t count_global;

INLINE uint_t countGlobalVariables(void){
    return count_global;
}

void preScan(pointer_t buf){
    while(token->type != tok_end){
        switch(token->type){
            case tok_reserved:
                switch(token->intern){
                    case key_boolean:
                    case key_character:
                    case key_integer:
                    case key_real:
                    case key_string:
                    case key_object:
                        declareVariable(buf);
                        expectedToken(tok_punctuation, punctuation(L';'), L";");
                        break;
                    case key_include:
                        break;
                    case key_block:
                        break;
                    case key_class:
                        declareClass();
                        break;
                    default:
                        printError(syntax_error, * token, NULL);
                }
                break;
            case tok_identifier:
                declareFunction();
                break;
            case tok_constant:
            case tok_string:
            case tok_punctuation:
                printError(syntax_error, *token, token->value);
                break;
            case tok_operator:
                break;
            default:
                break;
        }
        ++ token;
    }

    count_global = countVariables();
}
