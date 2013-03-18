/*
 *  cjalgorithm.m
 *  Typing
 *
 *  Written by Michael Dickens.
 *
 */

#include "cjalgorithm.h"

/* 
 * Runs a simple version of the algorithm without all the bells and whistles.
 * Only uses a single thread, and generally runs significantly slower than 
 * runAlgorithm().
 */
void runSimpleAlgorithm()
{
    Keyboard bestk, k;
    copyKeyboard(&bestk, &nilKeyboard);
    copyKeyboard(&k, &nilKeyboard);
    
    time_t startTime = time(NULL);
    time_t printTimeInterval = PRINT_TIME_INTERVAL;
    time_t timeOnPrint = startTime + printTimeInterval;
    
    int roundNum;
    for (roundNum = 0; ; ++roundNum) {
        initKeyboard(&k);
        anneal(&k, NULL, 0);
        
        if (k.fitness < bestk.fitness) {
            copyKeyboard(&bestk, &k);
            printPercentages(&bestk);
            printf("%d Rounds\n", roundNum);
            printTime(startTime);
            
            /* If a keyboard was just printed, don't print the time for
             * a while.
             */
            timeOnPrint = time(NULL) + printTimeInterval;
        } else if (time(NULL) >= timeOnPrint) {
            printf("%d Rounds\n", roundNum);
            printTime(startTime);
            timeOnPrint = time(NULL) + printTimeInterval;
            printTimeInterval = 1.5 * printTimeInterval + 1;
        }
    }
}

/*
 * Search for an optimal keyboard layout. Equipped to use multithreading.
 */
void runAlgorithm()
{	
	struct ThreadArg arg;
	copyKeyboard(&arg.bestk, &nilKeyboard);
	arg.numRounds = ALGORITHM_ROUNDS;
	arg.startTime = time(NULL);
	arg.numThreads = numThreads;
    arg.chanceToUsePreviousLayout = CHANCE_TO_USE_PREVIOUS_LAYOUT;
    arg.numberOfSwaps = NUM_SWAPS_BETWEEN_ROUNDS;
	arg.isFinished = FALSE;
    
    int runsBeforeChanceInc = RUNS_BEFORE_CHANCE_INC;
    int runsBeforeSwapsInc = RUNS_BEFORE_SWAPS_INC;
    int gtbRounds = GTB_ROUNDS;
    time_t printTimeInterval = PRINT_TIME_INTERVAL;
    time_t timeOnPrint = arg.startTime + printTimeInterval;
    
    int64_t prevBestFitness = FITNESS_MAX;
	
    int runNum;
	for (runNum = 0; runNum < MAX_RUNS; ++runNum) {
        if (runNum % runsBeforeChanceInc == 0) {
            arg.chanceToUsePreviousLayout =
                    pow(arg.chanceToUsePreviousLayout, CHANCE_EXPONENTIATOR);
			runsBeforeChanceInc = (int) (runsBeforeChanceInc * 1.2) + 1;
			if (detailedOutput)
                printf("Chance to use previous layout is now %f.\n",
                       arg.chanceToUsePreviousLayout);
        }
        
        if (runNum % runsBeforeSwapsInc == 0) {
            ++arg.numberOfSwaps;
			runsBeforeSwapsInc = (int) (runsBeforeSwapsInc * 1.2) + 1;
			if (detailedOutput)
                printf("Number of swaps between rounds is now %d.\n",
                       arg.numberOfSwaps);
        }
        
        if (runNum % RUNS_BEFORE_GTB_ROUNDS_INC == 0) {
            gtbRounds *= 2;
            if (detailedOutput)
                printf("Number of rounds in greatToBest() is now %d.\n",
                       gtbRounds);
        }
        
		runThreadsRec((void *) (&arg));
		        
        if (arg.bestk.fitness < prevBestFitness) {
            prevBestFitness = arg.bestk.fitness;
            printPercentages(&arg.bestk);
            printTime(arg.startTime);
            
            /* If a keyboard was just printed, don't print the time for  
             * a while.
             */
            timeOnPrint = time(NULL) + printTimeInterval;
        } else if (time(NULL) >= timeOnPrint && detailedOutput) {
            printTime(arg.startTime);
            timeOnPrint = time(NULL) + printTimeInterval;
            printTimeInterval = 1.5 * printTimeInterval + 1;
        }
        
        int64_t bestBeforeGTB = arg.bestk.fitness;
        greatToBest(&arg.bestk, gtbRounds);
        
        if (arg.bestk.fitness < bestBeforeGTB) {
            prevBestFitness = arg.bestk.fitness;
            if (detailedOutput)
                printf("\n***Found from greatToBest()***\n");
            printPercentages(&arg.bestk);
            printTime(arg.startTime);
        }
	}
}

