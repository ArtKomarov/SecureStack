/*
 * Stack.h
 *
 *  Created on: Oct 14, 2019
 *      Author: artem
 */

#ifndef STACK_H_
#define STACK_H_

typedef unsigned int ui;
typedef unsigned long long int llu;

typedef struct stk_t {
    int field;
} stk_t;

typedef struct stack {
    ui canary1;
    ui size;
    ui buff;
    stk_t *stk;
    ui hash;
    ui err;
    ui canary2;
} stack;


//extern FILE * dump_log;

ui    Hash          (const stack stk);
int   StackOk       (const stack stk);
int   StackConstruct(stack *stk);
int   StackPush     (stack *stk, stk_t a);
int   StackPop      (stack *stk, stk_t *a);
int   StackPrint    (stack *stk);
char* CheckAddress  (void  *p);
char* CheckNumber   (stk_t a);
int   StackDestruct (stack* stk);
int   PrintErrors   (int error);

#endif /* STACK_H_ */
