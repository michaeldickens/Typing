/*
 *  accessories.c
 *  Typing
 *
 *  Created by Michael Dickens on 8/11/09.  
 *  
 */

#include "accessories.h"

void worstDigraphsFromFile(char *filename)
{
	FILE *fp = fopen(filename, "r");
	Keyboard k, imp;
	while (layoutFromFile(fp, &k) != -1) {
		printf("Keyboard Layout:\n");
		printLayoutOnly(&k);
		worstDigraphs(&k);
	}
	
}

int worstDigraphs(Keyboard *k)
{
	int i;
	for (i = 0; i < 8; ++i) k->fingerUsage[i] = 0;
	
	char keys[diLen][2];
	memcpy(keys, diKeys, sizeof(char) * diLen * 2);
	
	long long values[diLen];

	for (i = 0; i < diLen; ++i) {
		k->distance		= 0;
		k->inRoll		= 0;
		k->outRoll		= 0;
		k->sameHand		= 0;
		k->sameFinger	= 0;
		k->rowChange	= 0;
		k->homeJump		= 0;
		k->toCenter		= 0;
		k->toOutside	= 0;
		int locs[2];
		
		locs[0] = loc(k, diKeys[i][0]);
		locs[1] = loc(k, diKeys[i][1]);
		
		// These all require that the hand be the same.
		if (hand[locs[0]] == hand[locs[1]]) {
			k->inRoll		= calcInRoll    (locs[0], locs[1]);	
			k->outRoll		= calcOutRoll   (locs[0], locs[1]);	
			k->sameHand		= SAME_HAND						  ;
			k->sameFinger	= calcSameFinger(locs[0], locs[1]);
			k->rowChange	= calcRowChange (locs[0], locs[1]);
			k->homeJump		= calcHomeJump  (locs[0], locs[1]);
			k->toCenter		= calcToCenter  (locs[0], locs[1]);
			k->toOutside	= calcToOutside (locs[0], locs[1]);
		}
		k->distance = (distanceCosts[locs[0]] + distanceCosts[locs[1]]) * DISTANCE;
//		printf("distance = %d, locs = [%d, %d]\n", k->distance, locs[0], locs[1]);
		// Re-assign diValues[i] to the cost of that digraph.
		values[i] = k->distance + k->inRoll + k->outRoll + k->sameHand + k->sameFinger + k->rowChange + k->homeJump + k->toCenter + k->toOutside;
		
		/* This function will tell you...
		 * Without this line: Which digraphs have the worst score.
		 * With this line: Which digraphs are the most damaging, based on both score and frequency.
		 */
		values[i] *= diValues[i];
	}
	
	sortDigraphs(keys, values, 0, diLen - 1);
		
	for (i = 0; i < diLen; ++i)
		printf("%c%c = %lld\n", keys[i][0], keys[i][1], values[i]);
		
	return 0;	
}

int sortDigraphs(char keys[][2], long long values[], int left, int right)
{	
	long long lltemp;
	char ctemp;
	
	long long pivot = values[(left + right) / 2];
	int i = left, j = right;
	do {
		while (values[i] < pivot) ++i;
		while (values[j] > pivot) --j;
		
		if (i <= j) {
			lltemp = values[i];
			values[i] = values[j];
			values[j] = lltemp;
			
			ctemp = keys[i][0];
			keys[i][0] = keys[j][0];
			keys[j][0] = ctemp;

			ctemp = keys[i][1];
			keys[i][1] = keys[j][1];
			keys[j][1] = ctemp;
			
			++i;
			--j;
		}
		
	} while (i <= j);
	
	if (i < right) sortDigraphs(keys, values, i, right);
	if (left < j) sortDigraphs(keys, values, left, j);
	
	return 0;
}

/* 
 * Read in each layout from the file and print out detailed information.
 */
int compare(char *filename)
{
	FILE *fp = fopen(filename, "r");
	char c = '2';
	while (c != EOF) {
		Keyboard k;
		c = layoutFromFile(fp, &k);
		calcFitnessDirect(&k);
		printPercentages(&k);
	}
	
	fclose(fp);
	
	return 0;
}


/* 
 * Improves each layout in the given file to the maximum extent possible.
 */
void improveFromFile(char *filename)
{
	FILE *fp = fopen(filename, "r");
	Keyboard k, imp;
	while (layoutFromFile(fp, &k) != -1) {
		printf("Layout to Improve:\n");
		imp = improver(k);
	}
}

/*
 * Improves k to the maximum extent possible.
 */
Keyboard improver(Keyboard k)
{
	calcFitnessDirect(&k);
	printPercentages(&k);
	
	Keyboard tk, bestk;
	copy(&tk, &k);
	copy(&bestk, &k);
	long long bestEval = k.fitness;
	long long curEval = doRun(&tk);
	
	if (curEval < bestEval) {
		copy(&bestk, &tk);
		bestEval = curEval;
		calcFitnessDirect(&tk);
		printPercentages(&tk);
	}

	return bestk;
}