/* 
 * This is the core function called by runAlgorithm(). It uses the 
 * following process to avoid as much downtime as possible.
 * 
 *  1. If numThreads <= 1, run anneal() numRounds times.
 *  2. Otherwise, recursively call this function with a decremented numThreads.
 *  3. Until the inner call of this function returns, keep running anneal().
 *  4. If the inner call found a bestk better than the current call, set the 
 *     current bestk to the inner bestk.
 * 
 * The core of this algorithm is step 3. In the worst case, this function will 
 * just be starting a new call to anneal() as the inner function returns, so 
 * the most possible time that can be wasted is the length of one anneal() call 
 * multiplied by the depth of the recursive call tree. The greatest proportion 
 * of time that can be wasted is (numThreads / numRounds), so for sufficiently 
 * large values of numRounds (perhaps 16 and above), the wasted time is trivial.
 */
void * runThreadsRec(void *arg)
{
	struct ThreadArg *threadArg = (struct ThreadArg *) arg;
		
	struct ThreadArg innerArg;
	copyThreadArg(&innerArg, threadArg);
	innerArg.numThreads = threadArg->numThreads - 1;
	innerArg.isFinished = FALSE;
	
    if (threadArg->numThreads > 1) {
        pthread_t thread;
        int ret = pthread_create(&thread, NULL, &runThreadsRec, 
                                 (void *) (&innerArg));
        if (ret) {
            threadArg->isFinished = TRUE;
            return (void *) (size_t) ret;
        }
    }
	
	Keyboard k, prevk;
    copyKeyboard(&k, &nilKeyboard);
    copyKeyboard(&prevk, &nilKeyboard);
    
    int i;
	for (i = 0; threadArg->numThreads <= 1 ? i < threadArg->numRounds :
                !innerArg.isFinished; ++i) {
        if (i > 0 && rand() / RAND_MAX < threadArg->chanceToUsePreviousLayout) {
            copyKeyboard(&k, &prevk);
            smartMutate(NULL, &k, threadArg->numberOfSwaps);
        } else {
            initKeyboard(&k);
        }
		
        anneal(&k, NULL, 0);
        copyKeyboard(&prevk, &k);
        
		if (k.fitness < threadArg->bestk.fitness) {
            copyKeyboard(&threadArg->bestk, &k);
            
            /* Only print keyboards on the bottom thread.
             */
            if (threadArg->numThreads <= 1) {
                printPercentages(&threadArg->bestk);
                printTime(threadArg->startTime);
            }
        }
    }
		
	if (innerArg.bestk.fitness < threadArg->bestk.fitness)
		copyKeyboard(&threadArg->bestk, &innerArg.bestk);
	
	threadArg->isFinished = TRUE;
	return NULL;
}

/* Take a great keyboard and make it the best keyboard. Uses an optimization 
 * heuristic that works best for nearly-optimal keyboards.
 */
void greatToBest(Keyboard *k, int numRounds)
{
	struct ThreadArg arg;
    initThreadArg(&arg);
	copyKeyboard(&arg.bestk, k);
	arg.numRounds = numRounds;
	arg.numThreads = numThreads;
	arg.isFinished = FALSE;
	
	greatToBestThreadRec((void *) (&arg));
    copyKeyboard(k, &arg.bestk);
}

void * greatToBestThreadRec(void *arg)
{
    struct ThreadArg *threadArg = (struct ThreadArg *) arg;
    
	Keyboard k;
    copyKeyboard(&k, &threadArg->bestk);
	
	struct ThreadArg innerArg;
	copyThreadArg(&innerArg, threadArg);
	innerArg.numThreads = threadArg->numThreads - 1;
	innerArg.isFinished = FALSE;
	
    if (threadArg->numThreads > 1) {
        pthread_t thread;
        int ret = pthread_create(&thread, NULL, &greatToBestThreadRec, 
                                 (void *) (&innerArg));
        if (ret) {
            threadArg->isFinished = TRUE;
            return (void *) (size_t) ret;
        }
    }
    
    /* Temporarily disabled until bugs are fixed in greatToBestBruteForce().
     */
    if (FALSE &&
        rand() / (double) INT_MAX < CHANCE_TO_USE_BRUTE_FORCE) {
        /* Use GTBBF(). If innerArg is still running when this finishes, it 
         * will begin using the standard algorithm below.
         */
        int64_t prevBest = threadArg->bestk.fitness;
        greatToBestBruteForce(&threadArg->bestk);
        if (detailedOutput && threadArg->bestk.fitness < prevBest)
            printf("\nFound with GTBBF(): %lld\n", threadArg->bestk.fitness);
    }
    
    /* Use the standard greatToBest() algorithm. */

    int numberOfSwaps = GTB_NUMBER_OF_SWAPS;
    
    int i;
    for (i = 0; threadArg->numThreads <= 1 ? i < threadArg->numRounds :
         !innerArg.isFinished; ++i) {
        if (i % GTB_ROUNDS_BEFORE_SWAP_INC == GTB_ROUNDS_BEFORE_SWAP_INC - 1) {
            ++numberOfSwaps;
        }
        
        copyKeyboard(&k, &threadArg->bestk);
        
        /* Any swaps made by smartMutate() are "locked in" and may not be
         * undone by anneal().
         */
        int lockins[numberOfSwaps][2];
        smartMutate(lockins, &k, numberOfSwaps);
        
        /* Use lockins only half the time. */
        if (i % 2 == 0) anneal(&k, lockins, numberOfSwaps);
        else anneal(&k, NULL, 0);
        
        calcFitness(&k);
        if (k.fitness < threadArg->bestk.fitness) {
            copyKeyboard(&threadArg->bestk, &k);
        }
    }	
	   
	if (innerArg.bestk.fitness < threadArg->bestk.fitness)
		copyKeyboard(&threadArg->bestk, &innerArg.bestk);

    threadArg->isFinished = TRUE;
	return NULL;
}

