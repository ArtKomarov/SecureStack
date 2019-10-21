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

stack StackConstruct(ui buff, char* StkCheckFile, char* StkErrFile) { // +argument buff
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
    for(ui i = 0; i < len; i++) { //Check if buff > size
        hash += stk.stk[i] * (i+1);
    }
    hash += stk.buff;
    return hash;
}

int StackOk(stack *stk) {
    assert(stk != NULL);
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
    assert(stk != NULL);
    int check = StackCheck(stk);
    if(check) {
        if(check == EBUFEMPTY){
            stk->err = EBUFEMPTY;
            PrintSAE(stk);
            return EBUFEMPTY;
        }
        else PrintSAE(stk);
    }
    stk_t* sup;
    //stk->size++; //del

    if(stk->size + 1 > stk->buff) {
        if(stk->buff * 2 < stk->size) {
            stk->err = BUFFOVERFLOW;
            //stk->size--;
            PrintSAE(stk);
            return BUFFOVERFLOW;
        }
        stk->buff *= 2;
        sup = (stk_t*)realloc(stk->stk, stk->buff * sizeof(stk_t));
        //stk->stk = (stk_t*)realloc(stk->stk, stk->buff*sizeof(stk_t)); //memory leak possible
        if(sup == NULL) {
            stk->err = ENOTENMEM;
            PrintSAE(stk);
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
    stk->hash = Hash(*stk);
    check = StackCheck(stk);
    if(check) PrintSAE(stk);
    return check;
}

stk_t StackPop(stack *stk) {
    assert(stk != NULL);
    stk_t a;
    if(StackCheck(stk))
        PrintSAE(stk);
    if(stk->size == 0) {
        stk->err = EUNDERFLOW;
        PrintSAE(stk);
        return POISON; //Stack free
    }

    a = stk->stk[--stk->size];

    if(stk->size <= stk->buff/2 - ODDS) {
        stk->buff /= 2;
        stk_t* sup = (stk_t*)realloc(stk->stk, stk->buff); //поправь
        if(sup == NULL) {
            stk->err = ENOTENMEM;
            PrintSAE(stk);
            return a;
        }
        stk->stk = sup;
    }

    stk->hash = Hash(*stk);
    if(StackCheck(stk))
        PrintSAE(stk);
    return a;
}

int StackPrint(stack* stk) {
    assert(stk != NULL);
    if(stk->stkprint == NULL)
        return 0;
    StackCheck(stk);
    /*FILE* fw = fopen("StkCheck.txt", "a");
    if(fw == NULL) {
        perror("fopen");
        return ENFILE;
    }*/

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
    //fclose(fw);
    return 0;
}

char* CheckAddress(void* p) {//address
    assert(p != NULL);
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
    assert(stk != NULL);
    stk->canary1 = CANARY1_CONST;
    stk->size    = 0;
    stk->buff    = 0;
    if(stk->stk)
        free(stk->stk);
    stk->stk     = NULL;
    stk->hash    = HASH_START;
    if(stk->ferr)
        fclose(stk->ferr);
    stk->ferr = NULL;
    if(stk->stkprint)
        fclose(stk->stkprint);
    stk->stkprint = NULL;
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
        //errno = EADDRINUSE;
        break;
    case ECANARY1:
        s = strdup("Canary1 is changed, maybe some array went beyond the boundaries and poison stack!");
        //errno = EADDRINUSE;
        break;
    case ECANARY2:
        s = strdup("Canary2 is changed, maybe some array went beyond the boundaries and poison stack!");
        //errno = EADDRINUSE;
        break;
    case EHASH:
        s = strdup("Hash is changed, our stack is poison!");
        //errno = EADDRINUSE;
        break;
    case EBUFEMPTY:
        s = strdup("Stack is a NULL pointer!");
        break;
    case ENOTENMEM:
        s = strdup("Can't reserve memory for stack!");
        //errno = ENOMEM;
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
    //fclose(fwe);
    return 0;
}

int StackCheck(stack *stk) {
    assert(stk != NULL);
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
    //StackPrint(stk);
    return stk->err;
}

int PrintSAE(stack* stk) {
    StackPrint(stk);
    PrintError(*stk);
    return 0;
}

int IncreaseBuff(stack *stk, ui a) {
    assert(stk != NULL);
    if(stk->buff + a < stk->buff) return -1;
    stk_t* sup = (stk_t*)realloc(stk->stk, (stk->buff + a) * sizeof(stk_t));
    if(sup == NULL)
        return ENOTENMEM;
    else
        stk->stk = sup;
    return 0;
}
