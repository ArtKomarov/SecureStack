/*
 * Stack.h
 *
 *  Created on: Oct 14, 2019
 *      Author: artem
 */

#ifndef STACK_H_
#define STACK_H_

#include <stdio.h>
#include "StackConsts.h"

typedef struct stack { //Structure stack
    ui canary1;     //Guard 1
    ui size;        //Current number of elements in stack
    ui buff;        //Memory size for stack
    stk_t *stk;     //Stack array
    ui hash;        //Stack hash
    FILE* stkprint; //File pointer to print stack
    FILE* ferr;     //File pointer to print errors in stack
    ui err;         //Current error number
    ui canary2;     //Guard 2
} stack;

stack StackConstruct(ui buff, char* StkCheckFile, char* StkErrFile); //Create stack (allocate memory, set support fields)
int   StackPush     (stack *stk, stk_t a); //Push element a to stack stk
stk_t StackPop      (stack *stk);          //Get element from stack stk
int   StackDestruct (stack* stk);          //Destruct the stack stk (free memory, set start values to support fields)
ui    Hash          (const stack stk);     //Calculate hash-value of stack stk
int   StackOk       (stack *stk);          //Check canaries, hash, stack memory and set stk.err (return same)
int   StackCheck    (stack *stk);          //Check canaries, hash, stack memory and set stk.err and also print errors to file (return error number)
int   StackPrint    (stack *stk);          //Print state of stack to file
int   PrintError    (const stack stk);     //Print error explanation of stk.err to file stk.ferr
int   PrintSAE      (stack* stk);          //Call functions StackPrint(stk) and PrintError(*stk)
int   IncreaseBuff  (stack *stk, ui a);    //Increase buffer of stack stk by a value (ret 0 on success)

char* CheckAddress  (void  *p);            //Check address of a pointer (support function)
char* CheckNumber   (stk_t a);             //Check number boundaries (support function)
int   MyAssert      (void* p);             //If p is nulptr, prints message to stdout and exit(-1) process

#endif /* STACK_H_ */
