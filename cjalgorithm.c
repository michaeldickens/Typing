/*
 *  cjalgorithm.m
 *  Typing
 *
 *  Created by Chris Johnson and Michael Dickens.
 *
 */

#include "cjalgorithm.h"
 
/* Contains the full-size keyboard but if you're only using the main 30 
 * keys then the extra indices may as well not exist.
 */
int indices[] = {
	 0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 
	11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 
	22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 
	33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 
	44, 45,                         52, 53, 
};

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
	2, 3, 4, 5, 6, 7, 8, 9,10,11, 1, 
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
};

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
	if (KEEP_NUMBERS) return bigLegalBox1[i] == bigLegalBox1[j];
	return TRUE;
}

long long improveLayout(long long evaluationToBeat, Keyboard *k)
{
	long long evaluation;
	int i, j;
	
	/* try swaps until we beat evaluationToBeat... */
	for (i = 0; i < KSIZE; i++) {
		for (j = i+1; j < KSIZE; j++) {
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
	int temp, k, n = KSIZE;

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
	int k, n = KSIZE;
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

