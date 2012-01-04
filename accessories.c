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
			printf("setfk <fk_setting>: Set the keyboard type. Type \"setfk help\" for more information.\n");
			printf("test fitness: Test that the fitness functions are working properly.\n");
			printf("worst <filename>: Find the worst digraphs for the keyboard layouts in <filename>.\n");
			printf("variables: Print all variables that can be modified.\n");
			printf("quit: Quit the keyboard optimization program.\n");
			printf("\n");

		} else if (streq(cmd, "algorithm")) {
			printf("Running the keyboard optimization algorithm. Press ctrl-C to abort.\n\n");
			run(kbd_filename);

		} else if (streqn(cmd, "best swap ", strlen("best swap "))) {
			char *filename = cmd + strlen("best swap ");
			FILE *fp = fopen(filename, "r");
			Keyboard k;
			if (layoutFromFile(fp, &k) != -1) {
				layoutFromFile(fp, &k);
				bestSwap(&k);
			}

			fclose(fp);

		} else if (streqn(cmd, "compare ", 8)) {
			compare(cmd + 8);

		} else if (streqn(cmd, "damaging ", 9)) {
			char *filename = cmd + 9;
			worstDigraphsFromFile(filename, TRUE);

		} else if (streq(cmd, "game")) {
			game();

		} else if (streqn(cmd, "get ", 4)) {
			getValue(cmd + 4);

		} else if (streqn(cmd, "improve ", 8)) {
			improveFromFile(cmd + 8);

		} else if (streq(cmd, "make typing data")) {
			makeTypingData();

		} else if (streqn(cmd, "set ", 4)) {
			setValue(cmd + 4);

		} else if (streqn(cmd, "setfk ", 6)) {
			if (streq(cmd+6, "no")) {
				setksize(FK_NO);
				printf("Keyboard set to non-full.\n\n");
			} else if (streq(cmd+6, "standard")) {
				setksize(FK_STANDARD);
				printf("Keyboard set to full standard.\n\n");
			} else if (streq(cmd+6, "kinesis")) {
				setksize(FK_KINESIS);
				printf("Keyboard set to full Kinesis.\n\n");
			} else if (streq(cmd+6, "iphone")) {
				setksize(FK_IPHONE);
				printf("Keyboard set to iPhone.\n\n");
			} else {
				printf("Undefined input. Valid inputs: \"setfk no\" (do not use full keyboard), \"setfk standard\" (use standard full keyboard), \"setfk kinesis\" (use Kinesis full keyboard).\n\n");
			}

		} else if (streq(cmd, "test fitness")) {
			testFitness();

		} else if (streq(cmd, "variables")) {
			printf("Boolean variables should be set to 0 for false and 1 for true. Variables not specified as booleans are integers.\n");
			printf("\t(bool) detailedOutput\n");
			printf("\t(bool) keepZXCV\n");
			printf("\t(bool) keepQWERTY\n");
			printf("\t(bool) keepNumbers\n");
			printf("\t(bool) keepParentheses\n");
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
			char *filename = cmd + 6;
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
			} else if (loc(&k, c) != -1) {
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
	int bestp; char bestc;
	int64_t score, bestScore = LLONG_MAX;
	
	Keyboard k2;
	
	shuffleIndices();
	
	int i, j, inx, total = 0, done = FALSE;
	for (i = 0; i < monLen && !done; ++i) {
		if (loc(k, monKeys[i]) != -1) continue;
		
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
	
	printf("The computer puts %c at %d.\n", bestc, bestp);
	k->layout[bestp] = bestc;
	return 0;
}

void worstDigraphsFromFile(char *filename, int damagingp)
{
	FILE *fp = fopen(filename, "r");
	Keyboard k;
	while (layoutFromFile(fp, &k) > 0) {
		printf("Keyboard Layout:\n");
		printLayoutOnly(&k);
		worstDigraphs(&k, damagingp);
	}
	
}

int worstDigraphs(Keyboard *k, int damagingp)
{
	int i;
	for (i = 0; i < 8; ++i) k->fingerUsage[i] = 0;
	
	char keys[diLen][2];
	memcpy(keys, diKeys, sizeof(char) * diLen * 2);
	
	int64_t values[diLen];

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
			k->sameHand		= sameHand						  ;
			k->sameFinger	= calcSameFinger(locs[0], locs[1]);
			k->rowChange	= calcRowChange (locs[0], locs[1]);
			k->homeJump		= calcHomeJump  (locs[0], locs[1]);
			k->toCenter		= calcToCenter  (locs[0], locs[1]);
			k->toOutside	= calcToOutside (locs[0], locs[1]);
		}
		k->distance = (distanceCosts[locs[0]] + distanceCosts[locs[1]]) * distance;

		// Re-assign diValues[i] to the cost of that digraph.
		values[i] = k->distance + k->inRoll + k->outRoll + k->sameHand + k->sameFinger + k->rowChange + k->homeJump + k->toCenter + k->toOutside;
		
		/* This function will tell you...
		 * Without this line: Which digraphs have the worst score.
		 * With this line: Which digraphs are the most damaging, based on both score and frequency.
		 */
		if (damagingp)
			values[i] *= diValues[i];
	}
	
	sortDigraphs(keys, values, 0, diLen - 1);
		
	for (i = 0; i < diLen; ++i)
		printf("%c%c = %lld\n", keys[i][0], keys[i][1], values[i]);
		
	return 0;	
}

