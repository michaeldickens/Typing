/*
 *  main.m
 *  Typing
 *
 *  Created by Michael Dickens on 8/11/09.
 *
 */
 
#include <stdio.h>
#include "cjalgorithm.h"

int main(int argc, const char *argv[])
{		
	full_keyboard = FK_NO;
	
	/* Initialize the typing data and the keyboard layout settings. */
	setksize(FK_NO);
	
	/* Initialize the genetic algorithm. */
	initAlgorithm();
	
	char *filename;
	if (full_keyboard == FK_NO) filename = "layoutstore.txt";
	else if (full_keyboard == FK_STANDARD) filename = "fulllayoutstore.txt";
	else if (full_keyboard == FK_KINESIS) filename = "kinesislayoutstore.txt";	

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
	
	int multipliers[] = { 18, 25, 5, 6, 2, 2, 15, 20 };

//	compileTypingData("alldigraphs.txt", diFilenames, multipliers, 8, 2, "1234567890abcdefghijklmnopqrstuvwxyz,.)(_\";-'=/", 1500);
//	compileTypingData("allchars.txt", charFilenames, multipliers, 8, 1, "1234567890abcdefghijklmnopqrstuvwxyz,.)(_\";-'=/", 1500);
		
	getCommands(filename);
	
//	run(filename);
//	runTimingTests();
//	compare(filename);
//	improveFromFile(filename);
//	worstDigraphsFromFile(filename);
//	testFitness();
	
	return 0;
}

int getCommands(char *filename)
{
	printf("Welcome to the Keyboard Layout Optimizer. If you have questions or comments, contact Michael Dickens by email (mtgandp@gmail.com) or leave a comment at http://mathematicalmulticore.wordpress.com/category/keyboards/.\n");
	printf("Type \"help\" for a listing of commands.\n\n");
	
	int length = 5000;
	char cmd[length];
	
	do {
		printf(">>> ");
		fgets(cmd, length, stdin);
		
		cmd[strlen(cmd)-1] = '\0'; // Remove the newline.
		
		if (streq(cmd, "help")) {
			printf("algorithm: Run the keyboard optimization algorithm.\n");
			printf("compare <filename>: Print information about the keyboards in <filename>. The keyboards must be in the proper format.\n");
			printf("improve <filename>: Try to improve the first keyboard in <filename>. The keyboard must be in the proper format.\n");
			printf("make typing data: Use the files in freq_types to customize character and digraph frequency.\n");
			printf("setfk <fk_setting>: Set the keyboard type. Type \"setfk\" for more information.\n");
			printf("quit: Quit the keyboard optimization program.\n");
			printf("\n");
			
		} else if (streq(cmd, "algorithm")) {
			printf("Running the keyboard optimization algorithm. Press ctrl-C to abort.\n\n");
			run(filename);
			
		} else if (streqn(cmd, "compare ", 8)) {
			compare(cmd + 8);
		
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
			} else {
				printf("Undefined input. Valid inputs: \"setfk no\" (do not use full keyboard), \"setfk standard\" (use standard full keyboard), \"setfk kinesis\" (use Kinesis full keyboard).\n\n");
			}
		
		} else if (streq(cmd, "variables")) {
			printf("\tkeepZXCV\n");
			printf("\tkeepQWERTY\n");
			printf("\tkeepNumbers\n");
			printf("\tkeepParentheses\n");
			printf("\tdistance\n");
			printf("\tinRoll\n");
			printf("\toutRoll\n");
			printf("\tsameHand\n");
			printf("\tsameFingerP\n");
			printf("\tsameFingerR\n");
			printf("\tsameFingerM\n");
			printf("\tsameFingerI\n");
			printf("\trowChange\n");
			printf("\thandWarp\n");
			printf("\thandSmooth\n");
			printf("\thomeJump\n");
			printf("\thomeJumpIndex\n");
			printf("\tdoubleJump\n");
			printf("\ttoCenter\n");
			printf("\ttoOutside\n");
					
		} else if (streq(cmd, "quit")) {
			printf("Goodbye!\n");
			break;
			
		} else {
			printf("Unknown command. Type \"help\" for a listing of commands.\n\n");
		}

		
	} while (strcmp(cmd, "exit") != 0);
	
	return 0;
}

