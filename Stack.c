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
//#include "StackConsts.h"
#include "Stack.h"
#include <math.h>

#define DEBUG

enum { //Error numbers
    ECANARY1 = 132,
    ECANARY2 = 133,
    EHASH  = 134,
    ECANARYBOTH = 135,
    EOPFILEERR = 136,
    EOPFILECH = 137,
    EUNDERFLOW = 138,
    EBUFEMPTY = ENOBUFS,
    ENOTENMEM = ENOMEM,
    BUFFOVERFLOW = EOVERFLOW

};

stack StackConstruct(ui buff, char* StkCheckFile, char* StkErrFile) {
    stack stk;
    stk.err = 0;
    stk.canary1 = CANARY1_CONST;
    stk.size = 0;
    if(buff >= 0) stk.buff = buff;
    else {
        stk.err = EUNDERFLOW;
        stk.buff = 0;
    }
    stk.stk = NULL;
    stk.stk = (stk_t*)calloc(buff, sizeof(stk_t));
    if(! stk.stk) {
        stk.err = EBUFEMPTY;
        buff = 0;
    }
    else {
        ui i;
        for(i = 0; i < buff; i++)
            stk.stk[i] = POISON;
    }
    stk.hash = HASH_START + buff;
    stk.canary2 = CANARY2_CONST;

#ifdef DEBUG
    if(StkCheckFile == NULL)
        stk.stkprint = fopen(STR_CHECK_FILE, "w");
    else
        stk.stkprint = fopen(StkCheckFile, "w");
    if(stk.stkprint == NULL) {
        stk.err = EOPFILECH;
        PrintSAE(&stk);
        stk.stkprint = stderr;
    }

    if(StkErrFile == NULL)
        stk.ferr = fopen(STR_ERR_FILE, "w");
    else
        stk.ferr = fopen(StkErrFile, "w");
    if(stk.ferr == NULL) {
        stk.err = EOPFILECH;
        PrintSAE(&stk);
        stk.ferr = stderr;
    }

    if(stk.err != 0) PrintSAE(&stk);
#endif //DEBUG
    return stk;
}

ui Hash(const stack stk) {
    ui hash = HASH_START;
    ui len = 0;
    if(stk.buff >= stk.size)
        len = stk.size;
    else
        len = stk.buff;
    if(stk.stk == NULL) return hash;
    for(ui i = 0; i < len; i++) {
        hash += stk.stk[i] * (i+1);
    }
    hash += stk.buff;
    return hash;
}

int StackOk(stack *stk) {
    MyAssert(stk);
    if(stk->stk == NULL && stk->err != ENOTENMEM) {
        stk->err = EBUFEMPTY;
        return EBUFEMPTY;
    }
    if(stk->canary1 != CANARY1_CONST && stk->canary2 != CANARY2_CONST) {
        stk->err = ECANARYBOTH;
        return ECANARYBOTH;
    }
    if(stk->canary1 != CANARY1_CONST) {
        stk->err = ECANARY1;
        return ECANARY1;
    }
    if(stk->canary2 != CANARY2_CONST) {
        stk->err = ECANARY2;
        return ECANARY2;
    }
    if(stk->size > stk->buff) {
        stk->err = ENOTENMEM;
        return ENOTENMEM;
    }
    if(stk->hash != Hash(*stk)) {
        stk->err = EHASH;
        return EHASH;
    }
    return stk->err;
}

