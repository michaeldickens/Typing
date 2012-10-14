/*
 *  accessories.c
 *  Typing
 *
 *  Created by Michael Dickens on 8/11/09.  
 *  
 */

#include "accessories.h"

int getCommands()
{
	printf("Welcome to the Keyboard Layout Optimizer. If you have questions or comments, contact Michael Dickens by email (mdickens93@gmail.com) or leave a comment at http://mathematicalmulticore.wordpress.com/category/keyboards/.\n");
	printf("Type \"help\" for a listing of commands.\n\n");

	int length = 5000;
	char cmd[length];

	do {
		printf(">>> ");
		fgets(cmd, length, stdin);

		cmd[strlen(cmd)-1] = '\0'; // Remove the newline.

		if (streq(cmd, "help")) {
			printf("algorithm: Run the keyboard optimization algorithm.\n");
			printf("best swap <filename>: For the first layout in <filename>, print the single swap that would improve the layout the most.\n");
			printf("compare <filename>: Print information about the keyboards in <filename>. The keyboards must be in the proper format.\n");
			printf("damaging <filename>: Find the most damaging digraphs for the keyboard layouts in <filename>.\n");
			printf("game: Play a keyboard layout game.\n");
			printf("get <variable>: Get the value of the specified variable.\n");
			printf("improve <filename>: Try to improve the first keyboard in <filename>. The keyboard must be in the proper format.\n");
			printf("make typing data: Use the files in freq_types to customize character and digraph frequency.\n");
			printf("set <variable> <value>: Set the specified variable to the given value.\n");
			printf("setksize <fk_setting>: Set the keyboard type. Type \"setksize help\" for more information.\n");
			printf("test fitness: Test that the fitness functions are working properly.\n");
			printf("use <keys>: Use <keys> in the keyboard layout instead of the default.\n");
			printf("worst <filename>: Find the worst digraphs for the keyboard layouts in <filename>.\n");
			printf("variables: Print all variables that can be modified.\n");
			printf("quit: Quit the keyboard optimization program.\n");
			printf("\n");

		} else if (streq(cmd, "algorithm")) {
			printf("Running the keyboard optimization algorithm. Press ctrl-C to abort.\n\n");
			runCJAlgorithm(kbdFilename);

		} else if (streqn(cmd, "best swap ", strlen("best swap "))) {
			const char *const filename = cmd + strlen("best swap ");
			FILE *file = fopen(filename, "r");
			CHECK_FILE_FOR_NULL(file, filename);
			
			Keyboard k;
			if (layoutFromFile(file, &k) != -1) {
				layoutFromFile(file, &k);
				bestSwap(&k);
			}

			fclose(file);

		} else if (streqn(cmd, "compare ", 8)) {
			compare(cmd + 8);

		} else if (streqn(cmd, "damaging ", 9)) {
			const char *const filename = cmd + 9;
			worstDigraphsFromFile(filename, TRUE);

		} else if (streq(cmd, "game")) {
			game();

		} else if (streqn(cmd, "get ", 4)) {
			getValue(cmd + 4);

		} else if (streqn(cmd, "improve ", strlen("improve "))) {
			improveFromFile(cmd + 8);

		} else if (streq(cmd, "make typing data")) {
			makeTypingData();

		} else if (streqn(cmd, "set ", strlen("set "))) {
			setValue(cmd + 4);

		} else if (streqn(cmd, "setksize ", strlen("setksize "))) {
			size_t str_len = strlen("setksize ");
			if (streq(cmd + str_len, "no")) {
				setksize(FK_NO);
				printf("Keyboard set to non-full. All user-defined values have been reset.\n\n");
			} else if (streq(cmd + str_len, "standard")) {
				setksize(FK_STANDARD);
				printf("Keyboard set to full standard. All user-defined values have been reset.\n\n");
			} else if (streq(cmd + str_len, "kinesis")) {
				setksize(FK_KINESIS);
				printf("Keyboard set to full Kinesis. All user-defined values have been reset.\n\n");
			} else if (streq(cmd + str_len, "iphone")) {
				setksize(FK_IPHONE);
				printf("Keyboard set to iPhone. All user-defined values have been reset.\n\n");
			} else {
				printf("Undefined input. Valid inputs: \"setksize no\" (do not use full keyboard), \"setksize standard\" (use standard full keyboard), \"setksize kinesis\" (use Kinesis full keyboard).\n\n");
			}

		} else if (streq(cmd, "test fitness")) {
			testFitness();
		
		} else if (streqn(cmd, "use ", 4)) {
			strcpy(keysToInclude, cmd + 4);
			initTypingData();
			
			printf("Now using keys: %s\n\n", cmd + 4);

		} else if (streq(cmd, "variables")) {
			printf("Boolean variables should be set to 0 for false and 1 for true. Variables not specified as booleans are integers.\n");
			printf("\t(bool) detailedOutput    : provide additional information while running the algorithm\n");
			printf("\t(bool) keepZXCV          : keep keys Z, X, C, and V in place\n");
			printf("\t(bool) keepQWERTY        : try to keep keys in their QWERTY positions\n");
			printf("\t(bool) keepNumbers       : keep numbers in place\n");
			printf("\t(bool) keepBrackets      : keep brackets symmetrical\n");
			printf("\t(bool) keepShiftPairs    : shifted/unshifted pairs of special characters stay together\n");
			printf("\t(bool) keepTab           : keep Tab in place\n");
			printf("\t(bool) keepNumbersShifted: numbers do not move between shifted and unshifted\n");
			printf("\nThese variables determine the costs for particular key combinations. Higher cost is worse.\n");
			printf("\tdistance\n");
			printf("\tinRoll\n");
			printf("\toutRoll\n");
			printf("\tsameHand\n");
			printf("\tsameFingerP\n");
			printf("\tsameFingerR\n");
			printf("\tsameFingerM\n");
			printf("\tsameFingerI\n");
			printf("\trowChangeUp\n");
			printf("\trowChangeDown\n");
			printf("\thandWarp\n");
			printf("\thandSmooth\n");
			printf("\thomeJump\n");
			printf("\thomeJumpIndex\n");
			printf("\tdoubleJump\n");
			printf("\ttoCenter\n");
			printf("\ttoOutside\n");

		} else if (streqn(cmd, "worst ", 6)) {
			const char *const filename = cmd + 6;
			worstDigraphsFromFile(filename, FALSE);

		} else if (streq(cmd, "quit")) {
			printf("Goodbye!\n");
			break;

		} else {
			printf("Unknown command. Type \"help\" for a listing of commands.\n\n");
		}


	} while (strcmp(cmd, "exit") != 0);

	return 0;
}

