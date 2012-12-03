/*
 *  cjalgorithm.m
 *  Typing
 *
 *  Created by Chris Johnson and Michael Dickens.
 *
 */

#include <limits.h>
#include "keyboard.h"

#define FILE_READ_NOT_HAPPEN -100

struct ThreadArg {
	Keyboard bestk;
    
	int numRounds;
    double chanceToUsePreviousLayout;
    int numberOfSwaps;
    
	time_t startTime;
	
	/* Indicates that the subroutine should keep creating new threads until 
	 * this reaches 0.
	 */
	int threadCount;
	
	/* Indicates whether the current thread is done running. */
	int isFinished;
};

void runAlgorithm();
void * runThreadsRec(void *arg);

void greatToBest(Keyboard *k, int numRounds);
void * greatToBestThreadRec(void *arg);
void greatToBestBruteForce(Keyboard *k);

void tryPermutations(Keyboard *bestk, Keyboard *k, int *origLocs, int *locs,
                    int length, int index);

int64_t anneal(Keyboard *k, int lockins[][2], size_t lockin_length);
int64_t improveLayout(int64_t evaluationToBeat, Keyboard *k, 
	int lockins[][2], size_t lockin_length);
int smartMutate(int swapIndices[][2], Keyboard *k, int numberOfSwaps);

void initThreadArg(struct ThreadArg *arg);
void copyThreadArg(struct ThreadArg *dest, struct ThreadArg *src);