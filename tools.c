/*
 *  charsets.c
 *  Typing
 *
 *  Created by Michael Dickens on 8/7/09.
 *
 */

#include "tools.h"


int rand30()
{
	return rand() % 30;
}

void copyArray(int out[], int in[], int length)
{
	memcpy(out, in, length * sizeof(int));
}

// Reads in a layout from layoutstore.txt and prints it as a computer-readable layout.
int humanReadableToComputerReadable()
{
	FILE *f = fopen("layoutstore.txt", "r");
	int layoutsInStore = 15;
	
	char c;
	int i;
	printf("\t");
	for (i = 0; i < 30; ++i) {
		c = getc(f);
		printf("layouts[%d].layout[%d] = '", layoutsInStore, i);
		if (c == '\'') printf("\\"); // Turns ' into \'
		printf("%c'; ", c);
		if (i % 5 == 4) printf("\n\t");
	}
	
	return 0;
}

char layoutFromFile(FILE *fp, Keyboard *k)
{
	int i;
	char c;
	
	int readUntilN = FALSE;
	for (i = 0; (c = getc(fp)) != EOF && i < KSIZE; ++i)
		if (readUntilN == TRUE) { i = -1; if (c == '\n') readUntilN = FALSE; }
		else if (c == '#') { readUntilN = TRUE; i = -1; } // '#' acts as a commenting key that comments out the rest of the line.
		else if (c == '\n') { printf("Error: Keyboard layout is not %d characters.\n", KSIZE); copy(&k, &nilKeyboard); return -1; }
		else {
			while (i < KSIZE && printIt[i] == FALSE)
				++i;
			if (i < KSIZE) k->layout[i] = c;
		}
	
	k->layout[KSIZE] = '\0';
	while (c != EOF && c != '\n') c = getc(fp);
	
	calcFitness(k);
	
	return c;
}

