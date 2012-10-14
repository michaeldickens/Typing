/*
 *  keyboard.h
 *  Typing
 *
 *  Created by Michael Dickens on 8/7/09.
 *
 * Includes the keyboard struct and the fitness function.
 */
 
#include <stdlib.h>
#include <limits.h>
#include <math.h>
#include "tools.h"

/* Takes a Keyboard pointer k and an integer index.
 * 
 * WARNING: Both k and index are evaluated multiple times.
 */
#define charAt(k, index) ((index) < ksize ? (k)->layout[index] : (k)->shiftedLayout[index - ksize])
#define setCharAt(k, index, c) ((index) < ksize ? ((k)->layout[index] = (c)) : ((k)->shiftedLayout[index - ksize] = (c)))

int initKeyboard(Keyboard *k);
int setLayout(Keyboard *k, char *layout);
int layoutFromFile(FILE *file, Keyboard *k);
int copy(Keyboard *k, Keyboard *original);
int swap(Keyboard *k, int loc1, int loc2);
int swapPair(Keyboard *k, int loc1, int loc2);
int printLayoutOnly(Keyboard *k);
int printLayoutRaw(char layout[]);
int printPercentages(Keyboard *k);
int charToPrintable(char *buffer, char c, int changeSpace);

int qwertyPositions(Keyboard *k);
int readLayouts(Keyboard pool[], int length);

int isSwappable(char c);
int isLegalSwap(Keyboard *k, int i, int j);
void shuffleIndices();
void shuffleLayout(Keyboard *kbd);

/* Returns the index of c on either the shifted or unshifted layout. */
int locWithoutShifted(Keyboard *k, char c);

/* Returns the index of c if c is on the unshifted layout, or index + ksize 
 * if c is on the shifted layout.
 */
int locWithShifted(Keyboard *k, char c);

/* To use, set USE_COST_ARRAY to TRUE. */
int64_t allDigraphCosts[900];

int calcFitnessDirect(Keyboard *k);
int scoreDigraphDirect(Keyboard *k, char digraph[], int64_t multiplier);
int calcFitness(Keyboard *k);
int scoreDigraph(Keyboard *k, char digraph[], int64_t multiplier, int allLocs[]);


char shortcutKeys[4];
char seedLayout[KSIZE_MAX]; // Holds every letter and is randomly shuffled. Used to seed the keyboards.
unsigned int seedVal;

int64_t calcShortcuts(Keyboard *k);
int64_t calcQWERTY(Keyboard *k);
int64_t calcBrackets(Keyboard *k);
int64_t calcBracketsGeneric(Keyboard *k, char openChar, char closeChar);
int64_t calcNumbersShifted(Keyboard *k);

int calcFingerWork(Keyboard *k);
int calcInRoll(int loc0, int loc1);
int calcOutRoll(int loc0, int loc1);
int calcSameFinger(int loc0, int loc1);
int calcRowChange(int loc0, int loc1);
int calcHomeJump(int loc0, int loc1);
int calcRingJump(int loc0, int loc1);
int calcToCenter(int loc0, int loc1);
int calcToOutside(int loc0, int loc1);

