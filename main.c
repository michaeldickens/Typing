/*
 *  main.m
 *  Typing
 *
 *  Created by Michael Dickens on 8/11/09.
 *
 */
 
#include <stdio.h>
#include "cjalgorithm.h"

int run(char *filename);

int main(int argc, const char *argv[])
{		
	/* init fitness evaluator */
	initData();

	/* init gen alg */
	initAlgorithm();
	
	char *filename;
	if (FULL_KEYBOARD == FK_NO) filename = "layoutstore.txt";
	else if (FULL_KEYBOARD == FK_STANDARD) filename = "fulllayoutstore.txt";
	else if (FULL_KEYBOARD == FK_KINESIS) filename = "kinesislayoutstore.txt";

	if (argc > 1) {		
		if (strcmp(argv[1], "algorithm") == 0) {
			run(filename);
		} else if (strcmp(argv[1], "compare") == 0) {
			compare(filename);
		} else if (strcmp(argv[1], "improve") == 0) {
			FILE *fp = fopen(filename, "r");
			Keyboard k;
			layoutFromFile(fp, &k);
			improver(k);
		} else if (strcmp(argv[1], "improveFromFile") == 0) {
			improveFromFile(filename);
		} else if (strcmp(argv[1], "test") == 0) {
			testFitness();
		}
	} else {
		run(filename);
//		runTimingTests();
//		compare(filename);
//		improveFromFile(filename);
//		worstDigraphsFromFile(filename);
//		testFitness();
	}
	
	return 0;
}

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
		if (INIT_FROM_FILE && !KEEP_NUMBERS) {
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