int game()
{
	printf("\tWelcome to the keyboard layout game. The object of the game is ");
	printf("to get the lowest score possible. The way the game works is ");
	printf("that we start with a blank keyboard and players take turns placing ");
	printf("keys. Every time you increase the cost of the keyboard, your ");
	printf("score increases by the difference between the new keyboard cost ");
	printf("and the previous keyboard cost.\n");
	printf("\tAt each round you type in a character, a space, and the position ");
	printf("at which you want to place the character. For example if you wanted ");
	printf("to put the letter 'c' at position 5, you would type in \"c 5\".");
	printf("\n\n");
	
	int divisor = 10000;
	char input[1000];
	
	int p2_computer = TRUE;
	char difficulty = '\0';
	
	do {
		printf("Play against a human or the computer? (h/c) ");
		fgets(input, 999, stdin);
		if (input[0] == 'h') {
			p2_computer = FALSE;
			printf("Human it is.\n\n");
		} else {
			printf("Computer it is. Select a difficulty level (a number from 0 to 9): ");
			fgets(input, 999, stdin);
			difficulty = input[0];
			if (difficulty >= '0' && difficulty <= '4')
				printf("Difficulty set to %c. Good luck!\n\n", difficulty);
			else if (difficulty >= '5' && difficulty <= '7')
				printf("You're in for a challenge. Good luck!\n\n");
			else if (difficulty == '8')
				printf("I hope you know what you're getting yourself into. Good luck...\n\n");
			else if (difficulty == '9')
				printf("You must be crazy to take on that kind of difficulty level. Good luck!\n\n");
			else if (difficulty == 'x')
				printf("Secret impossible mode enabled. If you manage to beat this level, please tell me how you did it.\n\n");
			else {
				printf("That is not a valid input. Please try again.\n\n");
				continue;
			}
		}
	} while (FALSE);
	
	int p1 = 0, p2 = 1;
	int64_t score[2];
	score[0] = 0; score[1] = 0;
	
	Keyboard k;
	copy(&k, &nilKeyboard);
	
	calcFitness(&k);
	
	int prev_fitness = k.fitness;
	
	int keynum;
	for (keynum = 0; keynum < ksize; ++keynum) {
		printLayoutOnly(&k);
		
		if (p2_computer && keynum % 2 == p2) {
			gameComputer(&k, difficulty);
		} else {
		
			printf("Player %d, type in a character and a position: ", keynum % 2 + 1);
			fgets(input, 999, stdin);
			if (strlen(input) < 3) {
				printf("Invalid input. Please try again.\n");
				--keynum;
				continue;
			}
			char c = input[0];
			int pos = atoi(input + 2);
			
			if (k.layout[pos]) {
				printf("That position is occupied. Please try again.\n");
				--keynum;
				continue;
			} else if (locWithoutShifted(&k, c) != -1) {
				printf("That character has already been placed. Please try again.\n");
				--keynum;
				continue;
			} else k.layout[pos] = c;
		
		}
		
		calcFitness(&k);
		
		/* The value added to the player's score is the difference of the keyboard's current 
		 * fitness and its previous fitness.
		 */
		score[keynum % 2] += (k.fitness - prev_fitness) / divisor;
		prev_fitness = k.fitness;
		
		printf("Player 1 score: %lld\n", score[p1]);
		printf("Player 2 score: %lld\n\n", score[p2]);
	}
	
	printf("Layout final fitness: %lld\n", k.fitness);
	printLayoutOnly(&k);
	printf("\n");
	
	if (score[p1] < score[p2]) {
		printf("Player 1 wins!\n");
	} else if (score[p1] > score[p2]) {
		printf("Player 2 wins!\n");
	} else {
		printf("It's a tie!\n");
	}
	
	printf("\n");
	
	return 0;
}

