/*
 *  values.h
 *  Typing
 *
 *  Created by Michael Dickens on 8/23/09.
 *
 */

#ifndef __VALUES_H__
#define __VALUES_H__

#include <ctype.h>
#include <math.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* If you have <sys/time.h>, you may un-comment the line below. */
//#define __SYS_TIME_H__

#ifdef SYS_TIME_H
#include <sys/time.h>
#endif
 
#define K_NO 0
#define K_STANDARD 1
#define K_KINESIS 2
#define K_IPHONE 3

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

/* If TRUE, scoreDigraph() uses allDigraphCosts to calculate costs. Otherwise, 
 * calculates costs by calling calcX functions.
 */
#define USE_COST_ARRAY TRUE

#define INIT_FROM_FILE FALSE

#define MAX_RUNS INT_MAX
#define ALGORITHM_ROUNDS 16                     /* recommended 8-32 */
#define CHANCE_TO_USE_PREVIOUS_LAYOUT 0.2       /* recommended 0.2 */
#define CHANCE_EXPONENTIATOR 0.9                /* recommended 0.7-1 */
#define RUNS_BEFORE_CHANCE_INC 1                /* recommended 1 */
#define NUM_SWAPS_BETWEEN_ROUNDS (ksize / 15)   /* recommended (ksize / 15) */
#define RUNS_BEFORE_SWAPS_INC 1                 /* recommended 1 */
#define PRINT_TIME_INTERVAL 60

/* Constants for greatToBest(). */
#define GTB_ROUNDS 4                    /* recommended 4-64 */
#define GTB_ROUNDS_MULTIPLIER 2         /* recommended 1.0-2.0 */  
#define RUNS_BEFORE_GTB_ROUNDS_INC 4    /* recommended 4-64 */
#define GTB_NUMBER_OF_SWAPS 10          /* recommended 10-16 */
#define GTB_ROUNDS_BEFORE_SWAP_INC 32   /* recommended ~64 */
#define CHANCE_TO_USE_BRUTE_FORCE 0.25
#define GTBBF_ROUNDS 8                  /* recommended 4-10 */

int64_t distanceCosts[KSIZE_MAX];
int64_t shortcutCosts[KSIZE_MAX];

double fingerPercentMaxes[FINGER_COUNT];
int64_t fingerWorkCosts[FINGER_COUNT];

int detailedOutput, numThreads;

/* 0 for false, 1 for true. 
 * keepNumbers: 2 if numbers can move about but must stay in number row.
 * 
 * Some of these variables require that their condition be true, and others 
 * merely provide a penalty for not doing so.
 * The Former: keepNumbers, keepShiftPairs, keepTab
 * The Latter: keepZXCV, keepBrackets, keepNumbersShifted
 */
int keepZXCV, keepQWERTY, keepNumbers, keepBrackets, keepShiftPairs, 
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

#endif
