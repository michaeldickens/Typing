/*
 *  values.h
 *  Typing
 *
 *  Created by Michael Dickens on 8/23/09.
 *
 */

#include <ctype.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
 
#define FK_NO 0
#define FK_STANDARD 1
#define FK_KINESIS 2
#define FK_IPHONE 3

#define TRUE 1
#define FALSE 0

/* Set fullKeyboard in initValues(). */
int fullKeyboard;

int ksize, trueksize;
char *kbdFilename;

#define KSIZE_MAX 200
#define FINGER_COUNT 10

#define NOT_WORK_WITH_full_keyboard(str) \
	if (ksize != 30) { \
		fprintf(stderr, "Error: Cannot use %s when keyboard size is not 30.", str); \
		return -1; \
	}

// If TRUE, run through the layouts in layoutstore.txt repeatedly when 
// running the algorithm. If FALSE, only run through once.


// For the following values...
// Peter Klausler used 4096, 50, ?, 50, ?
// Michael Capewell used 2048, 16, 32, N/A (48), 144
// For a thorough run:      64, 16, 1, 1024, 256
// Moderately thorough run: 56, 14, 1,  512, 256
// Fast run:                48, 12, 1,   64, 512



#define SIM_ANNEAL_GENERATIONS INT_MAX



int64_t distanceCosts[KSIZE_MAX];
int64_t  trueDistance[KSIZE_MAX];
int64_t shortcutCosts[KSIZE_MAX];

double fingerPercentMaxes[FINGER_COUNT];
int64_t fingerWorkCosts[FINGER_COUNT];

int detailedOutput;

/* 0 for false, 1 for true. 
 * keepNumbers: 2 if numbers can move about but must stay in number row.
 * 
 * Some of these variables require that their condition be true, and others 
 * merely provide a penalty for not doing so.
 * The Former: keepNumbers, keepShiftPairs, keepTab
 * The Latter: keepZXCV, keepParentheses, keepNumbersShifted
 */
int keepZXCV, keepQWERTY, keepNumbers, keepParentheses, keepShiftPairs, 
	keepTab, keepConsonantsRight, 
	keepNumbersShifted /* for bogboar */;
int zCost, xCost, cCost, vCost, qwertyPosCost, qwertyFingerCost, qwertyHandCost, 
	parenthesesCost, numbersShiftedCost;

int distance, inRoll, outRoll, sameHand, sameFingerP, sameFingerR, sameFingerM, 
	sameFingerI, sameFingerT, rowChangeDown, rowChangeUp, handWarp, handSmooth, 
	homeJump, homeJumpIndex, doubleJump, ringJump, toCenter, toOutside, 
	shiftCost, doubleShiftCost;

int initValues();
void initCosts();
void setksize(int type);