int gameComputer(Keyboard *k, char difficulty)
{
	int bestp = -1;
	char bestc = '\0';
	int64_t score, bestScore = INT64_MAX;
	
	Keyboard k2;
	
	int indices[2 * trueksize];
	buildShuffledIndices(indices, 2 * trueksize);
	
	int i, j, inx, total = 0, done = FALSE;
	for (i = 0; i < monLen && !done; ++i) {
		if (locWithoutShifted(k, monKeys[i]) != -1) continue;
		
		for (j = 0; j < ksize && !done; ++j) {
			inx = indices[j];
			if (k->layout[inx]) continue;
			
			copy(&k2, k);
			k2.layout[inx] = monKeys[i];
			
			calcFitness(&k2);
			score = k2.fitness - k->fitness;
			if (score < bestScore) {
				bestp = inx;
				bestc = monKeys[i];
				bestScore = score;
			}
			
			++total;
			switch (difficulty) {
			case '0': 
				if (total >=  2) done = TRUE; break;
			case '1': 
				if (total >=  5) done = TRUE; break;
			case '2': 
				if (total >= 12) done = TRUE; break;
			case '3': 
				if (total >= 20) done = TRUE; break;
			case '4': 
				if (total >= 30) done = TRUE; break;
			case '5': 
				if (total >= 45) done = TRUE; break;
			case '6': 
				if (total >= 65) done = TRUE; break;
			case '7': 
				if (total >= 90) done = TRUE; break;
			case '8': 
				if (total >=130) done = TRUE; break;
			case '9':
				if (total >=200) done = TRUE; break;
			default: 
				break;
			}
		}
	}
	
	if (bestp >= 0 && bestc != '\0') {
		printf("The computer puts %c at %d.\n", bestc, bestp);
		k->layout[bestp] = bestc;
	}
	
	if (bestp < 0)
		fprintf(stderr, "Error: In gameComputer(), uninitialized value bestp.\n");
	if (bestc != '\0')
		fprintf(stderr, "Error: In gameComputer(), uninitialized value bestc.\n");		

	return 0;	
}

