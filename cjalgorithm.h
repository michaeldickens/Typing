/*
 *  cjalgorithm.m
 *  Typing
 *
 *  Created by Chris Johnson and Michael Dickens.
 *
 */

#include <limits.h>
#include "accessories.h"

int run(char *filename);

long long doRun(Keyboard *keybr);
long long improveLayout(long long evaluationToBeat, Keyboard *keybr);
void shuffleIndices();
void shuffleLayout(char array[]);
void swapChars(char *a, char *b);

