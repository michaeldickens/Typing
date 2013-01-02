/*
 *  cjalgorithm.m
 *  Typing
 *
 *  Created by Chris Johnson and Michael Dickens.
 *
 */

#include "cjalgorithm.h"
#include "keystroke.h" 

int runCJAlgorithm(char *const filename)
{
	const time_t start = time(NULL);
    time_t finish = 0;
	int i, roundNum, isFileEmpty;
	
	int64_t curEval;
	int64_t bestEval = LLONG_MAX;

	Keyboard k = nilKeyboard;
	Keyboard prevk;
	Keyboard bestk = nilKeyboard;
	
	FILE *fp = fopen(filename, "r");

	if( !fp )
		{
		printf("Unable to open file: %s\n", filename);
		return 0;
		}
	
	// The simulated annealing algorithm is seeded with either a completely random 
	// layout or a mutated version of the last layout found so far. The probabilty 
	// of using a mutated last layout is chanceToUsePreviousLayout.
	double chanceToUsePreviousLayout = 0.2;
	const double subChanceToUseBestLayout = 0.1;
	int numberOfSwaps = 3;
	int roundsBeforeChanceInc = 100, roundsBeforeSwapInc = 10;

	int usedPreviousLayout = FALSE;
	int intervalBetweenPrints = 60, intervalInc = 0;
	
	/* Run Chris Johnson's simulated annealing algorithm. */
	isFileEmpty = FALSE;
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
			roundsBeforeSwapInc = (int) (roundsBeforeSwapInc * 1.4) + 1;
			if (detailedOutput) printf("Number of swaps between rounds is now %d.\n", numberOfSwaps);
		}

		int fileReadRes = FILE_READ_NOT_HAPPEN;
		if (!isFileEmpty) {
			if ((fileReadRes = layoutFromFile(fp, &k)) != 0) {
				isFileEmpty = TRUE;
				fclose(fp);
				fp = NULL;
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
			
			if( fp )
				fclose(fp);
			fp = fopen(filename, "r");
			if( !fp ) {
				printf("Unable to re-open file: %s\n", filename);
				return 0;
			}
			isFileEmpty = FALSE;
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
			printf("Time elapsed after %d rounds: %ld hours, %ld minutes, %ld seconds\n", roundNum, (finish-start)/3600, ((finish - start)%3600)/60, (finish-start)%60);
		} else if (curEval == bestEval && detailedOutput) {
			printf("Same layout found\n");
		} else if (time(NULL) - finish >= intervalBetweenPrints) {
			finish = time(NULL);
			printf("Time elapsed after %d rounds: %ld hours, %ld minutes, %ld seconds\n", roundNum, (finish-start)/3600, ((finish - start)%3600)/60, (finish-start)%60);
			++intervalInc;
			if (intervalInc >= 4) {
				intervalInc = 0;
				intervalBetweenPrints *= 2;
			}
		}
	}
		
	finish = time(NULL);
	printf("\nTime elapsed: %ld hours, %ld minutes, %ld seconds\n", (finish-start)/3600, ((finish - start)%3600)/60, (finish-start)%60);

    return 0;
}

int64_t anneal(Keyboard *const k)
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

int64_t improveLayout(const int64_t evaluationToBeat, Keyboard *const k)
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
int smartMutate(Keyboard *const k, const int numberOfSwaps)
{
	const int q = 8;
	
	const int swapslen = 2 * numberOfSwaps;	
	char swaps[swapslen];
	char key;
	int64_t used = monographs->kvt_used;
	int64_t i, j;
	
	for (j = 0; j < swapslen; ++j) {
		swaps[j] = getFirstKeystroke(monographs, 0);

		for (i = used - 1; i >= 0; --i) {
			key = getFirstKeystroke(monographs, i);
			if (isSwappable(key) && rand() % q == 0) {
				swaps[j] = key;
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