/* Calculates worst digraphs for the first keyboard in the given file. */
int worstDigraphsFromFile(const char *const filename, int damagingp)
{
	FILE *file = fopen(filename, "r");
	CHECK_FILE_FOR_NULL(file, filename);
	Keyboard k;
	if (layoutFromFile(file, &k) >= 0) {
		printf("Keyboard Layout:\n");
		printLayoutOnly(&k);
		worstDigraphs(&k, damagingp);
	}
	
	return 0;
}

int worstDigraphs(Keyboard *k, int damagingp)
{
	int i;
	for (i = 0; i < FINGER_COUNT; ++i) k->fingerUsage[i] = 0;
	
	char keys[diLen][2];
	memcpy(keys, diKeys, sizeof(char) * diLen * 2);
	
	int64_t values[diLen];

	for (i = 0; i < diLen; ++i) {
		k->distance	    = 0;
		k->inRoll       = 0;
		k->outRoll      = 0;
		k->sameHand     = 0;
		k->sameFinger   = 0;
		k->rowChange    = 0;
		k->homeJump     = 0;
		k->toCenter     = 0;
		k->toOutside    = 0;
		int locs[2];
		
		locs[0] = locWithoutShifted(k, diKeys[i][0]);
		locs[1] = locWithoutShifted(k, diKeys[i][1]);
		
		// These all require that the hand be the same.
		if (hand[locs[0]] == hand[locs[1]]) {
			k->inRoll     = calcInRoll    (locs[0], locs[1]);	
			k->outRoll    = calcOutRoll   (locs[0], locs[1]);	
			k->sameHand   = sameHand                        ;
			k->sameFinger = calcSameFinger(locs[0], locs[1]);
			k->rowChange  = calcRowChange (locs[0], locs[1]);
			k->homeJump   = calcHomeJump  (locs[0], locs[1]);
			k->toCenter   = calcToCenter  (locs[0], locs[1]);
			k->toOutside  = calcToOutside (locs[0], locs[1]);
		}
		k->distance = (distanceCosts[locs[0]] + distanceCosts[locs[1]]) * distance;

		/* Re-assign diValues[i] to the cost of that digraph. */
		values[i] = k->distance + k->inRoll + k->outRoll + k->sameHand + k->sameFinger + k->rowChange + k->homeJump + k->toCenter + k->toOutside;
		
		/* This function will tell you...
		 * Without this line: Which digraphs have the worst score.
		 * With this line: Which digraphs are the most damaging, based on both score and frequency.
		 */
		if (damagingp)
			values[i] *= diValues[i];
	}
	
	sortDigraphs(keys, values, 0, diLen - 1);
	
	for (i = 0; i < diLen; ++i) {
		char buf1[5];
		char buf2[5];
		charToPrintable(buf1, keys[i][0], FALSE);
		charToPrintable(buf2, keys[i][1], FALSE);
		
		printf("%s%s = %lld\n", buf1, buf2, values[i]);
		
	}
		
	return 0;	
}

int bestSwap(Keyboard *k)
{
	printf("Sorry, this function is not yet implemented.\n");
	return 0;
}

/* 
 * Read in each layout from the file and print out detailed information.
 */
int compare(const char *const filename)
{
	FILE *file = fopen(filename, "r");
	CHECK_FILE_FOR_NULL(file, filename);
	int ret = 1;
	while (ret != EOF && ret >= 0) {
		Keyboard k;
		ret = layoutFromFile(file, &k);
		if (ret >= 0) {
			printPercentages(&k);
		}
	}
	
	fclose(file);
	printf("\n");
	
	return 0;
}


/* 
 * Improves each layout in the given file to the maximum extent possible.
 */
int improveFromFile(const char *const filename)
{
	FILE *file = fopen(filename, "r");
	CHECK_FILE_FOR_NULL(file, filename);
	Keyboard k;
	if (layoutFromFile(file, &k) != -1) {
		printf("Layout to Improve:\n");
		improver(k);
	} else {
		fprintf(stderr, "Error: File %s does not contain a valid keyboard.\n\n", filename);
	}
    
    return 0;
}

