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




/* WARNING: Both k and index are evaluated multiple times.
 */
#define charAt(k, index) ((index) < ksize ? (k)->layout[index] : (k)->shiftedLayout[index])

int initKeyboard(Keyboard *k);
int setLayout(Keyboard *k, char *layout);
int layoutFromFile(FILE *fp, Keyboard *k);
int copy(Keyboard *k, Keyboard *original);
int swap(Keyboard *k, int loc1, int loc2);
int swapPair(Keyboard *k, int loc1, int loc2);

int printLayoutOnly(Keyboard *k);
int printLayoutRaw(char layout[]);
int printPercentages(Keyboard *k);

int charToPrintable(char *buffer, char c, int changeSpace);

int qwertyPositions(Keyboard *k);


int isSwappable(char c);
int isLegalSwap(Keyboard *k, int i, int j);
void shuffleIndices();
void shuffleLayout(Keyboard *kbd);
int loc(Keyboard *k, char c);
int locWithShifted(Keyboard *k, char c);


int calcFitnessDirect(Keyboard *k);
int scoreDigraphDirect(Keyboard *k, char digraph[], int multiplier);
int calcFitness(Keyboard *k);
int scoreDigraph(Keyboard *k, char digraph[], int multiplier, int allLocs[]);




int totalCalcFitness;

int64_t calcShortcuts(Keyboard *k);
int64_t calcQWERTY(Keyboard *k);
int64_t calcParentheses(Keyboard *k);
int64_t calcParensGeneric(Keyboard *k, char openChar, char closeChar);
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

