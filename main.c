/*
 * main.c
 *
 *  Created on: Oct 14, 2019
 *      Author: Artem
 */
#include "Stack.h"
//#include "StackOperations.h"

int main() {
    //stack * stk;
    stack stk;
    stk = StackConstruct(20, "StkCheck.txt", "StkErrs.txt");
    //printf("%u\n", Hash(stk));
    stk_t a = 75;
    stk_t b = 9957;
    StackPush(&stk, a);
    for(int i = 0; i < 350; i++) {
        StackPush(&stk, b+i);
        //StackPrint(&stk);
    }
    //printf("a = %d\n", a.field);
    //StackPrint(&stk);
    StackPush(&stk, b);
    //StackPrint(&stk);
    a = StackPop(&stk);
    a = StackPop(&stk);
    printf("a = %d\n", a);
    StackDestruct(&stk);
    return 0;
}
