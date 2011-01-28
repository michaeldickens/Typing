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

long long doRun(Keyboard *keybr);
int isLegalSwap(int i, int j);
int isSwappable(char c);
long long improveLayout(long long evaluationToBeat, Keyboard *keybr);
int smartMutate(Keyboard *k, int numberOfSwaps);
void shuffleIndices();
void shuffleLayout(char array[]);
void swapChars(char *a, char *b);

