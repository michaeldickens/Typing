/*
 *  cjalgorithm.m
 *  Typing
 *
 *  Created by Chris Johnson and Michael Dickens.
 *
 */

#include "cjalgorithm.h"
 

int legalBox1[] = {
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
	2, 2, 2, 2, 1, 1, 2, 2, 2, 2, 
	3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 
};

int legalBox2[] = {
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
	2, 2, 2, 2, 1, 1, 2, 2, 2, 2, 
	4, 4, 3, 3, 3, 3, 3, 3, 4, 4, 
};

int legalBox3[] = {
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
	2, 2, 2, 2, 1, 1, 2, 2, 2, 2, 
	3, 3, 4, 4, 3, 3, 4, 4, 3, 3, 
};

int bigLegalBox1[] = {
	1, 2, 3, 4, 5, 6, 7, 8, 9,10,11, 1, 1, 1, 
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
};

int kinesisLegalBox1[] = {
	2, 3, 4, 5, 6, 7, 8, 9,10,11, 1, 
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
};

int run(char *filename)
{
	int start = time(NULL), finish;

	long long curEval;
	Keyboard k, bestk;
	long long bestEval = LLONG_MAX;

	int i, numberOfRounds, isFileEmpty;
	
	FILE *fp = fopen(filename, "r");
	
	// The simulated annealing algorithm is seeded with either a completely random 
	// layout or a mutated version of the last layout found so far. The probabilty 
	// of using a mutated last layout is chanceToUsePreviousLayout.
	double chanceToUsePreviousLayout = 0.2;
	double subChanceToUseBestLayout = 0.1;
	int numberOfSwaps = 1;
	int roundsBeforeChanceInc = 100, roundsBeforeSwapInc = 50;

	int usedPreviousLayout = FALSE;
	int intervalBetweenPrints = 60, intervalInc = 0;
		
	// Run Chris Johnson's simulated annealing algorithm.
	isFileEmpty = FALSE;
	for (i = 0, numberOfRounds = 0; i < SIM_ANNEAL_GENERATIONS; ++i, ++numberOfRounds) {
		/* chanceToUsePreviousLayout increases as the program continues for longer and longer. */
		if (numberOfRounds % roundsBeforeChanceInc == roundsBeforeChanceInc - 1) {
			chanceToUsePreviousLayout *= 1.2;
			roundsBeforeChanceInc *= 2;
			if (detailedOutput) printf("Chance to use previous layout is now %f.\n", chanceToUsePreviousLayout);
		}
		
		if (numberOfRounds % roundsBeforeSwapInc == roundsBeforeSwapInc - 1) {
			++numberOfSwaps;
			roundsBeforeSwapInc *= 2;
			if (detailedOutput) printf("Number of swaps is now %d.\n", numberOfSwaps);
		}
		
		if (INIT_FROM_FILE && !keepNumbers) {
			if (layoutFromFile(fp, &k) == -1) {
				isFileEmpty = TRUE;
				fclose(fp);
			}
		} else isFileEmpty = TRUE;
		
		if (isFileEmpty) {
			if (numberOfRounds != 0 && (double) rand() / RAND_MAX <= chanceToUsePreviousLayout) {
				usedPreviousLayout = TRUE;
				/* There is a 1 out of subChanceToUseBestLayout chance that the best layout will be used instead of the 
				 * last layout. 
				 */
				if ((double) rand() / RAND_MAX <= subChanceToUseBestLayout) copy(&k, &bestk);
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
		
		curEval = doRun(&k);
		
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
			printf("Time elapsed after %d rounds: %d hours, %d minutes, %d seconds\n", numberOfRounds, (finish-start)/3600, ((finish - start)%3600)/60, (finish-start)%60);
		} else if (curEval == bestEval && detailedOutput) {
			printf("Same layout found\n");
		} else if (time(NULL) - finish >= intervalBetweenPrints) {
			finish = time(NULL);
			printf("Time elapsed after %d rounds: %d hours, %d minutes, %d seconds\n", numberOfRounds, (finish-start)/3600, ((finish - start)%3600)/60, (finish-start)%60);
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

long long doRun(Keyboard *k)
{
	long long lastEvaluation, evaluation;
	long long lastImprovement = 0;
	long long evaluationToBeat;
		
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

int isLegalSwap(int i, int j)
{
	if (keepNumbers) {
		if (full_keyboard == FK_STANDARD) return bigLegalBox1[i] == bigLegalBox1[j];
		else if (full_keyboard == FK_KINESIS) return kinesisLegalBox1[i] == kinesisLegalBox1[j];
	}
	return TRUE;
}

int isSwappable(char c)
{
	return !(keepNumbers && c >= '0' && c <= '9') && !(keepParentheses && (c == '(' || c == ')')) && 
			!(keepZXCV && (c == 'z' || c == 'x' || c == 'c' || c == 'v'));
}

long long improveLayout(long long evaluationToBeat, Keyboard *k)
{
	long long evaluation;
	int i, j;
	
	/* try swaps until we beat evaluationToBeat... */
	for (i = 0; i < ksize; i++) {
		for (j = i+1; j < ksize; j++) {
			if (!printIt[i] || !printIt[j] || !isLegalSwap(indices[i], indices[j])) continue;
			if (full_keyboard != FK_NO && ((indices[i] >= 1 && indices[i] <= 9) || (indices[j] >= 1 && indices[j] <= 9))) {
				printf("WARNING\n");
				printf("WARNING\n");
				printf("Swapping [%d]=%c with [%d]=%c.\n", indices[i], k->layout[indices[i]], indices[j], k->layout[indices[j]]);
				printf("WARNING\n");
				printf("WARNING\n");
			}
			
			swapChars(k->layout + indices[i], k->layout + indices[j]);
			calcFitness(k);
			evaluation = k->fitness;

			if (evaluation < evaluationToBeat) {
				/* good swap--keep it */
				return evaluation;
			} else {
				/* bad swap--undo it */
				swapChars(k->layout + indices[i], k->layout + indices[j]);
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
		
		if (isLegalSwap(lc1, lc2)) {
			swapChars(k->layout + lc1, k->layout + lc2);
		}
	}
	
	return 0;
}

void shuffleIndices()
{
	int temp, k, n = ksize;

	while (n > 1) {
		k = rand() % n;
		n--;

		temp = indices[n];
		indices[n] = indices[k];
		indices[k] = temp;
	}
}

void shuffleLayout(char array[])
{
	char temp;
	int k, n = ksize;
	while (n > 1) 
	{
		--n;
		while (printIt[n] == FALSE) --n;
		
		do {
			k = rand() % (n + 1);
		} while (printIt[k] == FALSE);
		temp = array[k];
		array[k] = array[n];
		array[n] = temp;
	}
}

void swapChars(char *a, char *b)
{
	char temp;

	temp = *a;
	*a = *b;
	*b = temp;
}

