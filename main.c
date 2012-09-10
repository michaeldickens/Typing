/*
 *  main.c
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
	
//	getCommands();
	
	setksize(FK_STANDARD);
	runCJAlgorithm("fullLayoutStore.txt");

	return 0;
}