/*
 * Improves k to the maximum extent possible.
 */
Keyboard improver(Keyboard k)
{
	printPercentages(&k);
	
	Keyboard tk, bestk;
	copy(&tk, &k);
	copy(&bestk, &k);
	int64_t bestEval = k.fitness;
	int64_t curEval = anneal(&tk, NULL, 0);
	
	if (curEval < bestEval) {
		copy(&bestk, &tk);
		bestEval = curEval;
		printPercentages(&tk);
	}

	return bestk;
}

/* 
 * Uses compileTypingData() to create files for typing data.
 */
int makeTypingData()
{
	static const char *diFilenames[] = {
		"freq_types/digraphs_00allProse.txt", 
		"freq_types/digraphs_01allCasual.txt", 
		"freq_types/digraphs_02allC.txt", 
		"freq_types/digraphs_02allJava.txt", 
		"freq_types/digraphs_02allPerl.txt", 
		"freq_types/digraphs_02allRuby.txt", 
		"freq_types/digraphs_03allFormal.txt", 
		"freq_types/digraphs_04allNews.txt", 
	};
	
	static const char *charFilenames[] = {
		"freq_types/chars_00allProse.txt", 
		"freq_types/chars_01allCasual.txt", 
		"freq_types/chars_02allC.txt", 
		"freq_types/chars_02allJava.txt", 
		"freq_types/chars_02allPerl.txt", 
		"freq_types/chars_02allRuby.txt", 
		"freq_types/chars_03allFormal.txt", 
		"freq_types/chars_04allNews.txt", 
	};
	
	int multipliers[8];
	
	printf("There are several types of text. You must specify an integer multiplier for each one. The multiplier ");
	printf("will determine how heavily to weight that file. For example if you do a lot of Java programming ");
	printf("you might set the Java multiplier to a relatively large number, whereas if you don't program at all you would set ");
	printf("the Java multiplier to 0. You may also use the default multipliers.\n");
	printf("CAUTION: If you set any multipliers to be greater than about 100, this will cause integer overflow and the program  ");
	printf("will not work properly.\n");
	printf("The list of text types is { prose, casual, C, Java, Perl, Ruby, formal, news }.\n\n");
	
	char input[100];
	
	printf("Use default multipliers? (y/n) ");
	fgets(input, 100, stdin);
	
	if (input[0] == 'y' || input[0] == 'Y') {
		multipliers[0] = 18;
		multipliers[1] = 25;
		multipliers[2] =  5;
		multipliers[3] =  6;
		multipliers[4] =  2;
		multipliers[5] =  2;
		multipliers[6] = 15;
		multipliers[7] = 20;
	} else {
		multipliers[0] = getNumber("prose: ");
		multipliers[1] = getNumber("casual: ");
		multipliers[2] = getNumber("C: ");
		multipliers[3] = getNumber("Java: ");
		multipliers[4] = getNumber("Perl: ");
		multipliers[5] = getNumber("Ruby: ");
		multipliers[6] = getNumber("formal: ");
		multipliers[7] = getNumber("news: ");
	}
	
	printf("\nPlease specify the maximum number of strings to put in the file. The optimizer will run faster ");
	printf("with fewer strings and more accurately with more strings. (The recommended number is 1000-2000.)\n");
	int max = getNumber("max: ");
	
	compileTypingData("allDigraphs.txt", diFilenames, multipliers, 8, 2, max);
	compileTypingData("allChars.txt", charFilenames, multipliers, 8, 1, max);
	
	printf("\nDone writing typing data. See allChars.txt and allDigraphs.txt for the result.\n\n");
	
	return 0;
}

int getNumber(char *description)
{
	char input[100];
	int num;
	
	do {
		printf("%s", description);
		fgets(input, 100, stdin);
		if (streq(input, "0")) {
			return 0;
		}
		num = atoi(input);
		if (num == 0) {
			printf("Undefined input. Please try again.\n");
		}
	} while (num == 0);
	
	return num;
}