int StackPush(stack *stk, stk_t a) {
    MyAssert(stk);
    int check = 0;
#ifdef DEBUG
    check = StackCheck(stk);
    if(check) {
        if(check == EBUFEMPTY){
            stk->err = EBUFEMPTY;
            PrintSAE(stk);
            return EBUFEMPTY;
        }
        else PrintSAE(stk);
    }
#endif
    stk_t* sup;

    if(stk->size + 1 > stk->buff) {
#ifdef DEBUG
        if(stk->buff * 2 < stk->size) {
            stk->err = BUFFOVERFLOW;
            PrintSAE(stk);
            return BUFFOVERFLOW;
        }
#endif
        stk->buff *= 2;
        sup = (stk_t*)realloc(stk->stk, stk->buff * sizeof(stk_t));
        if(sup == NULL) {
            stk->err = ENOTENMEM;
#ifdef DEBUG
            PrintSAE(stk);
#endif
            return ENOTENMEM;
        }
        else {
            stk->stk = sup;
            ui i;
            for(i = stk->size; i < stk->buff; i++)
                stk->stk[i] = POISON;
        }

    }

    stk->stk[stk->size++] = a; //postfix
#ifdef DEBUG
    stk->hash = Hash(*stk);
    check = StackCheck(stk);
    if(check) PrintSAE(stk);
#endif
    return check;
}

stk_t StackPop(stack *stk) {
    MyAssert(stk);
    stk_t a;
#ifdef DEBUG
    if(StackCheck(stk))
        PrintSAE(stk);
    if(stk->size == 0) {
        stk->err = EUNDERFLOW;
        PrintSAE(stk);
        return POISON; //Stack free
    }
#endif
#ifndef DEBUG
    if(stk->size == 0) {
        stk->err = EUNDERFLOW;
        return POISON; //Stack free
    }
#endif
    a = stk->stk[--stk->size];

    if(stk->size <= stk->buff/2 - ODDS) {
        stk->buff /= 2;
        stk_t* sup = (stk_t*)realloc(stk->stk, stk->buff);
#ifdef DEBUG
        if(sup == NULL) {
            stk->err = ENOTENMEM;
            PrintSAE(stk);
            return a;
        }
#endif
        stk->stk = sup;
    }

#ifdef DEBUG
    stk->hash = Hash(*stk);
    if(StackCheck(stk))
        PrintSAE(stk);
#endif
    return a;
}

int StackPrint(stack* stk) {
    MyAssert(stk);
    if(stk->stkprint == NULL)
        return 0;
    StackCheck(stk);

    llu name = (llu)stk;
    name = (name & MASK) >> 24;

    fprintf(stk->stkprint, "stack_t stk%llu [%p] %s\n{\n", name, stk, CheckAddress(stk));
    fprintf(stk->stkprint, "   canary1 = 0x%X\n", stk->canary1);
    fprintf(stk->stkprint, "   size = %u\n",    stk->size);
    fprintf(stk->stkprint, "   stack[%u] = [%p] %s\n   {\n", stk->size, stk->stk, CheckAddress(stk->stk));
    ui i;
    ui len;
    if(stk->size <= stk->buff) len = stk->size;
    else len = stk->buff;
    if(stk->stk != NULL)
        for(i = 0; i < len; i++) {
            fprintf(stk->stkprint, "       [%u] = %d%s\n", i, stk->stk[i], CheckNumber(stk->stk[i]));
        }
    fprintf(stk->stkprint, "   }\n");
    fprintf(stk->stkprint, "   hash = 0x%X\n",  stk->hash);
    fprintf(stk->stkprint, "   buff = %u\n",  stk->buff);
    fprintf(stk->stkprint, "   Stack print stream [%p] %s\n", stk->stkprint, CheckAddress(stk->stkprint));
    fprintf(stk->stkprint, "   Stack errors stream [%p] %s\n", stk->ferr, CheckAddress(stk->ferr));
    fprintf(stk->stkprint, "   errno = %u", stk->err);
    if(stk->err != 0)
        fprintf(stk->stkprint, "  (to get info about errors, use PrintErrors(const stack stk))\n");
    else
        fprintf(stk->stkprint, "  (OK)\n");
    fprintf(stk->stkprint, "   canary2 = 0x%X\n}\n", stk->canary2);
    //stk->hash = Hash(*stk);
    return 0;
}

char* CheckAddress(void* p) {//address
    MyAssert(p);
    if(p == NULL || (llu)p == 0 || (llu)p == 0xFFFFFFFFFFFFFFFF)
        return "(Fail?)";
    return "(OK)";
}

