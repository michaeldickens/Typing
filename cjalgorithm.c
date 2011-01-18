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
	1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12, 1, 1, 
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
	Keyboard k;
	long long bestEval = LLONG_MAX;

	int i, numberOfRounds, isFileEmpty;
	
	FILE *fp = fopen(filename, "r");
	
	int usedLastLayout = FALSE;
	int intervalBetweenPrints = 60, intervalInc = 0;
		
	// Run Chris Johnson's simulated annealing algorithm.
	isFileEmpty = FALSE;
	for (i = 0, numberOfRounds = 0; i < SIM_ANNEAL_GENERATIONS; ++i, ++numberOfRounds) {
		if (INIT_FROM_FILE && !keepNumbers) {
			if (layoutFromFile(fp, &k) == -1) {
				isFileEmpty = TRUE;
				fclose(fp);
			}
		} else isFileEmpty = TRUE;
		
		if (isFileEmpty) {
			if (numberOfRounds != 0 && (double) rand() / RAND_MAX <= CHANCE_TO_USE_LAST_LAYOUT) {
				usedLastLayout = TRUE;
				int j;
				for (j = 0; j < LAST_LAYOUT_MUTATIONS; ++j)
					k = mutate(k);
			} else {
				usedLastLayout = FALSE;
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
			if (usedLastLayout) {
				printf("\nEvolved from last layout: \n");
			}
			i = 0;
			bestEval = curEval;
			calcFitnessDirect(&k);
			printPercentages(&k);

			finish = time(NULL);
			printf("\nTime elapsed: %d hours, %d minutes, %d seconds\n", (finish-start)/3600, ((finish - start)%3600)/60, (finish-start)%60);
		} else if (curEval == bestEval) {
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
	if (keepNumbers) 
		if (full_keyboard == FK_STANDARD) return bigLegalBox1[i] == bigLegalBox1[j];
		else if (full_keyboard == FK_KINESIS) return kinesisLegalBox1[i] == kinesisLegalBox1[j];
	return TRUE;
}

long long improveLayout(long long evaluationToBeat, Keyboard *k)
{
	long long evaluation;
	int i, j;
	
	/* try swaps until we beat evaluationToBeat... */
	for (i = 0; i < ksize; i++) {
		for (j = i+1; j < ksize; j++) {
			if (isLegalSwap(indices[i], indices[j]) == FALSE) continue;
			
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

