/*
 *  accessories.h
 *  Typing
 *
 *  Created by Michael Dickens on 8/11/09.
 *
 */
 
#include "algorithm.h"

void worstDigraphsFromFile(char *filename);
int worstDigraphs(Keyboard *k);
int sortDigraphs(char keys[][2], long long values[], int left, int right);

int compare(char *filename);

void improveFromFile(char *filename);
Keyboard improver(Keyboard k);

int makeTypingData();
int getNumber(char *description);

//int runTimingTests();

int testFitness();
int testResult(int result, int expected);