char* CheckNumber(stk_t a) {
    if(a == POISON || a < (-LIMIT) || a > LIMIT)
        return " (Poison?)";
    return "";
}

int StackDestruct(stack* stk) {
    MyAssert(stk);
    stk->canary1 = CANARY1_CONST;
    stk->size    = 0;
    stk->buff    = 0;
    if(stk->stk)
        free(stk->stk);
    stk->stk     = NULL;
    stk->hash    = HASH_START;
#ifdef DEBUG
    if(stk->ferr)
        fclose(stk->ferr);
    stk->ferr = NULL;
    if(stk->stkprint)
        fclose(stk->stkprint);
    stk->stkprint = NULL;
#endif
    stk->err     = 0;
    stk->canary2 = CANARY2_CONST;
    return 0;
}

int PrintError(const stack stk) {
    if(stk.err == 0)
        return 1;
    if(stk.ferr == NULL)
        return EOPFILEERR;

    char *s;
    switch(stk.err){
    case ECANARYBOTH:
        s = strdup("Canary1 and canary2 are changed, maybe some array went beyond the boundaries and poison stack!");
        break;
    case ECANARY1:
        s = strdup("Canary1 is changed, maybe some array went beyond the boundaries and poison stack!");
        break;
    case ECANARY2:
        s = strdup("Canary2 is changed, maybe some array went beyond the boundaries and poison stack!");
        break;
    case EHASH:
        s = strdup("Hash is changed, our stack is poison!");
        break;
    case EBUFEMPTY:
        s = strdup("Stack is a NULL pointer!");
        break;
    case ENOTENMEM:
        s = strdup("Can't reserve memory for stack!");
        break;
    case EOPFILEERR:
        s = strdup("Error open file to print errors!");
        break;
    case EOPFILECH:
        s = strdup("Error open file to print stack!");
        break;
    case BUFFOVERFLOW:
        s = strdup("Too large buffer size!");
        break;
    case EUNDERFLOW:
        s = strdup("Stack underflow!");
        break;
    default:
        s = strdup("There is extra error!");
        break;
    }
    errno = stk.err;
    fprintf(stk.ferr, "ERROR %d: %s\n", stk.err, s);
    free(s);
    return 0;
}

int StackCheck(stack *stk) {
    MyAssert(stk);
    int flag = 0;
    if(stk->hash != Hash(*stk)) {
        stk->err = EHASH;
        PrintError(*stk);
        flag = 1;
    }
    if(stk->size > stk->buff) {
        stk->err = ENOTENMEM;
        PrintError(*stk);
        flag = 1;
    }
    if(stk->canary2 != CANARY2_CONST) {
        stk->err = ECANARY2;
        PrintError(*stk);
        flag = 1;
    }
    if(stk->canary1 != CANARY1_CONST) {
        stk->err = ECANARY1;
        PrintError(*stk);
        flag = 1;
    }
    if(stk->canary1 != CANARY1_CONST && stk->canary2 != CANARY2_CONST) {
        stk->err = ECANARYBOTH;
        PrintError(*stk);
        flag = 1;
    }
    if(stk->stk == NULL) {
        stk->err = EBUFEMPTY;
        PrintError(*stk);
        flag = 1;
    }
    if(stk->err != 0 && flag == 0) PrintError(*stk);
    return stk->err;
}

int PrintSAE(stack* stk) {
    MyAssert(stk);
    StackPrint(stk);
    PrintError(*stk);
    return 0;
}

int IncreaseBuff(stack *stk, ui a) {
    MyAssert(stk);
    if(stk->buff + a < stk->buff) return -1;
    stk_t* sup = (stk_t*)realloc(stk->stk, (stk->buff + a) * sizeof(stk_t));
    if(sup == NULL)
        return ENOTENMEM;
    else
        stk->stk = sup;
    return 0;
}

int MyAssert(void* p) {
    if(p == NULL) {
        printf("ERROR: NULL pointer in function!\n");
        exit(-1);
    }
    return 0;
}
