/*
 * StackConsts.c
 *
 *  Created on: Oct 19, 2019
 *      Author: artem
 */

#include "StackConsts.h"
#include <errno.h>
#include <stdio.h>

const ui CANARY1_CONST = 0xDE0CAFCA;
const ui CANARY2_CONST = 0xDEADDEAD;
const ui HASH_START = 0x0000000E;
const llu MASK = 0x100000000 - 1;//8589934591; // 2^33 - 1
const ui ODDS = 48;
const stk_t POISON = 0xEDAED0ED;
const stk_t LIMIT = 512;
const int STR_LEN = 32;
const char STR_ERR_FILE[]   = "StkErrs.txt";
const char STR_CHECK_FILE[] = "StkCheck.txt";
