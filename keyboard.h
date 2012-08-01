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
int copy(Keyboard *const k, const Keyboard *const original);
int swap(Keyboard *k, int loc1, int loc2);
int swapPair(Keyboard *k, int loc1, int loc2);

int printLayoutOnly(const Keyboard *const k);
int printLayoutRaw(const char layout[]);
int printPercentages(const Keyboard *const k);

int charToPrintable(char *buffer, char c, int changeSpace);

int qwertyPositions(const Keyboard *const k);


int isSwappable(char c);
int isLegalSwap(const Keyboard *const k, int i, int j);
void shuffleIndices();
void shuffleLayout(Keyboard *kbd);
int loc(const Keyboard *const k, const char c);
int locWithShifted(const Keyboard *const k, const char c);


int calcFitnessDirect(Keyboard *k);
int scoreDigraphDirect(Keyboard *const k, const char digraph[], const int multiplier);
int calcFitness(Keyboard *k);
int scoreDigraph(Keyboard *const k, const char digraph[], const int64_t multiplier, const int allLocs[]);






int64_t calcShortcuts(const Keyboard *k);
int64_t calcQWERTY(const Keyboard *k);
int64_t calcParentheses(const Keyboard *const k);
int64_t calcParensGeneric(const Keyboard *const k, char openChar, char closeChar);
int64_t calcNumbersShifted(const Keyboard *const k);

int calcFingerWork(Keyboard *k);
int calcInRoll(int loc0, int loc1);
int calcOutRoll(int loc0, int loc1);
int calcSameFinger(int loc0, int loc1);
int calcRowChange(int loc0, int loc1);
int calcHomeJump(int loc0, int loc1);
int calcRingJump(int loc0, int loc1);
int calcToCenter(int loc0, int loc1);
int calcToOutside(int loc0, int loc1);