/*
 *  TIMING DATA
 *  
 *  old: 
 * 
 *  Time to initialize 100,000 keyboards:	~25,000 microseconds.
 *  Time to mutate 100,000 keyboards:		~35,000 microseconds.
 *  Time to copy 100,000 keyboards:			~12,000 microseconds.
 *  Time to score 100,000 keyboards:    ~43,630,000 microseconds. ~160,000,000 microseconds if trigraphs are used for same hand.
 *  Time to sort 1024 keyboards:			   ~600 microseconds.
 *  Time to do loc() 100,000 times:			  ~9000 microseconds.
 *  Time to do rand30() 100,000 times:		  ~2000 microseconds.
 *  
 *  new: 
 *  I don't know why some of them took longer than before. That must be a bug. I didn't change 
 *  initialize, mutate, copy, or rand30().
 *  
 *  Time to initialize 100,000 keyboards:	~270,000 microseconds.
 *  Time to mutate 100,000 keyboards:		~180,000 microseconds.
 *  Time to copy 100,000 keyboards:			 ~29,000 microseconds.
 *  Time to score 100,000 keyboards:	  ~8,140,858 microseconds.
 *  Time to do loc() 100,000 times:			   ~2000 microseconds.
 *  Time to do rand30() 100,000 times:		   ~4000 microseconds.

 *  
 *  
 *  You can see that the scoring takes far longer than anything else. This program can score about 12,000 layouts per second. This 
 *  is about six times as fast as Michael Capewell's program (http://www.michaelcapewell.com/projects/keyboard/) 
 *  and many times faster than Peter Klausler's program (http://klausler.com/evolved.html)
 *  
 */
int runTimingTests()
{
	int start, startsec;
	int i, j;
	Keyboard tester;
	Keyboard array[1024];
	
	int initAverage = 0;
	int mutateAverage = 0;
	int sortAverage = 0;
	int copyAverage = 0;
	int fitnessAverage = 0;
	int locAverage = 0;
	int randAverage = 0;
		
	for (j = 0; j < 10; j++) {
		// How long does it take to initialize a keyboard?
		gettimeofday(&tv, NULL);
		start = tv.tv_usec;
		startsec = tv.tv_sec;
		for (i = 0; i < 100000; ++i) initKeyboard(&tester);
		gettimeofday(&tv, NULL);
		initAverage = ((initAverage * j) + tv.tv_usec - start + 1000000*(tv.tv_sec - startsec)) / (j + 1);
		
		// How long does it take to mutate a keyboard?
		gettimeofday(&tv, NULL);
		start = tv.tv_usec;
		startsec = tv.tv_sec;
		for (i = 0; i < 100000; ++i) tester = mutate(tester);
		gettimeofday(&tv, NULL);
		mutateAverage = ((mutateAverage * j) + tv.tv_usec - start + 1000000*(tv.tv_sec - startsec)) / (j + 1);
		
		// This is causing bad access
		// How long does it take to sort 1024 keyboards?
		gettimeofday(&tv, NULL);
		start = tv.tv_usec;
		startsec = tv.tv_sec;
//		sortPool(array, 0, 1023);
		gettimeofday(&tv, NULL);
		sortAverage = ((sortAverage * j) + tv.tv_usec - start + 1000000*(tv.tv_sec - startsec)) / (j + 1);

		// How long does it take to copy a keyboard?
		gettimeofday(&tv, NULL);
		start = tv.tv_usec;
		startsec = tv.tv_sec;
		for (i = 0; i < 100000; ++i) copy(&tester, &array[0]);
		gettimeofday(&tv, NULL);
		copyAverage = ((copyAverage * j) + tv.tv_usec - start + 1000000*(tv.tv_sec - startsec)) / (j + 1);

		// How long does it take to score a keyboard?
		gettimeofday(&tv, NULL);
		start = tv.tv_usec;
		startsec = tv.tv_sec;
		for (i = 0; i < 100000; ++i) calcFitness(&tester);
		gettimeofday(&tv, NULL);
		fitnessAverage = ((fitnessAverage * j) + tv.tv_usec - start + 1000000*(tv.tv_sec - startsec)) / (j + 1);

		// How long does it take to find a key on a keyboard?
		gettimeofday(&tv, NULL);
		start = tv.tv_usec;
		startsec = tv.tv_sec;
		for (i = 0; i < 100000; ++i) loc(&tester, 'a');
		gettimeofday(&tv, NULL);
		locAverage = ((locAverage * j) + tv.tv_usec - start + 1000000*(tv.tv_sec - startsec)) / (j + 1);

		// How long does it take to generate a pseudorandom number from 0 to 29?
		gettimeofday(&tv, NULL);
		start = tv.tv_usec;
		startsec = tv.tv_sec;
		for (i = 0; i < 100000; ++i) rand30();
		gettimeofday(&tv, NULL);
		randAverage = ((randAverage * j) + tv.tv_usec - start + 1000000*(tv.tv_sec - startsec)) / (j + 1);
	}
	printf(" *  Time to initialize 100,000 keyboards:	%d microseconds.\n", initAverage);
	printf(" *  Time to mutate 100,000 keyboards:		%d microseconds.\n", mutateAverage);
	printf(" *  Time to sort 1024 keyboards:			%d microseconds.\n", sortAverage);
	printf(" *  Time to copy 100,000 keyboards:			%d microseconds.\n", copyAverage);
	printf(" *  Time to score 100,000 keyboards:		%d microseconds.\n", fitnessAverage);
	printf(" *  Time to do loc() 100,000 times:			%d microseconds.\n", locAverage);
	printf(" *  Time to do rand30() 100,000 times:		%d microseconds.\n", randAverage);
	
	return 0;
}

