/*
 *  cjalgorithm.m
 *  Typing
 *
 *  Created by Chris Johnson and Michael Dickens.
 *
 */

#include <limits.h>
#include "algorithm.h"

#define FILE_READ_NOT_HAPPEN -100
#define FILE_READ_FAIL -101

int runCJAlgorithm(char *filename);
int64_t greatToBest(Keyboard *k);
int tryPermutations(int length, int *p, int index, int *used, int *locs, 
	Keyboard *bestk, Keyboard *origk);

int64_t anneal(Keyboard *k, int lockins[][2], size_t lockin_length);
int64_t improveLayout(int64_t evaluationToBeat, Keyboard *k, 
	int lockins[][2], size_t lockin_length);
int smartMutate(int swapIndices[][2], Keyboard *k, int numberOfSwaps);