/*
 *  TIMING DATA
 *  
 *  Updated 10 Sep 2012, run on a MacBook Pro 2.7 GHz
 * 
 *  Main-30 Keyboard: 
 *  
 *  Time to initialize 100,000 keyboards:   448,000 microseconds.
 *  Time to mutate 100,000 keyboards:       293,000 microseconds.
 *  Time to copy 100,000 keyboards:          11,000 microseconds.
 *  Time to score 100,000 keyboards:      1,477,000 microseconds.
 *  Time to do locW/OShifted() 100,000 times: 8,000 microseconds.
 *  Time to do rand30() 100,000 times:          700 microseconds.
 *  
 *  
 *  Full Standard Keyboard: 
 *  
 *  Time to initialize 100,000 keyboards:  1,130,000 microseconds.
 *  Time to mutate 100,000 keyboards:        294,000 microseconds.
 *  Time to copy 100,000 keyboards:           20,000 microseconds.
 *  Time to score 100,000 keyboards:       3,910,000 microseconds.
 *  Time to do locW/OShifted() 100,000 times: 15,000 microseconds.
 *  
 *  
 *  This program can score about 60,000 layouts per second. This is about 20 
 *  times as fast as Michael Capewell's program (http://www.michaelcapewell.com/projects/keyboard/) 
 *  when you adjust for clock speed, and many times faster than 
 *  Peter Klausler's program (http://klausler.com/evolved.html)
 *  
 */
int runTimingTests()
{
	int start, startsec;
	int i, j;
	Keyboard tester;
	Keyboard array[1024];
	
	int initAverage = 0;
	int copyAverage = 0;
	int fitnessAverage = 0;
	int locAverage = 0;
	
	struct timeval tv;
		
	/* Use a longer loop to get more accurate data. */
	for (j = 0; j < 50; j++) {
		// How long does it take to initialize a keyboard?
		gettimeofday(&tv, NULL);
		start = tv.tv_usec;
		startsec = tv.tv_sec;
		for (i = 0; i < 100000; ++i) initKeyboard(&tester);
		gettimeofday(&tv, NULL);
		initAverage = ((initAverage * j) + tv.tv_usec - start + 1000000*(tv.tv_sec - startsec)) / (j + 1);
		
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
		for (i = 0; i < 100000; ++i) locWithoutShifted(&tester, 'a');
		gettimeofday(&tv, NULL);
		locAverage = ((locAverage * j) + tv.tv_usec - start + 1000000*(tv.tv_sec - startsec)) / (j + 1);
	}
	printf(" *  Time to initialize 100,000 keyboards:     %d microseconds.\n", initAverage);
	printf(" *  Time to copy 100,000 keyboards:           %d microseconds.\n", copyAverage);
	printf(" *  Time to score 100,000 keyboards:          %d microseconds.\n", fitnessAverage);
	printf(" *  Time to do locW/OShifted() 100,000 times: %d microseconds.\n", locAverage);
	
	return 0;
}

/* 
 * Make sure that all of the fitness functions are working correctly. A bug in there 
 * would be hard to notice without explicitly testing it.
 * 1: There is a bug in one of the fitness functions.
 */
