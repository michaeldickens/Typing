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
	
	getCommands();

	setksize(FK_STANDARD);
	Keyboard k;
	FILE *fp = fopen("fullLayoutStore.txt", "r");
	layoutFromFile(fp, &k);
	calcFitnessDirect(&k);
	printPercentages(&k);
	fclose(fp);
	greatToBest(&k);
	calcFitnessDirect(&k);
	printPercentages(&k);
		
	return 0;
}



