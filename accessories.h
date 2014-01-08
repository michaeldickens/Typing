/*
 *  accessories.h
 *  Typing
 *
 *  Created by Michael Dickens on 8/11/09.
 *
 */

#ifndef __ACCESSORIES_H__
#define __ACCESSORIES_H__
 
#include "cjalgorithm.h"

int getCommands();

int game();
int gameComputer(Keyboard *k, char difficulty);

int worstDigraphsFromFile(const char *filename, int damagingp);
int worstDigraphs(Keyboard *k, int damagingp);

int bestSwap(Keyboard *k);

int compare(const char *filename);

int improveFromFile(const char *filename);
Keyboard improver(Keyboard k);

int makeTypingData();
int getInteger(const char *description);

int runTimingTests();

int testFitness();
int testResult(int result, int expected);

#endif
