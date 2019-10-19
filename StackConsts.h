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
extern const ui CANARY1_CONST;
extern const ui CANARY2_CONST;
extern const ui HASH_START;
extern const llu MASK;//8589934591; // 2^33 - 1
extern const ui ODDS;
extern const int LIMIT;

#endif /* STACKCONSTS_H_ */
