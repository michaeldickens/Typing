/*
 *  main.c
 *  Typing
 *
 *  Created by Michael Dickens on 8/11/09.
 *
 */

#include <stdio.h>
#include "accessories.h"

int main()
{
	/* Initialize the typing data and the keyboard layout settings. */
	setksize(FK_NO);
//	getCommands();
	setksize(FK_STANDARD);
    compare("/Users/MTGAP/XCodeProjects/Typing4/Typing/layoutStore.txt");
//	runThreadedAlgorithm();
//    runCJAlgorithm("fullLayoutStore.txt");
	
	return 0;
}