/*
 * Chooses several indices and searches all permutations of keys at those
 * indices to find the best one. More likely to choose rare keys.
 * 
 * WARNING: Currently buggy. Do not use until bugs are fixed.
 */
void greatToBestBruteForce(Keyboard *k)
{
	int i, j, length = GTBBF_ROUNDS;
	int locs[length];
		
	/* Choose which locs to permute. */
	for (i = 0, j = monLen - 1; i < length; --j) {
		while (!isSwappable(monographs[j].key) || isBracket(monographs[j].key) ||
               keepShiftPair(monographs[j].key) || rand() % 4 == 0)
			--j;
		
		/* If we run out of characters (which is unlikely), loop back around. 
         * WARNING: This may add duplicate characters to locs[]. I don't expect 
         * this to cause problems, but it might.
         */
		if (j < 0) j = monLen - 1;
        
		locs[i] = locWithShifted(k, monographs[j].key);
        if (locs[i] != -1)
            ++i;
	}
	
    /* Always add brackets in pairs. */
	for (i = 0; i < length; ++i) {
		if (isBracket(charAt(k, locs[i]))) {
			int savedLoc = locs[(i+1) % length];
            char match = getMatchingBracket(charAt(k, locs[i]));
			locs[(i+1) % length] = locWithShifted(k, match);
            if (locs[(i+1) % length] == -1) {
                fprintf(stderr, "Error: Unable to find '%c'.\n", match);
            }
			++i;
			
			/* If locs already contained the matching bracket, remove one of 
			 * the copies of the matching bracket and replace it with the 
			 * character that was removed.
			 */
			for (j = 0; j < length; ++j)
				if (i != j && locs[i] == locs[j])
					locs[j] = savedLoc;
		}
	}
	
	/* Initialize the keyboards. */
	calcFitness(k);
	Keyboard origk;
	copyKeyboard(&origk, k);
    
	/* Find the best permutation. */
    int origLocs[length];
    memcpy(origLocs, locs, sizeof(int) * length);
	tryPermutations(k, &origk, origLocs, locs, length, 0);
}

void tryPermutations(Keyboard *bestk, Keyboard *k, int *origLocs, int *locs,
                    int length, int index)
{
    int i;
    if (index == length - 1) {
        Keyboard saved;
        copyKeyboard(&saved, k);
        
        int hasPairMoved[ksize];
        memset(hasPairMoved, FALSE, sizeof(hasPairMoved));
        
        for (i = 0; i < length; ++i) {
            if (!isSwappable(charAt(k, origLocs[i])) ||
                !isSwappable(charAt(&saved, locs[i]))) {
                fprintf(stderr, "Error: In tryPermutations(), trying to swap unswappable %c and %c\n",
                       charAt(k, origLocs[i]), charAt(&saved, locs[i]));
            }

            if (keepShiftPair(charAt(k, origLocs[i])) ||
                    keepShiftPair(charAt(&saved, locs[i]))) {
                fprintf(stderr, "Error: In tryPermutations(), trying to swap %c and %c where one of them must remain in a pair.\n",
                        charAt(k, origLocs[i]), charAt(&saved, locs[i]));
                copyKeyboard(k, &saved);
                return;
            }
            setCharAt(k, origLocs[i], charAt(&saved, locs[i]));
        }

        /* If brackets are still properly aligned, score the permuted layout to 
         * see if it's better. If brackets are not properly aligned, don't 
         * bother. Note: This also works if keepBrackets == 0.
         */
        if (calcBrackets(k) == 0) {
            calcFitness(k);
            if (k->fitness < bestk->fitness) {
                copyKeyboard(bestk, k);
            }
        }
        
        /* Put keys back in their original spots. */
        copyKeyboard(k, &saved);
    } else {
        for (i = index; i < length; ++i) {
            int temp = locs[index];
            locs[index] = locs[i];
            locs[i] = temp;
            
            tryPermutations(bestk, k, origLocs, locs, length, index + 1);
            
            temp = locs[index];
            locs[index] = locs[i];
            locs[i] = temp;
        }
    }
}

