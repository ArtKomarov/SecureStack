/*
 * Stack.h
 *
 *  Created on: Oct 14, 2019
 *      Author: artem
 */

#ifndef STACK_H_
#define STACK_H_

#include <stdio.h>

typedef unsigned int ui;
typedef unsigned long long int llu;
typedef int stk_t;

typedef struct stack {
    ui canary1;
    ui size;
    ui buff;
    stk_t *stk;
    ui hash;
    FILE* stkprint;
    FILE* ferr;
    ui err;
    ui canary2;
} stack;

stack StackConstruct(ui buff, char* StkCheckFile, char* StkErrFile); //Create stack (allocate memory, set support fields)
ui    Hash          (const stack stk);     //Calculate hash-value of stack stk
int   StackOk       (stack *stk);          //Check canaries, hash, stack memory and set stk.err (return same)
int   StackCheck    (stack *stk);          //Check canaries, hash, stack memory and set stk.err and also print errors to file (return error number)
int   StackPush     (stack *stk, stk_t a); //Push element a to stack stk
stk_t StackPop      (stack *stk);          //Get element from stack stk
int   StackPrint    (stack *stk);          //Print state of stack to file
int   StackDestruct (stack* stk);          //Destruct the stack stk (free memory, set start values to support fields)
int   PrintErrors   (const stack stk);     //Print error explanation of stk.err to file stk.ferr
int   IncreaseBuff  (stack *stk, ui a);    //Increase buffer of stack stk by a value (ret 0 on success)

char* CheckAddress  (void  *p);            //Check address of a pointer (support function)
char* CheckNumber   (stk_t a);             //Check number boundaries (support function)

#endif /* STACK_H_ */