int initData()
{
	initValues();

	int i;
	
	srand(time(NULL));
	
	for (i = 0; i < KSIZE; ++i)
		nilKeyboard.layout[i] = '2';
	nilKeyboard.layout[KSIZE] = '\0';
	nilKeyboard.fitness = 0;
	nilKeyboard.distance = 0;
	nilKeyboard.inRoll = 0;
	nilKeyboard.outRoll = 0;
	nilKeyboard.sameHand = 0;
	nilKeyboard.sameFinger = 0;
	nilKeyboard.rowChange = 0;
	nilKeyboard.homeJump = 0;
	nilKeyboard.toCenter = 0;
	nilKeyboard.toOutside = 0;
		
	if (FULL_KEYBOARD == FK_NO) {
		int fingerCopy[KSIZE] = {
			PINKY, RING, MIDDLE, INDEX, INDEX, INDEX, INDEX, MIDDLE, RING, PINKY, 
			PINKY, RING, MIDDLE, INDEX, INDEX, INDEX, INDEX, MIDDLE, RING, PINKY, 
			PINKY, RING, MIDDLE, INDEX, INDEX, INDEX, INDEX, MIDDLE, RING, PINKY, 
		};
		copyArray(finger, fingerCopy, KSIZE);
		
		int rowCopy[] = {
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
			1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
			2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 
		};
		copyArray(row, rowCopy, KSIZE);
		
		int handCopy[KSIZE] = {
			LEFT, LEFT, LEFT, LEFT, LEFT, RIGHT, RIGHT, RIGHT, RIGHT, RIGHT, 
			LEFT, LEFT, LEFT, LEFT, LEFT, RIGHT, RIGHT, RIGHT, RIGHT, RIGHT, 
			LEFT, LEFT, LEFT, LEFT, LEFT, RIGHT, RIGHT, RIGHT, RIGHT, RIGHT, 
		};
		copyArray(hand, handCopy, KSIZE);
		
		int isCenterCopy[KSIZE] = {
			FALSE, FALSE, FALSE,  FALSE, TRUE, TRUE, FALSE, FALSE, FALSE, FALSE, 
			FALSE, FALSE, FALSE,  FALSE, TRUE, TRUE, FALSE, FALSE, FALSE, FALSE, 
			FALSE, FALSE, FALSE,  FALSE, TRUE, TRUE, FALSE, FALSE, FALSE, FALSE, 
		};
		copyArray(isCenter, isCenterCopy, KSIZE);
		
		for (i = 0; i < KSIZE; ++i)
			isOutside[i] = FALSE;
		
		int printItCopy[KSIZE] = {
			TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, 
			TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, 
			TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, 
		};
		copyArray(printIt, printItCopy, KSIZE);
	
	} else if (FULL_KEYBOARD == FK_STANDARD) {
		int fingerCopy[KSIZE] = {
			PINKY, PINKY, RING, MIDDLE, INDEX, INDEX, INDEX, INDEX, MIDDLE, RING, PINKY, PINKY, PINKY, PINKY, 
			PINKY, PINKY, RING, MIDDLE, INDEX, INDEX, INDEX, INDEX, MIDDLE, RING, PINKY, PINKY, PINKY, PINKY, 
			PINKY, PINKY, RING, MIDDLE, INDEX, INDEX, INDEX, INDEX, MIDDLE, RING, PINKY, PINKY, PINKY, PINKY, 
			PINKY, PINKY, RING, MIDDLE, INDEX, INDEX, INDEX, INDEX, MIDDLE, RING, PINKY, PINKY, PINKY, PINKY, 
		};
		copyArray(finger, fingerCopy, KSIZE);
		
		int rowCopy[] = {
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
			1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
			2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 
			3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 
		};
		copyArray(row, rowCopy, KSIZE);
		
		int handCopy[KSIZE] = {
			LEFT, LEFT, LEFT, LEFT, LEFT, LEFT, RIGHT, RIGHT, RIGHT, RIGHT, RIGHT, RIGHT, RIGHT, RIGHT, 
			LEFT, LEFT, LEFT, LEFT, LEFT, LEFT, RIGHT, RIGHT, RIGHT, RIGHT, RIGHT, RIGHT, RIGHT, RIGHT, 
			LEFT, LEFT, LEFT, LEFT, LEFT, LEFT, RIGHT, RIGHT, RIGHT, RIGHT, RIGHT, RIGHT, RIGHT, RIGHT, 
			LEFT, LEFT, LEFT, LEFT, LEFT, LEFT, RIGHT, RIGHT, RIGHT, RIGHT, RIGHT, RIGHT, RIGHT, RIGHT, 
		};
		copyArray(hand, handCopy, KSIZE);
		
		int isCenterCopy[KSIZE] = {
			FALSE, FALSE, FALSE, FALSE,  FALSE, TRUE, TRUE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 
			FALSE, FALSE, FALSE, FALSE,  FALSE, TRUE, TRUE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 
			FALSE, FALSE, FALSE, FALSE,  FALSE, TRUE, TRUE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 
			FALSE, FALSE, FALSE, FALSE,  FALSE, TRUE, TRUE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 
		};
		copyArray(isCenter, isCenterCopy, KSIZE);
		
		int isOutsideCopy[KSIZE] = {
			TRUE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, TRUE, TRUE, TRUE, 
			TRUE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, TRUE, TRUE, TRUE, 
			TRUE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, TRUE, TRUE, TRUE, 
			TRUE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, TRUE, TRUE, TRUE, 
		};
		copyArray(isOutside, isOutsideCopy, KSIZE);
		
		int printItCopy[KSIZE] = {
			TRUE , TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE , TRUE, FALSE, 
			FALSE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE , TRUE, TRUE , 
			FALSE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE , FALSE, FALSE, 
			FALSE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, FALSE, FALSE, FALSE, 
		};
		copyArray(printIt, printItCopy, KSIZE);
		
	} else {
		int fingerCopy[KSIZE] = {
			PINKY, RING, MIDDLE, INDEX, INDEX, INDEX, INDEX, MIDDLE, RING, PINKY, PINKY, 
			PINKY, RING, MIDDLE, INDEX, INDEX, INDEX, INDEX, MIDDLE, RING, PINKY, PINKY, 
			PINKY, RING, MIDDLE, INDEX, INDEX, INDEX, INDEX, MIDDLE, RING, PINKY, PINKY, 
			PINKY, RING, MIDDLE, INDEX, INDEX, INDEX, INDEX, MIDDLE, RING, PINKY, PINKY, 
			PINKY, RING, MIDDLE, INDEX, INDEX, INDEX, INDEX, MIDDLE, RING, PINKY, PINKY, 
		};
		copyArray(finger, fingerCopy, KSIZE);
		
		int rowCopy[KSIZE] = {
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
			1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
			2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 
			3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 
			4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 
		};
		copyArray(row, rowCopy, KSIZE);
		
		int handCopy[KSIZE] = {
			LEFT, LEFT, LEFT, LEFT, LEFT, RIGHT, RIGHT, RIGHT, RIGHT, RIGHT, RIGHT, 
			LEFT, LEFT, LEFT, LEFT, LEFT, RIGHT, RIGHT, RIGHT, RIGHT, RIGHT, RIGHT, 
			LEFT, LEFT, LEFT, LEFT, LEFT, RIGHT, RIGHT, RIGHT, RIGHT, RIGHT, RIGHT, 
			LEFT, LEFT, LEFT, LEFT, LEFT, RIGHT, RIGHT, RIGHT, RIGHT, RIGHT, RIGHT, 
			LEFT, LEFT, LEFT, LEFT, LEFT, RIGHT, RIGHT, RIGHT, RIGHT, RIGHT, RIGHT, 
		};
		copyArray(hand, handCopy, KSIZE);	
		
		int isCenterCopy[KSIZE] = {
			FALSE, FALSE, FALSE, FALSE, TRUE, TRUE, FALSE, FALSE, FALSE, FALSE, FALSE, 
			FALSE, FALSE, FALSE, FALSE, TRUE, TRUE, FALSE, FALSE, FALSE, FALSE, FALSE, 
			FALSE, FALSE, FALSE, FALSE, TRUE, TRUE, FALSE, FALSE, FALSE, FALSE, FALSE, 
			FALSE, FALSE, FALSE, FALSE, TRUE, TRUE, FALSE, FALSE, FALSE, FALSE, FALSE, 
			FALSE, FALSE, FALSE, FALSE, TRUE, TRUE, FALSE, FALSE, FALSE, FALSE, FALSE, 
		};
		copyArray(isCenter, isCenterCopy, KSIZE);
		
		int isOutsideCopy[KSIZE] = {
			FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, TRUE, 
			FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, TRUE, 
			FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, TRUE, 
			FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, TRUE, 
			FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, TRUE, 
		};
		copyArray(isOutside, isOutsideCopy, KSIZE);

		int printItCopy[KSIZE] = {
			TRUE , TRUE , TRUE , TRUE , TRUE , TRUE , TRUE , TRUE , TRUE , TRUE , TRUE , 
			TRUE , TRUE , TRUE , TRUE , TRUE , TRUE , TRUE , TRUE , TRUE , TRUE , TRUE , 
			TRUE , TRUE , TRUE , TRUE , TRUE , TRUE , TRUE , TRUE , TRUE , TRUE , TRUE , 
			TRUE , TRUE , TRUE , TRUE , TRUE , TRUE , TRUE , TRUE , TRUE , TRUE , FALSE, 
			TRUE , TRUE , FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, TRUE , TRUE , FALSE, 
		};
		copyArray(printIt, printItCopy, KSIZE);		
	}
	
	for (i = 0; i < KSIZE; ++i)
		isCenterOrOutside[i] = isCenter[i] || isOutside[i];

	qwerty[ 0] = 'q'; qwerty[ 1] = 'w'; qwerty[ 2] = 'e'; qwerty[ 3] = 'r'; qwerty[ 4] = 't'; 
	qwerty[ 5] = 'y'; qwerty[ 6] = 'u'; qwerty[ 7] = 'i'; qwerty[ 8] = 'o'; qwerty[ 9] = 'p'; 
	qwerty[10] = 'a'; qwerty[11] = 's'; qwerty[12] = 'd'; qwerty[13] = 'f'; qwerty[14] = 'g'; 
	qwerty[15] = 'h'; qwerty[16] = 'j'; qwerty[17] = 'k'; qwerty[18] = 'l'; qwerty[19] = ';'; 
	qwerty[20] = 'z'; qwerty[21] = 'x'; qwerty[22] = 'c'; qwerty[23] = 'v'; qwerty[24] = 'b'; 
	qwerty[25] = 'n'; qwerty[26] = 'm'; qwerty[27] = ','; qwerty[28] = '.'; qwerty[29] = '\''; 

	FILE *file;
	
	if (FULL_KEYBOARD == FK_NO) file = fopen("30digraphs.txt", "r");
	else file = fopen("alldigraphs.txt", "r");
	
	char c = '\0';
	i = 0;
	totalDi = 0;
	diLen = 0;
	while (c != EOF) {
		diKeys[i][0] = getc(file);
		diKeys[i][1] = getc(file);
		c = getc(file);
		
		diValues[i] = 0;
		while ((c = getc(file)) != EOF && c >= '0' && c <= '9') {
			diValues[i] *= 10;
			diValues[i] += c - '0';
		}
		
		diValues[i] /= DIVISOR;
		totalDi += diValues[i++];
		while (c != EOF && c != '\n')
			c = getc(file);
	}

	diLen = i;
	fclose(file);
	

	if (FULL_KEYBOARD == FK_NO) file = fopen("30chars.txt", "r");
	else file = fopen("allchars.txt", "r");
	
	c = '\0';
	i = 0;
	totalMon = 0;
	monLen = 0;
	while (c != EOF) {
		monKeys[i] = getc(file);
		c = getc(file);
		
		monValues[i] = 0;
		while ((c = getc(file)) != EOF && c >= '0' && c <= '9') {
			monValues[i] *= 10;
			monValues[i] += c - '0';
		}
		
		monValues[i] /= DIVISOR;
		totalMon += monValues[i++];
		while (c != EOF && c != '\n')
			c = getc(file);
	}
	
	monLen = i;
	fclose(file);
	
	return 0;
} // int initData();