/* 
 * Simulated annealing algorithm based on an implementation by Chris Johnson.
 */
int64_t anneal(Keyboard *k, int lockins[][2], size_t lockin_length)
{
	int64_t lastEvaluation, evaluation;
	int64_t lastImprovement = 0;
	int64_t evaluationToBeat = FITNESS_MAX;
		
	/* Do the "zeroth" iteration */
	calcFitness(k);
	lastEvaluation = evaluation = k->fitness;
        
    /* TODO: Test the three versions of this to see which works best. (See 
     * email "source code" for details.)
     */
	/* Keep doing iterations while the layout is still improving */
	do {
		if (evaluation < lastEvaluation) {
			lastImprovement = lastEvaluation - evaluation;
		} else {
			lastImprovement = 0;
		}
		
		lastEvaluation = evaluation;
		evaluationToBeat = lastEvaluation + lastImprovement;
		evaluation = improveLayout(evaluationToBeat, k, lockins, lockin_length);
	} while (evaluation < evaluationToBeat);

	return evaluation;
}

/* 
 * Modified from a version written by Chris Johnson.
 */
int64_t improveLayout(int64_t evaluationToBeat, Keyboard *k, 
	int lockins[][2], size_t lockin_length)
{
	int64_t evaluation;
	int i, j, inx;
	
	/* Create a list of indices and shuffle it. */
	int indices[2 * trueksize];
	buildShuffledIndices(indices, 2 * trueksize);

	/* try swaps until we beat evaluationToBeat... */
	for (i = 0; i < 2 * trueksize; ++i) {
		for (j = i + 1; j < 2 * trueksize; ++j) {
			
			if (!isLegalSwap(k, indices[i], indices[j])) {
				continue;
			}
			
			/* TODO: This is slow. I don't think it will be a significant 
			 * bottleneck, but I should profile it just in case.
			 */
			int skipRound = FALSE;
			for (inx = 0; inx < lockin_length; ++inx) {
				if (lockins[inx][0] == indices[i] || 
						lockins[inx][0] == indices[j] || 
						lockins[inx][1] == indices[i] || 
						lockins[inx][1] == indices[j]) {
					skipRound = TRUE;
					break;
				}
			}
			if (skipRound) {
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
 * Goes through the list of characters from least to most common. Selects 
 * characters to swap. Less frequent characters are more likely to be swapped.
 * 
 * For the nth character, the probability that it will be mutated is given by 
 *   P(1) = 1 / q
 *   P(n) = (1 / q) * (1 - (P(1) + P(2) + ... + P(n-1)))
 * where q is a constant.
 * 
 * Alternatively, 
 *   P(n) = (q-1)^(n-1) / q^n
 * (This formula does not necessarily apply for the last element.)
 * 
 * These probabilities are only correct if every swap is legal. If not, legal 
 * swaps are somewhat more probable and illegal swaps are of course completely 
 * impossible.
 * 
 * swapIndices: After the function returns, this contains a list of swaps made. 
 *   It must be able to contain (numberOfSwaps) swaps. If swapIndices is NULL, 
 *   this function will execute as normal but will not write anything to 
 *   swapIndices.
 */
int smartMutate(int swapIndices[][2], Keyboard *k, int numberOfSwaps)
{
	int q = monLen / 4;
	
	int swapslen = 2 * numberOfSwaps;	
	char charsToSwap[swapslen];
	
	int i, j;
	
	/* Fills charsToSwap. */
	for (j = 0; j < swapslen; ++j) {
		charsToSwap[j] = monographs[0].key;

		for (i = monLen - 1; i >= 0; --i) {
			if (isSwappable(monographs[i].key) && rand() % q == 0) {
				charsToSwap[j] = monographs[i].key;
				break;
			}
		}
	}
	
	int lc1, lc2;
	for (i = 0; i < swapslen; i += 2) {
		lc1 = locWithShifted(k, charsToSwap[i]);
		lc2 = locWithShifted(k, charsToSwap[i+1]);
		
		if (swapIndices) {
            swapIndices[i / 2][0] = lc1;
            swapIndices[i / 2][1] = lc2;
        }
		
		if (isLegalSwap(k, lc1, lc2)) {
			swap(k, lc1, lc2);
		}
	}
	
	return 0;
}

void initThreadArg(struct ThreadArg *arg)
{
    memset(arg, 0, sizeof(struct ThreadArg));
}

void copyThreadArg(struct ThreadArg *dest, struct ThreadArg *src)
{
    memcpy(dest, src, sizeof(struct ThreadArg));
}
