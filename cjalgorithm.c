/*
 *  cjalgorithm.m
 *  Typing
 *
 *  Created by Chris Johnson and Michael Dickens.
 *
 */

#include "cjalgorithm.h"
 

int runCJAlgorithm(char *filename)
{
	int start = time(NULL), finish;
	int i, roundNum, isFileEmpty;
	
	int64_t curEval;
	int64_t bestEval = LLONG_MAX;

	Keyboard k, prevk, bestk;
	
	FILE *fp = fopen(filename, "r");
	
	// The simulated annealing algorithm is seeded with either a completely random 
	// layout or a mutated version of the last layout found so far. The probabilty 
	// of using a mutated last layout is chanceToUsePreviousLayout.
	double chanceToUsePreviousLayout = 0.2;
	double subChanceToUseBestLayout = 0.1;
	int numberOfSwaps = 3;
	int roundsBeforeChanceInc = 100, roundsBeforeSwapInc = 10;

	int usedPreviousLayout = FALSE;
	int intervalBetweenPrints = 60, intervalInc = 0;
	
	/* Run Chris Johnson's simulated annealing algorithm. */
	isFileEmpty = INIT_FROM_FILE ? FALSE : TRUE;
	for (i = 0, roundNum = 0; i < SIM_ANNEAL_GENERATIONS; ++i, ++roundNum) {
		copy(&prevk, &k);

		/* chanceToUsePreviousLayout increases as the program continues for 
		   longer and longer.
		 */
		if (roundNum % roundsBeforeChanceInc == roundsBeforeChanceInc - 1) {
			chanceToUsePreviousLayout *= 1.2;
			roundsBeforeChanceInc = roundsBeforeChanceInc * 2 + 1;
			if (detailedOutput) printf("Chance to use previous layout is now %f.\n", chanceToUsePreviousLayout);
		}
		
		if (roundNum % roundsBeforeSwapInc == roundsBeforeSwapInc - 1) {
			++numberOfSwaps;
			roundsBeforeSwapInc = roundsBeforeSwapInc * 1.4 + 1;
			if (detailedOutput) printf("Number of swaps between rounds is now %d.\n", numberOfSwaps);
		}

		int fileReadRes = FILE_READ_NOT_HAPPEN;
		if (INIT_FROM_FILE && !isFileEmpty) {
			if ((fileReadRes = layoutFromFile(fp, &k)) != 0) {
				isFileEmpty = TRUE;
				fclose(fp);
			}
		} 
				
		if (isFileEmpty || fileReadRes == FILE_READ_NOT_HAPPEN) {
			if (roundNum > 0 && (double) rand() / RAND_MAX <= chanceToUsePreviousLayout) {				
				usedPreviousLayout = TRUE;
				/* There is a 1 out of subChanceToUseBestLayout chance that the 
				 * best layout will be used instead of the last layout.
				 */
				if ((double) rand() / RAND_MAX <= subChanceToUseBestLayout) copy(&k, &bestk);
				else copy(&k, &prevk);
				
				smartMutate(&k, numberOfSwaps);
			} else {
				usedPreviousLayout = FALSE;
				initKeyboard(&k);
			}
			
			if (REPEAT_LAYOUTSTORE) {
				fclose(fp);
				fp = fopen(filename, "r");
				isFileEmpty = FALSE;
			}
		}
				
		curEval = anneal(&k);
		
		if (curEval < bestEval) {
			if (usedPreviousLayout && detailedOutput) {
				printf("\nFound from previous layout: \n");
			}
			i = 0;
			bestEval = curEval;
			calcFitnessDirect(&k);
			printPercentages(&k);
			copy(&bestk, &k);

			finish = time(NULL);
			printf("Time elapsed after %d rounds: %d hours, %d minutes, %d seconds\n", roundNum, (finish-start)/3600, ((finish - start)%3600)/60, (finish-start)%60);
		} else if (curEval == bestEval && detailedOutput) {
			printf("Same layout found\n");
		} else if (time(NULL) - finish >= intervalBetweenPrints) {
			finish = time(NULL);
			printf("Time elapsed after %d rounds: %d hours, %d minutes, %d seconds\n", roundNum, (finish-start)/3600, ((finish - start)%3600)/60, (finish-start)%60);
			++intervalInc;
			if (intervalInc >= 4) {
				intervalInc = 0;
				intervalBetweenPrints *= 2;
			}
		}
	}
		
	finish = time(NULL);
	printf("\nTime elapsed: %d hours, %d minutes, %d seconds\n", (finish-start)/3600, ((finish - start)%3600)/60, (finish-start)%60);

    return 0;
}

