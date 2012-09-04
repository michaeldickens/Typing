/*
 *  algorithm.c
 *  Typing
 *
 *  Created by Michael Dickens on 8/9/09.
 *  
 */

#include "algorithm.h"


Keyboard runAlgorithm()
{
	if (ksize != 30) { \
		fprintf(stderr, "Error: runAlgorithm() does not work when keyboard size is not 30."); \
		return nilKeyboard;
	}

	int i;
	int j;
	
	totalCalcFitness = 0;
	Keyboard allStarPool[ALL_STAR_POOL_SIZE];
	
	Keyboard tempPool[POOL_SIZE];
	int bestScore = 0;
	int offset = 0;
	for (i = 0; i < ALL_STAR_POOL_SIZE / TAKE_BEST_N; ++i) {
		if (TAKE_BEST_N == 1) {
//			initKeyboard(&allStarPool[i]); anneal(&allStarPool[i]);
			allStarPool[i] = runForOne();
		} else {
			runLoop(tempPool);			
			printKeyboard(&tempPool[0]);
			
			for (j = 0; j < TAKE_BEST_N; ++j, ++offset)
				allStarPool[offset] = tempPool[j];
		}
		
		if (bestScore == 0) bestScore = allStarPool[i].fitness;
		else if (allStarPool[i].fitness < bestScore) {
			bestScore = allStarPool[i].fitness;
			printf("Best keyboard for round %d: \n", i+1);
			printKeyboard(&allStarPool[i]);
		}
	}
	
	printf("\n\nAll Star Pool filled. Initial phase complete. Now computing best layout.\n\n");

	Keyboard oldBest;
	initKeyboard(&oldBest);
	
	int generationsSoFar, totalGenerations;
	for (generationsSoFar = 0, totalGenerations = 0; generationsSoFar < ALL_STAR_GENERATIONS; ++generationsSoFar, ++totalGenerations) {
		copy(&oldBest, &allStarPool[0]);
		for (i = 0; i < ALL_STAR_POOL_SIZE; ++i) calcFitness(&allStarPool[i]);
		sortPool(allStarPool, 0, ALL_STAR_POOL_SIZE - 1);
		for (i = 0; i < ALL_STAR_POOL_SIZE/2; ++i) allStarPool[i+(ALL_STAR_POOL_SIZE/2)] = mutate(allStarPool[i]);
		if (!isEqual(&allStarPool[0], &oldBest)) generationsSoFar = -1;
	}
	
	printf("\nAfter %d fitness calculations, ", totalCalcFitness);
	printf("\nFinal Best Keyboard: \n");
	printKeyboard(&allStarPool[0]);
	printPercentages(&allStarPool[0]);	
	
	return allStarPool[0];
}

Keyboard runForOne()
{
	int i;
	Keyboard pool[POOL_SIZE];
	for (i = 0; i < POOL_SIZE; ++i) initKeyboard(&pool[i]);
		
	Keyboard oldBest;
	initKeyboard(&oldBest);

	int generationsSoFar, totalGenerations;
	int deleteThisMany = POOL_SIZE / 2;
	for (generationsSoFar = 0, totalGenerations = 0; generationsSoFar < GENERATIONS; ++generationsSoFar, ++totalGenerations) {
		copy(&oldBest, &pool[0]);
		
		for (i = 0; i < POOL_SIZE; ++i) calcFitness(&pool[i]);
		sortPool(pool, 0, POOL_SIZE - 1);
		for (i = 0; i < deleteThisMany; ++i) pool[i+deleteThisMany] = mutate(pool[i]);
		if (isEqual(&pool[0], &oldBest) == FALSE) generationsSoFar = -1;
	}
			
	return pool[0];

}


int runLoop(Keyboard pool[])
{
	int i;
	for (i = 0; i < POOL_SIZE; ++i) initKeyboard(&pool[i]);
		
	Keyboard oldBest;
	initKeyboard(&oldBest);

	int generationsSoFar;
	for (generationsSoFar = 0; generationsSoFar < GENERATIONS; ++generationsSoFar) {
		copy(&oldBest, &pool[0]);

		for (i = 0; i < POOL_SIZE; ++i) calcFitness(&pool[i]);
		sortPool(pool, 0, POOL_SIZE - 1);
		for (i = 0; i < POOL_SIZE/2; ++i) pool[i+(POOL_SIZE/2)] = mutate(pool[i]);
		if (isEqual(&pool[0], &oldBest) == FALSE) generationsSoFar = -1;
	}
			
	return 0;
}

Keyboard simpleMutate(Keyboard k)
{
	int r0, r1;
	
	r0 = rand() % ksize;
	r1 = rand() % ksize;
	swap(&k, r0, r1);
	
	return k;
}

Keyboard mutate(Keyboard k)
{
	return simpleMutate(k);
}

/* Sort by fitness from highest to lowest. Remember that lower is better. */
int sortPool(Keyboard pool[], int left, int right)
{
	Keyboard temp;
	int i = left;
	int j = right;
	int x = pool[(left+right)/2].fitness;
	
	do {
		while (pool[i].fitness < x) i++;
		while (pool[j].fitness > x) j--;
		if (i <= j) {
			temp = pool[i];
			pool[i] = pool[j]; 
			pool[j] = temp;
			i++; j--;
		}
	} while (i <= j);
	
	if (left < j) sortPool(pool, left, j);
	if (i < right) sortPool(pool, i, right);
	
	return 0;
}