int testFitness()
{
	if (fullKeyboard == FK_NO) {
		printf("\nTesting calcInRoll():\n");
		testResult(calcInRoll(0, 0), 0);
		testResult(calcInRoll(10, 11), inRoll);
		testResult(calcInRoll(14, 15), 0);
		testResult(calcInRoll(29, 28), inRoll);
		testResult(calcInRoll(3, 4), 0);
		testResult(calcInRoll(4, 3), 0);
		testResult(calcInRoll(11, 2), 0);
		testResult(calcInRoll(2, 11), 0);
		
		printf("\nTesting calcOutRoll():\n");
		testResult(calcOutRoll(0, 0), 0);
		testResult(calcOutRoll(11, 10), outRoll);
		testResult(calcOutRoll(15, 14), 0);
		testResult(calcOutRoll(28, 29), outRoll);
		testResult(calcOutRoll(3, 4), 0);
		testResult(calcOutRoll(14, 13), 0);
		
		printf("\nTesting calcSameFinger():\n");
		testResult(calcSameFinger(0, 0), 0);
		testResult(calcSameFinger(10, 0), sameFingerP);
		testResult(calcSameFinger(11, 21), sameFingerR);
		testResult(calcSameFinger(7, 27), sameFingerM);
		testResult(calcSameFinger(13, 4), sameFingerI);
		testResult(calcSameFinger(0, 4), 0);
		
		printf("\nTesting calcRowChange():\n");
		testResult(calcRowChange(8, 8), 0);
		testResult(calcRowChange(10, 0), rowChangeUp);
		testResult(calcRowChange(11, 0), rowChangeUp + handWarp);
		testResult(calcRowChange(13, 2), rowChangeUp + handSmooth);
		testResult(calcRowChange(8, 16), rowChangeDown);
		testResult(calcRowChange(8, 26), rowChangeDown);
		testResult(calcRowChange(28, 9), rowChangeUp + handWarp);
		testResult(calcRowChange(25, 5), rowChangeUp);
		
		printf("\nTesting calcHomeJump():\n");
		testResult(calcHomeJump(8, 8), 0);
		testResult(calcHomeJump(10, 0), 0);
		testResult(calcHomeJump(11, 0), 0);
		testResult(calcHomeJump(10, 1), 0);
		testResult(calcHomeJump(8, 16), 0);
		testResult(calcHomeJump(8, 26), homeJump + homeJumpIndex);
		testResult(calcHomeJump(28, 9), homeJump);
		testResult(calcHomeJump(25, 5), homeJump);
		
		printf("\nTesting calcToCenter():\n");
		testResult(calcToCenter(0, 0), 0);
		testResult(calcToCenter(13, 14), toCenter);
		testResult(calcToCenter(17, 15), toCenter);
		testResult(calcToCenter(4, 1), toCenter);
		testResult(calcToCenter(25, 9), toCenter);
		testResult(calcToCenter(25, 15), 0);
	
	} else if (fullKeyboard == FK_STANDARD) {
		printf("\nTesting calcHomeJump():\n");
		testResult(calcHomeJump(8, 8), 0);
		testResult(calcHomeJump(14, 0), 0);
		testResult(calcHomeJump(15, 0), 0);
		testResult(calcHomeJump(14, 1), 0);
		testResult(calcHomeJump(36, 9), 0);
		testResult(calcHomeJump(48, 9), doubleJump + homeJumpIndex);
		testResult(calcHomeJump(49, 23), homeJump + homeJumpIndex);
		testResult(calcHomeJump(44, 18), homeJump);
		testResult(calcHomeJump(44, 3), doubleJump);
		
	
	} else {
		printf("\nTesting calcInRoll():\n");
		testResult(calcInRoll(0, 0), 0);
		testResult(calcInRoll(2, 3), inRoll);
		testResult(calcInRoll(2, 14), 0);
		testResult(calcInRoll(13, 3), 0);
		testResult(calcInRoll(29, 28), inRoll);
		testResult(calcInRoll(28, 27), 0);
		
		printf("\nTesting calcSameFinger():\n");
		testResult(calcSameFinger(0, 0), 0);
		testResult(calcSameFinger(11, 0), sameFingerP);
		testResult(calcSameFinger(12, 23), sameFingerR);
		testResult(calcSameFinger(7, 29), sameFingerM);
		testResult(calcSameFinger(25, 15), sameFingerI);
		testResult(calcSameFinger(0, 4), 0);

		printf("\nTesting calcHomeJump():\n");
		testResult(calcHomeJump(8, 8), 0);
		testResult(calcHomeJump(11, 0), 0);
		testResult(calcHomeJump(12, 0), 0);
		testResult(calcHomeJump(11, 1), 0);
		testResult(calcHomeJump(8, 17), 0);
		testResult(calcHomeJump(19, 39), homeJump + homeJumpIndex);
		testResult(calcHomeJump(41, 20), homeJump);
		testResult(calcHomeJump(38, 16), homeJump);
	}

	
	return 0;
}

int testResult(int result, int expected)
{
	if (result == expected) printf("Test succeeded.\n");
	else printf("Test failed (%d expected, %d found).\n", expected, result);
	return 0;
}

