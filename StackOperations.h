/*
 * StackOperations.h
 *
 *  Created on: Oct 14, 2019
 *      Author: artem
 */

#ifndef STACKOPERATIONS_H_
#define STACKOPERATIONS_H_

#include "Stack.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

enum{
    PUSH,
    POP,
    ADD,
    SUB,
    MUL,
    DIV,
    SQRT,
    SIN,
    COS,
    END
};

int GetCommands(stack *stk);
int Get0arg(char*s);
int Get1arg(char*s, stk_t *elem);
int Get2arg(char*s, stk_t *elem1, stk_t *elem2);

const int limit = 100;
const char *commands[] = {"push", "pop", "add", "sub", "mul", "div", "sqrt", "sin", "cos", "end"};

int GetCommands(stack *stk) {
    char s[limit];
    while(fgets(s, limit, stdin) != NULL) {
        if(s[0] == '\n' || s[0] == '\0') continue;
        char *chr = strchr(s, ' ');
        if(chr == NULL) chr = strchr(s, '\n');
        int i;
        for(i = 0; i < 10; i++) {
            if(strncmp(s, commands[i], chr - s) == 0)
                break;
        }
        if(i == 10) {
            printf("Invalid command!\n");
            continue;
        }
        //StackConstruct(&stk);
        switch(i) {
        case PUSH: {
            stk_t elem;
            int ch = Get1arg(s, &elem);
            if(!ch) continue;
            StackPush(stk, elem);
            StackPrint(stk);
            break;
        }
        case POP: {
            stk_t elem;
            int ch = Get1arg(s, &elem);
            if(!ch) continue;
            StackPop(stk, &elem);
            StackPrint(stk);
            break;

        }
        case ADD: {
            stk_t elem1;
            stk_t elem2;
            int ch = Get0arg(s);
            if(!ch) continue;
            StackPop(stk, &elem1);
            StackPop(stk, &elem2);
            StackPrint(stk);
            elem1.field += elem2.field;
            StackPush(stk, elem1);
            break;
        }
        case SUB: {
            stk_t elem1;
            stk_t elem2;
            int ch = Get0arg(s);
            if(!ch) continue;
            StackPop(stk, &elem1);
            StackPop(stk, &elem2);
            StackPrint(stk);
            elem1.field -= elem2.field;
            StackPush(stk, elem1);
            break;

        }
        }
    }
    return 0;
}

int Get0arg(char*s) {
    char* chr = strchr(s, ' ');
    if(chr != NULL) {
        while(*chr == ' ') chr++;
        if(*chr != '\n' && *chr != '\0'){
            printf("Too many arguments!\n");
            return 0;
        }
    }
    return 1;
}

int Get1arg(char*s, stk_t *elem) {
    char* chr = strchr(s, ' ');
    char* chr2;
    char* arg1;
    if(chr == NULL) {
        printf("Invalid get 1 argument\n");
        return 0;
    }
    while(*chr == ' ') chr++;
    chr2 = strchr(chr, ' ');
    if(chr2 != NULL) {
        arg1 = strndup(chr, chr2 - chr);
        elem->field = atoi(arg1);
        free(arg1);
        while(*chr2 == ' ') chr2++;
        if(*chr2 != '\n' && *chr2 != '\0') {
            printf("Too many arguments!\n");
            return 0;
        }
    }
    else
        elem->field = atoi(chr);
    return 1;
}

int Get2arg(char*s, stk_t *elem1, stk_t *elem2) {
    char* chr = strchr(s, ' ');
    char* chr2;
    char* arg1;
    if(chr == NULL) {
        printf("Invalid get 1 argument\n");
        return 0;
    }
    while(*chr == ' ') chr++;
    chr2 = strchr(chr, ' ');
    if(chr2 != NULL) printf("Not enough arguments!\n");
    arg1 = strndup(chr, chr2 - chr);
    elem1->field = atoi(arg1);
    free(arg1);
    while(*chr2 == ' ') chr2++;
    chr = strchr(chr2, ' ');
    if(chr != NULL) {
        arg1 = strndup(chr2, chr - chr2);
        elem2->field = atoi(arg1);
        free(arg1);
        while(*chr == ' ') chr++;
        if(*chr != '\n' && *chr != '\0') {
            printf("Too many arguments!\n");
            return 0;
        }
    }
    else
        elem2->field = atoi(chr2);



    if(*chr2 != '\n' && *chr2 != '\0') {
        printf("Too many arguments!\n");
        return 0;
    }

    return 1;
}

#endif /* STACKOPERATIONS_H_ */
