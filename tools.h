/*
 *  charsets.h
 *  Typing
 *
 *  Created by Michael Dickens on 8/7/09.
 *
 */
 
#include <time.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "values.h"
 
#define MAX_TRI_LEN 5000
#define MAX_DI_LEN 5000
#define MAX_MON_LEN 100

long long totalMon;
long long totalDi;
int monLen, diLen, triLen;

/* Constant definitions */

#define LEFT 0
#define RIGHT 1

#define PINKY 0
#define RING 1
#define MIDDLE 2
#define INDEX 3

/* Reduces monValues and diValues so as to prevent integer overflow. */
#define DIVISOR 100

typedef struct {
	char layout[KSIZE + 1]; /* The one extra character is set to '\0' so 
								(layout) can be treated as a string. */
	long long fingerUsage[8];
	long long shortcut;
	long long fitness;
	long long distance;
	long long inRoll;
	long long outRoll;
	long long sameHand;
	long long sameFinger;
	long long rowChange;
	long long homeJump;
	long long toCenter;
	long long toOutside;
} Keyboard;



Keyboard nilKeyboard;

/* These are declared here because C's file hierarchy is stupid.
 */
Keyboard atleMutate(Keyboard k);
Keyboard simpleMutate(Keyboard k);
Keyboard mutate(Keyboard k);

int rand30();
int mod30(int x);
void copyArray(int out[], int in[], int length);
int strNumsToArr(int arr[], char *str, int length);
char layoutFromFile(FILE *fp, Keyboard *k);
int initData();

int qwerty[30];

int randomizer;
struct timeval tv;
FILE *input;

int hand[KSIZE];
int finger[KSIZE];
int row[KSIZE];
int isCenter[KSIZE];
int isOutside[KSIZE];
int isCenterOrOutside[KSIZE];

int printIt[KSIZE];

int costs[900];

char triKeys[MAX_TRI_LEN][3];
long long triValues[MAX_TRI_LEN];

char diKeys[MAX_DI_LEN][2];
long long diValues[MAX_DI_LEN];

char monKeys[MAX_MON_LEN];
long long monValues[MAX_MON_LEN];

