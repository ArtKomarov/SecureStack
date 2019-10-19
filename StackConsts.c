/*
 * StackConsts.c
 *
 *  Created on: Oct 19, 2019
 *      Author: artem
 */

#include "StackConsts.h"
#include <errno.h>

const ui c1 = 0xDE0CAFCA;
const ui c2 = 0xDEADDEAD;
const ui h1 = 0x0000000E;
const llu mask = 0x100000000 - 1;//8589934591; // 2^33 - 1
const ui odds = 48;
const int lim = 512;
