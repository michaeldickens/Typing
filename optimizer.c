/*
 *  optimizer.c
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
    setksize(K_NO);
    
    setksize(K_KINESIS);
    Keyboard k;
    initKeyboard(&k);
    for (int i = 0; i < 50; i++)
        anneal(&k, NULL, 0);

//    getCommands();
    
	return 0;
}

