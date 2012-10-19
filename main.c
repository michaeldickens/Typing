/*
 *  main.c
 *  Typing
 *
 *  Created by Michael Dickens on 8/11/09.
 *
 */

#include <stdio.h>
#include "accessories.h"

void timeRun()
{
    char str[100];
    sprintf(str, "performance data/GTB_%d_inc_%d.txt", GTB_ROUNDS, 
            RUNS_BEFORE_GTB_ROUNDS_INC);
    FILE *fp = fopen(str, "w");
    int i;
    for (i = 0; i < 200; ++i) {
        time_t start = time(NULL);
        runAlgorithm();
        time_t finish = time(NULL);
        fprintf(fp, "%3ld,", finish - start);
    }
    fprintf(fp, "\n");
    fclose(fp);
}



int main()
{
	/* Initialize the typing data and the keyboard layout settings. */
    setksize(FK_NO);
    getCommands();
	
	return 0;
}

