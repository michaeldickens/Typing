/*
 *  values.h
 *  Typing
 *
 *  Created by Michael Dickens on 8/23/09.
 *
 */

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

/* Set full_keyboard in initValues(). */
int full_keyboard;

int ksize, trueksize;
char *kbd_filename;

#define KSIZE_MAX 100
#define FINGER_COUNT 8

#define NOT_WORK_WITH_full_keyboard(str) \
	if (ksize != 30) { \
		fprintf(stderr, "Error: Cannot use %s when keyboard size is not 30.", str); \
		return -1; \
	}

#define NOT_WORK_WITH_30_KEYBOARD(str) \
	if (ksize == 30) { \
		fprintf(stderr, "Error: Cannot use %s when keyboard size is 30.", str); \
		return -1; \
	}

// If TRUE, run through the layouts in layoutstore.txt repeatedly when 
// running the algorithm. If FALSE, only run through once.
#define REPEAT_LAYOUTSTORE 1 // 1 = TRUE, 0 = FALSE

// For the following values...
// Peter Klausler used 4096, 50, ?, 50, ?
// Michael Capewell used 2048, 16, 32, N/A (48), 144
// For a thorough run:      64, 16, 1, 1024, 256
// Moderately thorough run: 56, 14, 1,  512, 256
// Fast run:                48, 12, 1,   64, 512
#define POOL_SIZE             56
#define GENERATIONS           14
#define TAKE_BEST_N            1
#define ALL_STAR_POOL_SIZE   512
#define ALL_STAR_GENERATIONS 256

#define SIM_ANNEAL_GENERATIONS INT_MAX
#define IMPROVER_GENERATIONS 256

#define INIT_FROM_FILE TRUE

#define Z_COST 10
#define X_COST  6
#define C_COST 12
#define V_COST 14
#define QWERTY_POS_COST		14
#define QWERTY_FINGER_COST	4
#define QWERTY_HAND_COST	20
#define PARENTHESES_COST	5000000000 // warning: this will cause overflow

int64_t distanceCosts[KSIZE_MAX];
int64_t  trueDistance[KSIZE_MAX];
int64_t shortcutCosts[KSIZE_MAX];

double fingerPercentMaxes[FINGER_COUNT];
int64_t fingerWorkCosts[FINGER_COUNT];

int detailedOutput;

/* 0 for false, 1 for true. 
 * keepNumbers: 2 if numbers can move about but must stay in number row.
 */
int keepZXCV, keepQWERTY, keepNumbers, keepParentheses;

int distance, inRoll, outRoll, sameHand, sameFingerP, sameFingerR, sameFingerM, 
	sameFingerI, sameFingerT, rowChangeDown, rowChangeUp, handWarp, handSmooth, 
	homeJump, homeJumpIndex, doubleJump, ringJump, toCenter, toOutside;

int initValues();
void initCosts();
void setksize(int type);