/* 
 * Make sure that all of the fitness functions are working correctly. A bug in there 
 * would be hard to notice without explicitly testing it.
 * 1: There is a bug in one of the fitness functions.
 */
int testFitness()
{
	if (FULL_KEYBOARD == FK_NO) {
		printf("\nTesting calcInRoll():\n");
		testResult(calcInRoll(0, 0), 0);
		testResult(calcInRoll(10, 11), IN_ROLL);
		testResult(calcInRoll(14, 15), 0);
		testResult(calcInRoll(29, 28), IN_ROLL);
		testResult(calcInRoll(3, 4), 0);
		testResult(calcInRoll(4, 3), 0);
		testResult(calcInRoll(11, 2), 0);
		testResult(calcInRoll(2, 11), 0);
		
		printf("\nTesting calcOutRoll():\n");
		testResult(calcOutRoll(0, 0), 0);
		testResult(calcOutRoll(11, 10), OUT_ROLL);
		testResult(calcOutRoll(15, 14), 0);
		testResult(calcOutRoll(28, 29), OUT_ROLL);
		testResult(calcOutRoll(3, 4), 0);
		testResult(calcOutRoll(14, 13), 0);
		
		printf("\nTesting calcSameFinger():\n");
		testResult(calcSameFinger(0, 0), 0);
		testResult(calcSameFinger(10, 0), SAME_FINGER_P);
		testResult(calcSameFinger(11, 21), SAME_FINGER_R);
		testResult(calcSameFinger(7, 27), SAME_FINGER_M);
		testResult(calcSameFinger(13, 4), SAME_FINGER_I);
		testResult(calcSameFinger(0, 4), 0);
		
		printf("\nTesting calcRowChange():\n");
		testResult(calcRowChange(8, 8), 0);
		testResult(calcRowChange(10, 0), ROW_CHANGE);
		testResult(calcRowChange(11, 0), ROW_CHANGE + HAND_WARP);
		testResult(calcRowChange(13, 2), ROW_CHANGE + HAND_SMOOTH);
		testResult(calcRowChange(8, 16), ROW_CHANGE);
		testResult(calcRowChange(8, 26), ROW_CHANGE);
		testResult(calcRowChange(28, 9), ROW_CHANGE + HAND_WARP);
		testResult(calcRowChange(25, 5), ROW_CHANGE);
		
		printf("\nTesting calcHomeJump():\n");
		testResult(calcHomeJump(8, 8), 0);
		testResult(calcHomeJump(10, 0), 0);
		testResult(calcHomeJump(11, 0), 0);
		testResult(calcHomeJump(10, 1), 0);
		testResult(calcHomeJump(8, 16), 0);
		testResult(calcHomeJump(8, 26), HOME_JUMP + HOME_JUMP_INDEX);
		testResult(calcHomeJump(28, 9), HOME_JUMP);
		testResult(calcHomeJump(25, 5), HOME_JUMP);
		
		printf("\nTesting calcToCenter():\n");
		testResult(calcToCenter(0, 0), 0);
		testResult(calcToCenter(13, 14), TO_CENTER);
		testResult(calcToCenter(17, 15), TO_CENTER);
		testResult(calcToCenter(4, 1), TO_CENTER);
		testResult(calcToCenter(25, 9), TO_CENTER);
		testResult(calcToCenter(25, 15), 0);
	
	} else if (FULL_KEYBOARD == FK_STANDARD) {
		printf("\nTesting calcHomeJump():\n");
	
	} else {
		printf("\nTesting calcInRoll():\n");
		testResult(calcInRoll(0, 0), 0);
		testResult(calcInRoll(2, 3), IN_ROLL);
		testResult(calcInRoll(2, 14), 0);
		testResult(calcInRoll(13, 3), 0);
		testResult(calcInRoll(29, 28), IN_ROLL);
		testResult(calcInRoll(28, 27), 0);
		
		printf("\nTesting calcSameFinger():\n");
		testResult(calcSameFinger(0, 0), 0);
		testResult(calcSameFinger(11, 0), SAME_FINGER_P);
		testResult(calcSameFinger(12, 23), SAME_FINGER_R);
		testResult(calcSameFinger(7, 29), SAME_FINGER_M);
		testResult(calcSameFinger(25, 15), SAME_FINGER_I);
		testResult(calcSameFinger(0, 4), 0);
	}

	
	return 0;
}

int testResult(int result, int expected)
{
	if (result == expected) printf("Test succeeded.\n");
	else printf("Test failed (%d expected, %d found).\n", expected, result);
	return 0;
}

