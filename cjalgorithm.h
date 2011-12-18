/*
 *  cjalgorithm.m
 *  Typing
 *
 *  Created by Chris Johnson and Michael Dickens.
 *
 */

#include <limits.h>
#include "algorithm.h"

int run(char *filename);

int64_t doRun(Keyboard *keybr);
int isLegalSwap(int i, int j);
int isSwappable(char c);
int64_t improveLayout(int64_t evaluationToBeat, Keyboard *keybr);
int smartMutate(Keyboard *k, int numberOfSwaps);
void shuffleIndices();
void shuffleLayout(char array[]);
void swapChars(char *a, char *b);