int64_t anneal(Keyboard *k)
{
	int64_t lastEvaluation, evaluation;
	int64_t lastImprovement = 0;
	int64_t evaluationToBeat;
		
	/* Do the "zeroth" iteration */
	calcFitness(k);
	lastEvaluation = evaluation = k->fitness;
		
	/* Keep doing iterations while the layout is still "improving" */
	do {
		if (evaluation < lastEvaluation) {
			lastImprovement = lastEvaluation - evaluation;
		} else {
			lastImprovement = 0;
		}
		
		lastEvaluation = evaluation;
		shuffleIndices();
		evaluationToBeat = lastEvaluation + lastImprovement;
		evaluation = improveLayout(evaluationToBeat, k);
		
	} while (evaluation < evaluationToBeat);

	return evaluation;
}

int64_t improveLayout(int64_t evaluationToBeat, Keyboard *k)
{
	int64_t evaluation;
	int i, j;
		
	/* try swaps until we beat evaluationToBeat... */
	for (i = 0; i < 2 * trueksize; i++) {
		for (j = i+1; j < 2 * trueksize; j++) {
			
			if (!isLegalSwap(k, indices[i], indices[j])) {
				continue;
			}
			swap(k, indices[i], indices[j]); 
			
			calcFitness(k);
			evaluation = k->fitness;

			if (evaluation < evaluationToBeat) {
				/* good swap--keep it */
				return evaluation;
			} else {
				/* bad swap--undo it */
				swap(k, indices[i], indices[j]);
			}
		}
	}

	/* ...or not */
	return evaluationToBeat;
}

/* 
 * Goes through the list of characters from least to most common. Selects characters to 
 * swap. Less frequent characters are more likely to be swapped.
 * 
 * For the nth character, the probability that it will be mutated is given by 
 *   P(1) = 1 / q
 *   P(n) = (1 / q) * (1 - P(n-1))
 * 
 * Alternatively, 
 *   P(n) = (q-1)^(n-1) / q^n
 * 
 * This holds true for every element but the last. For the last element n, 
 *   P(n) = 1 - (P(1) + P(2) + ... + P(n-1))
 * 
 * These probabilities are only correct if every swap is legal. If not, legal swaps are 
 * somewhat more probable and illegal swaps are of course completely impossible.
 * 
 */
int smartMutate(Keyboard *k, int numberOfSwaps)
{
	int q = 8;
	
	int swapslen = 2 * numberOfSwaps;	
	char swaps[swapslen];
	
	int i, j;
	
	for (j = 0; j < swapslen; ++j) {
		swaps[j] = monKeys[0];

		for (i = monLen - 1; i >= 0; --i) {
			if (isSwappable(monKeys[i]) && rand() % q == 0) {
				swaps[j] = monKeys[i];
				break;
			}
		}
	}
	
	int lc1, lc2;
	for (i = 0; i < swapslen - 1; i += 2) {
		lc1 = loc(k, swaps[i]);
		lc2 = loc(k, swaps[i+1]);
		
		if (isLegalSwap(k, lc1, lc2)) {
			swap(k, lc1, lc2);
		}
	}
	
	return 0;
}

