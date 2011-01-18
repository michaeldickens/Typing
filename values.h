/*
 *  values.h
 *  Typing
 *
 *  Created by Michael Dickens on 8/23/09.
 *
 */
 
#define TRUE 1
#define FALSE 0

#define FK_NO 0
#define FK_STANDARD 1
#define FK_KINESIS 2

/* Set full_keyboard in initValues(). */
int full_keyboard, ksize;

#define KSIZE_MAX 100

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
#define IMPROVER_GENERATIONS 32

// The simulated annealing algorithm is seeded with either a completely random 
// layout or a mutated version of the last layout found so far. The probabilty 
// of using a mutated last layout is CHANCE_TO_USE_LAST_LAYOUT.
#define CHANCE_TO_USE_LAST_LAYOUT (1.0 / 3)
#define LAST_LAYOUT_MUTATIONS 4 // Number of mutations to the last layout.

#define KEEP_ZXCV   0 // 0 = FALSE, 1 = TRUE
#define KEEP_QWERTY 0
#define KEEP_NUMBERS 1
#define KEEP_PARENTHESES 1

#define INIT_FROM_FILE 1

// All values are compounding. For example, say we jump over the home row on the 
// index finger. The cost is sameHand + rowChange + homeJump + homeJumpIndex.
#define DISTANCE		  1
#define IN_ROLL			-40
#define OUT_ROLL		  5
#define SAME_HAND		  5
#define SAME_FINGER_P	150
#define SAME_FINGER_R	140
#define SAME_FINGER_M	110
#define SAME_FINGER_I	 90
#define ROW_CHANGE		 15
#define HAND_WARP		 10
#define HAND_SMOOTH		- 0
#define HOME_JUMP		100
#define HOME_JUMP_INDEX	-90
#define DOUBLE_JUMP		220 // Does not compound with HOME_JUMP.
#define TO_CENTER		 20
#define TO_OUTSIDE		 20

#define SHORTCUT		100
#define Z_COST 10
#define X_COST  6
#define C_COST 12
#define V_COST 14
#define QWERTY_POS_COST		5
#define QWERTY_FINGER_COST	2
#define QWERTY_HAND_COST	2
#define PARENTHESES_COST	5000000000

long long fingerCosts[5];
long long distanceCosts[KSIZE_MAX];
long long  trueDistance[KSIZE_MAX];
long long shortcutCosts[KSIZE_MAX];

int distance, inRoll, outRoll, sameHand, sameFingerP, sameFingerR, sameFingerM, 
	sameFingerI, rowChange, handWarp, handSmooth, homeJump, homeJumpIndex, 
	doubleJump, toCenter, toOutside;

int initValues();
void setksize(int type);
