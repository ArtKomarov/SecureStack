/*
 * Source.c
 *
 *  Created on: Oct 14, 2019
 *      Author: artem
 */
#include "Stack.h"
#include "StackOperations.h"

int main() {
    //stack * stk;
    stack stk;
    StackConstruct(&stk);
    //printf("%u\n", Hash(stk));
    stk_t a = {75};
    stk_t b = {9957};
    //GetCommands(&stk);
    //StackPush(&stk, a);
    //StackPush(&stk, b);
    //printf("a = %d\n", a.field);
    //StackPrint(&stk);
    //StackPop(&stk, &a);
    //printf("a = %d\n", a.field);
    StackDestruct(&stk);
    return 0;
}
