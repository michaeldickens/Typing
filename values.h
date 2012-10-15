/*
 *  values.h
 *  Typing
 *
 *  Created by Michael Dickens on 8/23/09.
 *
 */

#include <ctype.h>
#include <math.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifdef SYS_TIME_H
#include <sys/time.h>
#endif
 
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

#define FITNESS_MAX INT64_MAX
#define KSIZE_MAX 200
#define FINGER_COUNT 10

#define NOT_WORK_WITH_FULL_KEYBOARD(str) \
	if (ksize != 30) { \
		fprintf(stderr, "Error: Cannot use %s when keyboard size is not 30.", str); \
		return -1; \
	}

/* If TRUE, run through the layouts in layoutstore.txt repeatedly when 
 * running the algorithm. If FALSE, only run through once.
 */
#define REPEAT_LAYOUTSTORE TRUE

/* If TRUE, scoreDigraph() uses allDigraphCosts to calculate costs. Otherwise, 
 * calculates costs by calling calcX functions.
 */
#define USE_COST_ARRAY FALSE

#define INIT_FROM_FILE FALSE

#define ALGORITHM_ROUNDS 16                     /* recommended 8-32 */
#define CHANCE_TO_USE_PREVIOUS_LAYOUT 0.2       /* recommended 0.2 */
#define RUNS_BEFORE_CHANCE_INC 1                /* recommended 1 */
#define NUM_SWAPS_BETWEEN_ROUNDS (ksize / 15)   /* recommended (ksize / 15) */
#define RUNS_BEFORE_SWAPS_INC 1                 /* recommended 1 */
#define PRINT_TIME_INTERVAL 60

/* Constants for greatToBest(). */
#define GTB_ROUNDS 16                   /* recommended 16-64 */
#define RUNS_BEFORE_GTB_ROUNDS_INC 4    /* recommended 4-20 */
#define GTB_NUMBER_OF_SWAPS 10          /* recommended 10-16 */
#define GTB_ROUNDS_BEFORE_SWAP_INC 64

int64_t distanceCosts[KSIZE_MAX];
int64_t  trueDistance[KSIZE_MAX];
int64_t shortcutCosts[KSIZE_MAX];

double fingerPercentMaxes[FINGER_COUNT];
int64_t fingerWorkCosts[FINGER_COUNT];

int detailedOutput, threadCount;

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
	bracketsCost, numbersShiftedCost;

int distance, inRoll, outRoll, sameHand, sameFingerP, sameFingerR, sameFingerM, 
	sameFingerI, sameFingerT, rowChangeDown, rowChangeUp, handWarp, handSmooth, 
	homeJump, homeJumpIndex, doubleJump, ringJump, toCenter, toOutside, 
	shiftCost, doubleShiftCost;

int initValues();
void initCosts();
void setksize(int type);

/* Defined in fitness.c */
int preCalculateFitness();