int sortDigraphs(char keys[][2], int64_t values[], int left, int right)
{	
	int64_t lltemp;
	char ctemp;
	
	int64_t pivot = values[(left + right) / 2];
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

int bestSwap(Keyboard *k)
{
	calcFitnessDirect(k);
	printPercentages(k);

	calcFitness(k);
	int64_t fitness = k->fitness;
	int64_t origFitness = fitness;
	int64_t bestFitness = fitness;
	int bestIndices[2];
	Keyboard bestKeyboard = *k;
	
	int i, j;
	for (i = 0; i < trueksize; i++) {
		for (j = i+1; j < trueksize; j++) {
			if (!printIt[i] || !printIt[j] || !isLegalSwap(indices[i], indices[j]))
				continue;
			
			swapChars(k->layout + indices[i], k->layout + indices[j]);
			calcFitness(k);
			
			if (k->fitness < bestFitness) {
				bestFitness = k->fitness;
				bestKeyboard = *k;
				bestIndices[0] = indices[i];
				bestIndices[1] = indices[j];
			}
			
			/* Print out all swaps that are a certain percentage better. */
			if ((origFitness - k->fitness) / ((double) origFitness) > 0.05) {
				calcFitnessDirect(k);
				printPercentages(k);
			}

			swapChars(k->layout + indices[i], k->layout + indices[j]);
		}
	}
	
	printf("swap: %c and %c\n", k->layout[bestIndices[0]], k->layout[bestIndices[1]]);
	calcFitnessDirect(&bestKeyboard);
	printPercentages(&bestKeyboard);
	return 0;
}

/* 
 * Read in each layout from the file and print out detailed information.
 */
int compare(char *filename)
{
	FILE *fp = fopen(filename, "r");
	char c = '2';
	while (c != EOF && c != 0) {
		Keyboard k;
		c = layoutFromFile(fp, &k);
		if (c != 0) {
			calcFitnessDirect(&k);
			printPercentages(&k);
		}
	}
	
	fclose(fp);
	printf("\n");
	
	return 0;
}


/* 
 * Improves each layout in the given file to the maximum extent possible.
 */
void improveFromFile(char *filename)
{
	FILE *fp = fopen(filename, "r");
	Keyboard k, imp;
	if (layoutFromFile(fp, &k) != -1) {
		printf("Layout to Improve:\n");
		imp = improver(k);
	} else {
		fprintf(stderr, "Error: File %s does not contain a valid keyboard.\n\n", filename);
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
	int64_t bestEval = k.fitness;
	int64_t curEval = doRun(&tk);
	
	if (curEval < bestEval) {
		copy(&bestk, &tk);
		bestEval = curEval;
		calcFitnessDirect(&tk);
		printPercentages(&tk);
	}

	return bestk;
}

/* 
 * Uses compileTypingData() to create files for typing data.
 */
int makeTypingData()
{
	char *diFilenames[] = {
		"freq_types/digraphs_00allProse.txt", 
		"freq_types/digraphs_01allCasual.txt", 
		"freq_types/digraphs_02allC.txt", 
		"freq_types/digraphs_02allJava.txt", 
		"freq_types/digraphs_02allPerl.txt", 
		"freq_types/digraphs_02allRuby.txt", 
		"freq_types/digraphs_03allFormal.txt", 
		"freq_types/digraphs_04allNews.txt", 
	};
	
	char *charFilenames[] = {
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
	
	compileTypingData("alldigraphs.txt", diFilenames, multipliers, 8, 2, "1234567890abcdefghijklmnopqrstuvwxyz,.)(_\";-'=/", max);
	compileTypingData("allchars.txt", charFilenames, multipliers, 8, 1, "1234567890abcdefghijklmnopqrstuvwxyz,.)(_\";-'=/", max);
	
	printf("\nDone writing typing data. See allchars.txt and alldigraphs.txt for the result.\n\n");
	
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
//int runTimingTests()
//{
//	int start, startsec;
//	int i, j;
//	Keyboard tester;
//	Keyboard array[1024];
//	
//	int initAverage = 0;
//	int mutateAverage = 0;
//	int sortAverage = 0;
//	int copyAverage = 0;
//	int fitnessAverage = 0;
//	int locAverage = 0;
//	int randAverage = 0;
//		
//	for (j = 0; j < 10; j++) {
//		// How long does it take to initialize a keyboard?
//		gettimeofday(&tv, NULL);
//		start = tv.tv_usec;
//		startsec = tv.tv_sec;
//		for (i = 0; i < 100000; ++i) initKeyboard(&tester);
//		gettimeofday(&tv, NULL);
//		initAverage = ((initAverage * j) + tv.tv_usec - start + 1000000*(tv.tv_sec - startsec)) / (j + 1);
//		
//		// How long does it take to mutate a keyboard?
//		gettimeofday(&tv, NULL);
//		start = tv.tv_usec;
//		startsec = tv.tv_sec;
//		for (i = 0; i < 100000; ++i) tester = mutate(tester);
//		gettimeofday(&tv, NULL);
//		mutateAverage = ((mutateAverage * j) + tv.tv_usec - start + 1000000*(tv.tv_sec - startsec)) / (j + 1);
//		
//		// This is causing bad access
//		// How long does it take to sort 1024 keyboards?
//		gettimeofday(&tv, NULL);
//		start = tv.tv_usec;
//		startsec = tv.tv_sec;
////		sortPool(array, 0, 1023);
//		gettimeofday(&tv, NULL);
//		sortAverage = ((sortAverage * j) + tv.tv_usec - start + 1000000*(tv.tv_sec - startsec)) / (j + 1);
//
//		// How long does it take to copy a keyboard?
//		gettimeofday(&tv, NULL);
//		start = tv.tv_usec;
//		startsec = tv.tv_sec;
//		for (i = 0; i < 100000; ++i) copy(&tester, &array[0]);
//		gettimeofday(&tv, NULL);
//		copyAverage = ((copyAverage * j) + tv.tv_usec - start + 1000000*(tv.tv_sec - startsec)) / (j + 1);
//
//		// How long does it take to score a keyboard?
//		gettimeofday(&tv, NULL);
//		start = tv.tv_usec;
//		startsec = tv.tv_sec;
//		for (i = 0; i < 100000; ++i) calcFitness(&tester);
//		gettimeofday(&tv, NULL);
//		fitnessAverage = ((fitnessAverage * j) + tv.tv_usec - start + 1000000*(tv.tv_sec - startsec)) / (j + 1);
//
//		// How long does it take to find a key on a keyboard?
//		gettimeofday(&tv, NULL);
//		start = tv.tv_usec;
//		startsec = tv.tv_sec;
//		for (i = 0; i < 100000; ++i) loc(&tester, 'a');
//		gettimeofday(&tv, NULL);
//		locAverage = ((locAverage * j) + tv.tv_usec - start + 1000000*(tv.tv_sec - startsec)) / (j + 1);
//
//		// How long does it take to generate a pseudorandom number from 0 to 29?
//		gettimeofday(&tv, NULL);
//		start = tv.tv_usec;
//		startsec = tv.tv_sec;
//		for (i = 0; i < 100000; ++i) rand30();
//		gettimeofday(&tv, NULL);
//		randAverage = ((randAverage * j) + tv.tv_usec - start + 1000000*(tv.tv_sec - startsec)) / (j + 1);
//	}
//	printf(" *  Time to initialize 100,000 keyboards:	%d microseconds.\n", initAverage);
//	printf(" *  Time to mutate 100,000 keyboards:		%d microseconds.\n", mutateAverage);
//	printf(" *  Time to sort 1024 keyboards:			%d microseconds.\n", sortAverage);
//	printf(" *  Time to copy 100,000 keyboards:			%d microseconds.\n", copyAverage);
//	printf(" *  Time to score 100,000 keyboards:		%d microseconds.\n", fitnessAverage);
//	printf(" *  Time to do loc() 100,000 times:			%d microseconds.\n", locAverage);
//	printf(" *  Time to do rand30() 100,000 times:		%d microseconds.\n", randAverage);
//	
//	return 0;
//}

/* 
 * Make sure that all of the fitness functions are working correctly. A bug in there 
 * would be hard to notice without explicitly testing it.
 * 1: There is a bug in one of the fitness functions.
 */
int testFitness()
{
	if (full_keyboard == FK_NO) {
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
	
	} else if (full_keyboard == FK_STANDARD) {
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

