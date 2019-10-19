/*
 * Stack.c
 *
 *  Created on: Oct 16, 2019
 *      Author: artem
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <errno.h>
#include <string.h>
#include "StackConsts.h"
#include "Stack.h"

enum { //Error numbers
    ECANARY1 = 132,
    ECANARY2 = 133,
    EHASH  = 134,
    ECANARYBOTH = EADDRINUSE
};

int StackConstruct(stack* stk) { // +argument buff
    assert(stk != NULL);
    stk->canary1 = c1;
    stk->size = 0;
    stk->buff = 1;
    stk->stk = NULL;
    stk->stk = (stk_t*)calloc(1, sizeof(stk_t));
    assert(stk->stk != NULL);
    stk->hash = h1 + 1;
    stk->err = 0;
    stk->canary2 = c2;
    FILE* fw = fopen("StkCheck.txt", "w");
    if(fw) fclose(fw);
    FILE* fwe = fopen("StkErrs.txt", "w");
    if(fwe) fclose(fwe);
    return 0;
}

ui Hash(const stack stk) {
    ui hash = h1;
    for(ui i = 0; i < stk.size; i++) { //Check if buff > size
        hash += stk.stk[i].field * (i+1);
    }
    hash += stk.buff;
    return hash;
}

int StackOk(const stack stk) {
    if(stk.canary1 != c1 && stk.canary2 != c2)
        return ECANARYBOTH;
    if(stk.canary1 != c1)
        return ECANARY1;
    if(stk.canary2 != c2)
        return ECANARY2;
    if(stk.stk == NULL || stk.size > stk.buff)
        return ENOBUFS;
    if(stk.hash != Hash(stk)){
        return EHASH;
    }
    return stk.err;
}

int StackPush(stack *stk, stk_t a) {
    assert(stk != NULL);
    if(!PrintErrors(StackOk(*stk))) return 0;

    stk->size++; //del

    if(stk->size > stk->buff) {
        if(stk->buff * 2 < stk->size) {
            stk->err = EOVERFLOW;
            stk->size--;
            return EOVERFLOW;
        }
        stk->buff *= 2;
        stk->stk = (stk_t*)realloc(stk->stk, stk->buff*sizeof(stk_t)); //memory leak possible
        if(stk->stk == NULL) {
            stk->err = ENOMEM;
            return ENOMEM;
        }
    }

    stk->stk[stk->size-1] = a; //postfix
    stk->hash = Hash(*stk);
    PrintErrors(StackOk(*stk));
    return 0;
}

int StackPop(stack *stk, stk_t *a) {
    assert(stk != NULL);
    assert(a   != NULL);
    if(!PrintErrors(StackOk(*stk))) return 0;
    if(stk->size == 0) return 0; //Stack free

    //*a = stk->stk[stk->(--size)];
    *a = stk->stk[stk->size-1];
    stk->size--;

    if(stk->size > odds) { //sll
        if(stk->size <= stk->buff/2 - odds) {
            stk->buff /= 2;
            stk->stk = (stk_t*)realloc(stk->stk, stk->buff); //поправь
            if(stk->stk == NULL) {
                stk->err = ENOMEM;
                return ENOMEM;
            }
        }
    }

    stk->hash = Hash(*stk);
    PrintErrors(StackOk(*stk));
    return 0;
}

int StackPrint(stack* stk) { //StackOk
    assert(stk != NULL);
    FILE* fw = fopen("StkCheck.txt", "a");
    if(fw == NULL) {
        perror("fopen");
        return ENFILE;
    }

    llu name = (llu)stk;
    name = (name & mask) >> 24;

    fprintf(fw, "stack_t stk%llu [%p] %s\n{\n", name, stk, CheckAddress(stk));
    fprintf(fw, "   canary1 = %x\n", stk->canary1);
    fprintf(fw, "   size = %u\n",    stk->size);
    fprintf(fw, "   stack[%u] = [%p] %s\n   {\n", stk->size, stk->stk, CheckAddress(stk->stk));
    ui i;
    for(i = 0; i < stk->size && i < stk->buff; i++) {
        fprintf(fw, "       [%u] = %d%s\n", i, stk->stk[i].field, CheckNumber(stk->stk[i]));
    }
    fprintf(fw, "   }\n");
    fprintf(fw, "   hash = %x\n",  stk->hash);
    fprintf(fw, "   buff = %u\n",  stk->buff);
    fprintf(fw, "   errno = %u\n", stk->err);
    fprintf(fw, "   canary2 = %x\n}\n", stk->canary2);

    fclose(fw);
    return 0;
}

char* CheckAddress(void* p) {//address
    if(p == NULL || (llu)p == 0 || (llu)p == 0xFFFFFFFFFFFFFFFF)
        return "(Fail?)";
    return "(OK)";
}

char* CheckNumber(stk_t a) {
    if(a.field == 0 || a.field < (-lim) || a.field > lim)
        return " (Poison?)";
    return "";
}

int StackDestruct(stack* stk) {
    assert(stk != NULL);
    stk->canary1 = c1;
    stk->size    = 0;
    stk->buff    = 0;
    if(stk->stk)
        free(stk->stk);
    stk->stk     = NULL;
    stk->hash    = h1;
    stk->err     = 0;
    stk->canary2 = c2;
    return 0;
}

int PrintErrors(int error) {
    if(error == 0)
        return 1;
    FILE* fwe = fopen("StkErrs.txt", "a");
    if(fwe == NULL) {
        errno =  ENFILE;
        return 0;
    }

    char *s;
    switch(error){
    case ECANARYBOTH:
        s = strdup("Canary1 and canary2 are changed, maybe some array went beyond the boundaries and poison stack!\n");
        errno = EADDRINUSE;
        break;
    case ECANARY1:
        s = strdup("Canary1 is changed, maybe some array went beyond the boundaries and poison stack!\n");
        errno = EADDRINUSE;
        break;
    case ECANARY2:
        s = strdup("Canary2 is changed, maybe some array went beyond the boundaries and poison stack!\n");
        errno = EADDRINUSE;
        break;
    case EHASH:
        s = strdup("Hash is changed, our stack is poison!\n");
        errno = EADDRINUSE;
        break;
    case ENFILE:
        s = strdup("Can't open file!\n");
        errno = ENFILE;
        break;
    case ENOMEM:
        s = strdup("Can't reserve memory for stack!\n");
        errno = ENOMEM;
        break;
    default:
        s = strdup("There is extra error!\n");
        if(error < 131) errno = error;
        else errno = EPERM;
        break;
    }
    fprintf(fwe, "ERROR %d: %s", error, s);
    free(s);
    fclose(fwe);
    return 0;
}
