/*
 * StackConsts.h
 *
 *  Created on: Oct 19, 2019
 *      Author: artem
 */

#ifndef STACKCONSTS_H_
#define STACKCONSTS_H_

typedef unsigned int ui;
typedef unsigned long long int llu;
typedef ui stk_t;

extern const ui CANARY1_CONST;
extern const ui CANARY2_CONST;
extern const ui HASH_START;
extern const llu MASK;//8589934591; // 2^33 - 1
extern const ui ODDS;
extern const ui POISON;
extern const stk_t LIMIT;
extern const int STR_LEN;
extern const char STR_ERR_FILE[];
extern const char STR_CHECK_FILE[];

#endif /* STACKCONSTS_H_ */
