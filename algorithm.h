/*
 *  algorithm.h
 *  Typing
 *
 *  Created by Michael Dickens on 8/9/09.
 *
 */

#include <stdlib.h>
#include <limits.h>
#include "keyboard.h"

int initAlgorithm();
Keyboard runAlgorithm();
Keyboard runForOne();
int runLoop(Keyboard pool[]);
int sortPool(Keyboard pool[], int left, int right);

// See tools.h for declaration of mutate().