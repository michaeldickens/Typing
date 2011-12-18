/*
 *  accessories.h
 *  Typing
 *
 *  Created by Michael Dickens on 8/11/09.
 *
 */
 
#include "cjalgorithm.h"

int game();
int gameComputer(Keyboard *k, char difficulty);

/* 
 * damagingp: If true, finds the most damaging digraphs, i.e. takes into 
 *   account digraph frequency. If false, finds the worst digraphs without 
 *   respect to frequency.
 */
void worstDigraphsFromFile(char *filename, int damagingp);
int worstDigraphs(Keyboard *k, int damagingp);

int sortDigraphs(char keys[][2], int64_t values[], int left, int right);

/* 
 * Finds the single swap that will improve the layout the most.
 */
int bestSwap(Keyboard *k);

int compare(char *filename);

void improveFromFile(char *filename);
Keyboard improver(Keyboard k);

int makeTypingData();
int getNumber(char *description);

//int runTimingTests();

int testFitness();
int testResult(int result, int expected);