/*
 *  charsets.h
 *  Typing
 *
 *  Created by Michael Dickens on 8/7/09.
 *
 */

#include "keystroke.h" 
#include "values.h"

#define streq(str1, str2) (strcmp(str1, str2) == 0)
#define streqn(str1, str2, n) (strncmp(str1, str2, n) == 0)

#define ASCII_SHIFT 14

#define ERROR_RATE_PERCENT 2

int64_t totalMon;
int64_t totalDi;

/* Constant definitions */

#define LEFT 0
#define RIGHT 1

#define PINKY 0
#define RING 1
#define MIDDLE 2
#define INDEX 3
#define THUMB 4

/* Reduces monValues and diValues so as to prevent integer overflow. */
#define DIVISOR 100

/* These are guaranteed to hold a standard QWERTY layout. */

#define DEFAULT_KEYBOARD_30 "qwertyuiopasdfghjkl;zxcvbnm,./QWERTYUIOPASDFGHJKL:ZXCVBNM<>?"
#define DEFAULT_KEYBOARD_STANDARD "`1234567890-=qwertyuiop[]\\asdfghjkl;'zxcvbnm,./~!@#$%^&*()_+QWERTYUIOP{}|ASDFGHJKL:\"ZXCVBNM<>?"
#define DEFAULT_KEYBOARD_KINESIS "1234567890-\tqwertyuiop=asdfghjkl;'zxcvbnm,./`\\[]\n !@#$%^&*()_\tQWERTYUIOP+ASDFGHJKL:\"ZXCVBNM<>?~|{}\n "

char keysToInclude[200];

typedef struct {
	char layout[KSIZE_MAX + 1]; /* The one extra character is set to '\0' so 
								(layout) can be treated as a string. */
	char shiftedLayout[KSIZE_MAX + 1];
	int64_t fingerUsage[FINGER_COUNT];
	int64_t fitness;
	int64_t distance;
	int64_t fingerWork;
	int64_t inRoll;
	int64_t outRoll;
	int64_t sameHand;
	int64_t sameFinger;
	int64_t rowChange;
	int64_t homeJump;
	int64_t ringJump;
	int64_t toCenter;
	int64_t toOutside;
} Keyboard;



Keyboard nilKeyboard;

/* These are declared here because C's file hierarchy is stupid.
 */



void copyArray(int out[], const int in[], const int length);
int initData();
void initKeyboardData();
void initTypingData();
void internalError( int code );
int compileTypingData(char *outfileName, char *const filenames[], const int multipliers[], const size_t length, const int unit, const size_t max);
char convertEscapeChar(char c);

int setValue(char *str);
int getValue(char *name);

int alwaysKeepShiftPairP(char c);


int qwerty[30];

/* For each key, indicates which hand (LEFT or RIGHT) is responsible for typing that key.
 */
int hand[KSIZE_MAX];

/* For each key, indicates which finger (PINKY, RING, MIDDLE or INDEX) is responsible for 
 * typing that key.
 */
int finger[KSIZE_MAX];

/* Where 0 is pinky, -1 is left of pinky, ..., 3 is index, 4 is right of index.
 * Anything greater than 4 is thumb.
 */
int column[KSIZE_MAX];

/* For each key, indicates which row that key lies on. The top row is 0, the row below it 
 * is 1, the row below that is 2, etc.
 */
int row[KSIZE_MAX];

/* Indicates which row is the home row.
 */
int homeRow;

/* Indicates the index of the first number in the layout, assuming keepNumbers == TRUE.
 */
int numStart;

/* For each key, indicates whether that key requires a reach to the center.
 */
int isCenter[KSIZE_MAX];

/* For each key, indicates whether that key requires a reach to the outside.
 */
int isOutside[KSIZE_MAX];

/* For each key, indicates whether that key requires a reach to the center OR a reach to 
 * the outside. This one is produced automatically from isCenter[] and isOutside[].
 */
int isCenterOrOutside[KSIZE_MAX];

/* For each key, indicates whether that key should be printed. Any place-holder key that 
 * does not actually exist on the keyboard should not be printed.
 */
int printable[KSIZE_MAX];

/* Lookup tables for calcRowChange(). Each row and column represents a finger.
 */
int rowChangeTableUp[5][5];
int rowChangeTableDown[5][5];

extern KeystrokeValueTable *digraphs;
extern KeystrokeValueTable *monographs;

/* Used in cjalgorithm.c. */
int indices[2 * KSIZE_MAX];
