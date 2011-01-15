/*
 *  keyboard.h
 *  Typing
 *
 *  Created by Michael Dickens on 8/7/09.
 *
 * Includes the keyboard struct and the fitness function.
 */
 
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <math.h>
#include "tools.h"

/* Miscellaneous definitions */
#define IS_TRIGRAPH_THREE																		\
	if (sizeof(trigraph)/sizeof(char) != 3) {													\
		printf("The length of trigraph[] is %d, must be 3.", sizeof(trigraph)/sizeof(char));	\
		return -1;																				\
	}																							
	
#define IS_LOC_THREE																			\
	if (sizeof(locs)/sizeof(char) != 3) {														\
		printf("The length of locs[] is %d, must be 3.", sizeof(locs)/sizeof(char));			\
		return -1;																				\
	}																							


/* The shift value for simplePrintKeyboard(). */
#define SIMPLE_SHIFT 0 // 3

int initKeyboard(Keyboard *k);
int setLayout(Keyboard *k, char *layout);
int copy(Keyboard *k, Keyboard *original);
int swap(Keyboard *k, int loc1, int loc2);
int numberOfSameKeys(Keyboard *k, Keyboard *m);
int isEqual(Keyboard *k, Keyboard *m);
int printLayoutOnly(Keyboard *k);
int printKeyboard(Keyboard *k);
int printPercentages(Keyboard *k);
int simplePrintKeyboard(Keyboard *k);
int readLayouts(Keyboard pool[], int length);

int calcFitnessDirect(Keyboard *k);
int scoreDigraphDirect(Keyboard *k, char digraph[], int multiplier);
int calcFitness(Keyboard *k);
void scoreDigraph(Keyboard *k, char digraph[], int multiplier, int allLocs[]);
int calcShortcuts(Keyboard *k);
int calcQWERTY(Keyboard *k);
int calcParentheses(Keyboard *k);


void scoreTrigraph(Keyboard *k, char trigraph[], int multiplier, int allLocs[]);


char shortcutKeys[4];
char seedLayout[KSIZE]; // Holds every letter and is randomly shuffled. Used to seed the keyboards.
unsigned int seedVal;

int totalCalcFitness;

int calcDistance(int loc0, int loc1);
int calcInRoll(int loc0, int loc1);
int calcOutRoll(int loc0, int loc1);
int calcSameHand(int loc0, int loc1);
int calcSameFinger(int loc0, int loc1);
int calcRowChange(int loc0, int loc1);
int calcHomeJump(int loc0, int loc1);
int calcToCenter(int loc0, int loc1);
int calcToOutside(int loc0, int loc1);

int loc(Keyboard *k, char c);
