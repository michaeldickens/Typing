/*
 *  main.m
 *  Typing
 *
 *  Created by Michael Dickens on 8/11/09.
 *
 */
 
#include <stdio.h>
#include "accessories.h"

int main(int argc, const char *argv[])
{			
	/* Initialize the typing data and the keyboard layout settings. */
	setksize(FK_NO);

	getCommands();
		
	return 0;
}

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

