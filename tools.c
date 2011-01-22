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

/* 
 * Copies a layout from fp onto k.
 * 
 * Return Codes
 * -1: Error.
 *  0: No new layout was found in the file.
 * anything else: The last character read.
 */
char layoutFromFile(FILE *fp, Keyboard *k)
{
	int i;
	char c;
	
	int readUntilN = FALSE, noNewKeyboard = TRUE;
	for (i = 0; (c = getc(fp)) != EOF && i < ksize; ++i)
		if (readUntilN == TRUE) { i = -1; if (c == '\n') readUntilN = FALSE; }
		else if (c == '#') { readUntilN = TRUE; i = -1; } // '#' comments out the rest of the line.
		else if (c == '\n') { 
			if (i > 0) {
				printf("Error: Keyboard layout is not %d characters.\n", ksize);
				copy(&k, &nilKeyboard);
				return -1; 
			} else i = -1;
		} else {
			noNewKeyboard = FALSE;
			while (i < ksize && printIt[i] == FALSE)
				++i;
			if (i < ksize) k->layout[i] = c;
		}
	
	k->layout[ksize] = '\0';
	while (c != EOF && c != '\n') c = getc(fp);
	
	if (noNewKeyboard)
		return 0;
	calcFitness(k);
	
	return c;
}

int initData()
{
	initValues();

	int i;
	
	srand(time(NULL));
	
	for (i = 0; i < ksize; ++i)
		nilKeyboard.layout[i] = '2';
	nilKeyboard.layout[ksize] = '\0';
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
	
	qwerty[ 0] = 'q'; qwerty[ 1] = 'w'; qwerty[ 2] = 'e'; qwerty[ 3] = 'r'; qwerty[ 4] = 't'; 
	qwerty[ 5] = 'y'; qwerty[ 6] = 'u'; qwerty[ 7] = 'i'; qwerty[ 8] = 'o'; qwerty[ 9] = 'p'; 
	qwerty[10] = 'a'; qwerty[11] = 's'; qwerty[12] = 'd'; qwerty[13] = 'f'; qwerty[14] = 'g'; 
	qwerty[15] = 'h'; qwerty[16] = 'j'; qwerty[17] = 'k'; qwerty[18] = 'l'; qwerty[19] = ';'; 
	qwerty[20] = 'z'; qwerty[21] = 'x'; qwerty[22] = 'c'; qwerty[23] = 'v'; qwerty[24] = 'b'; 
	qwerty[25] = 'n'; qwerty[26] = 'm'; qwerty[27] = ','; qwerty[28] = '.'; qwerty[29] = '\''; 

	initKeyboardData();
	
	initTypingData();
		
	
	return 0;
} // int initData();

void initKeyboardData()
{
	int i;
	
	if (full_keyboard == FK_NO) {
		int fingerCopy[KSIZE_MAX] = {
			PINKY, RING, MIDDLE, INDEX, INDEX, INDEX, INDEX, MIDDLE, RING, PINKY, 
			PINKY, RING, MIDDLE, INDEX, INDEX, INDEX, INDEX, MIDDLE, RING, PINKY, 
			PINKY, RING, MIDDLE, INDEX, INDEX, INDEX, INDEX, MIDDLE, RING, PINKY, 
		};
		copyArray(finger, fingerCopy, ksize);
		
		int rowCopy[] = {
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
			1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
			2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 
		};
		copyArray(row, rowCopy, ksize);
		
		int handCopy[KSIZE_MAX] = {
			LEFT, LEFT, LEFT, LEFT, LEFT, RIGHT, RIGHT, RIGHT, RIGHT, RIGHT, 
			LEFT, LEFT, LEFT, LEFT, LEFT, RIGHT, RIGHT, RIGHT, RIGHT, RIGHT, 
			LEFT, LEFT, LEFT, LEFT, LEFT, RIGHT, RIGHT, RIGHT, RIGHT, RIGHT, 
		};
		copyArray(hand, handCopy, ksize);
		
		int isCenterCopy[KSIZE_MAX] = {
			FALSE, FALSE, FALSE,  FALSE, TRUE, TRUE, FALSE, FALSE, FALSE, FALSE, 
			FALSE, FALSE, FALSE,  FALSE, TRUE, TRUE, FALSE, FALSE, FALSE, FALSE, 
			FALSE, FALSE, FALSE,  FALSE, TRUE, TRUE, FALSE, FALSE, FALSE, FALSE, 
		};
		copyArray(isCenter, isCenterCopy, ksize);
		
		for (i = 0; i < KSIZE_MAX; ++i)
			isOutside[i] = FALSE;
		
		int printItCopy[KSIZE_MAX] = {
			TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, 
			TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, 
			TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, 
		};
		copyArray(printIt, printItCopy, ksize);
	
		int indicesCopy[KSIZE_MAX] = {
			 0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 
			10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 
			20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 
		};
		copyArray(indices, indicesCopy, ksize);

	} else if (full_keyboard == FK_STANDARD) {
		int fingerCopy[KSIZE_MAX] = {
			PINKY, PINKY, RING, MIDDLE, INDEX, INDEX, INDEX, INDEX, MIDDLE, RING, PINKY, PINKY, PINKY, PINKY, 
			PINKY, PINKY, RING, MIDDLE, INDEX, INDEX, INDEX, INDEX, MIDDLE, RING, PINKY, PINKY, PINKY, PINKY, 
			PINKY, PINKY, RING, MIDDLE, INDEX, INDEX, INDEX, INDEX, MIDDLE, RING, PINKY, PINKY, PINKY, PINKY, 
			PINKY, PINKY, RING, MIDDLE, INDEX, INDEX, INDEX, INDEX, MIDDLE, RING, PINKY, PINKY, PINKY, PINKY, 
		};
		copyArray(finger, fingerCopy, ksize);
		
		int rowCopy[] = {
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
			1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
			2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 
			3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 
		};
		copyArray(row, rowCopy, ksize);
		
		int handCopy[KSIZE_MAX] = {
			LEFT, LEFT, LEFT, LEFT, LEFT, LEFT, RIGHT, RIGHT, RIGHT, RIGHT, RIGHT, RIGHT, RIGHT, RIGHT, 
			LEFT, LEFT, LEFT, LEFT, LEFT, LEFT, RIGHT, RIGHT, RIGHT, RIGHT, RIGHT, RIGHT, RIGHT, RIGHT, 
			LEFT, LEFT, LEFT, LEFT, LEFT, LEFT, RIGHT, RIGHT, RIGHT, RIGHT, RIGHT, RIGHT, RIGHT, RIGHT, 
			LEFT, LEFT, LEFT, LEFT, LEFT, LEFT, RIGHT, RIGHT, RIGHT, RIGHT, RIGHT, RIGHT, RIGHT, RIGHT, 
		};
		copyArray(hand, handCopy, ksize);
		
		int isCenterCopy[KSIZE_MAX] = {
			FALSE, FALSE, FALSE, FALSE,  FALSE, TRUE, TRUE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 
			FALSE, FALSE, FALSE, FALSE,  FALSE, TRUE, TRUE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 
			FALSE, FALSE, FALSE, FALSE,  FALSE, TRUE, TRUE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 
			FALSE, FALSE, FALSE, FALSE,  FALSE, TRUE, TRUE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 
		};
		copyArray(isCenter, isCenterCopy, ksize);
		
		int isOutsideCopy[KSIZE_MAX] = {
			TRUE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, TRUE, TRUE, TRUE, 
			TRUE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, TRUE, TRUE, TRUE, 
			TRUE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, TRUE, TRUE, TRUE, 
			TRUE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, TRUE, TRUE, TRUE, 
		};
		copyArray(isOutside, isOutsideCopy, ksize);
		
		int printItCopy[KSIZE_MAX] = {
			TRUE , TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE , TRUE, FALSE, 
			FALSE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE , TRUE, TRUE , 
			FALSE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE , FALSE, FALSE, 
			FALSE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, FALSE, FALSE, FALSE, 
		};
		copyArray(printIt, printItCopy, ksize);
		
		int indicesCopy[KSIZE_MAX] = {
			0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 
		       15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 
			   29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 
			   43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 
		};
		copyArray(indices, indicesCopy, ksize);
		
	} else {
		int fingerCopy[KSIZE_MAX] = {
			PINKY, RING, MIDDLE, INDEX, INDEX, INDEX, INDEX, MIDDLE, RING, PINKY, PINKY, 
			PINKY, RING, MIDDLE, INDEX, INDEX, INDEX, INDEX, MIDDLE, RING, PINKY, PINKY, 
			PINKY, RING, MIDDLE, INDEX, INDEX, INDEX, INDEX, MIDDLE, RING, PINKY, PINKY, 
			PINKY, RING, MIDDLE, INDEX, INDEX, INDEX, INDEX, MIDDLE, RING, PINKY, PINKY, 
			PINKY, RING, MIDDLE, INDEX, INDEX, INDEX, INDEX, MIDDLE, RING, PINKY, PINKY, 
		};
		copyArray(finger, fingerCopy, ksize);
		
		int rowCopy[KSIZE_MAX] = {
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
			1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
			2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 
			3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 
			4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 
		};
		copyArray(row, rowCopy, ksize);
		
		int handCopy[KSIZE_MAX] = {
			LEFT, LEFT, LEFT, LEFT, LEFT, RIGHT, RIGHT, RIGHT, RIGHT, RIGHT, RIGHT, 
			LEFT, LEFT, LEFT, LEFT, LEFT, RIGHT, RIGHT, RIGHT, RIGHT, RIGHT, RIGHT, 
			LEFT, LEFT, LEFT, LEFT, LEFT, RIGHT, RIGHT, RIGHT, RIGHT, RIGHT, RIGHT, 
			LEFT, LEFT, LEFT, LEFT, LEFT, RIGHT, RIGHT, RIGHT, RIGHT, RIGHT, RIGHT, 
			LEFT, LEFT, LEFT, LEFT, LEFT, RIGHT, RIGHT, RIGHT, RIGHT, RIGHT, RIGHT, 
		};
		copyArray(hand, handCopy, ksize);	
		
		int isCenterCopy[KSIZE_MAX] = {
			FALSE, FALSE, FALSE, FALSE, TRUE, TRUE, FALSE, FALSE, FALSE, FALSE, FALSE, 
			FALSE, FALSE, FALSE, FALSE, TRUE, TRUE, FALSE, FALSE, FALSE, FALSE, FALSE, 
			FALSE, FALSE, FALSE, FALSE, TRUE, TRUE, FALSE, FALSE, FALSE, FALSE, FALSE, 
			FALSE, FALSE, FALSE, FALSE, TRUE, TRUE, FALSE, FALSE, FALSE, FALSE, FALSE, 
			FALSE, FALSE, FALSE, FALSE, TRUE, TRUE, FALSE, FALSE, FALSE, FALSE, FALSE, 
		};
		copyArray(isCenter, isCenterCopy, ksize);
		
		int isOutsideCopy[KSIZE_MAX] = {
			FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, TRUE, 
			FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, TRUE, 
			FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, TRUE, 
			FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, TRUE, 
			FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, TRUE, 
		};
		copyArray(isOutside, isOutsideCopy, ksize);

		int printItCopy[KSIZE_MAX] = {
			TRUE , TRUE , TRUE , TRUE , TRUE , TRUE , TRUE , TRUE , TRUE , TRUE , TRUE , 
			TRUE , TRUE , TRUE , TRUE , TRUE , TRUE , TRUE , TRUE , TRUE , TRUE , TRUE , 
			TRUE , TRUE , TRUE , TRUE , TRUE , TRUE , TRUE , TRUE , TRUE , TRUE , TRUE , 
			TRUE , TRUE , TRUE , TRUE , TRUE , TRUE , TRUE , TRUE , TRUE , TRUE , FALSE, 
			TRUE , TRUE , FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, TRUE , TRUE , FALSE, 
		};
		copyArray(printIt, printItCopy, ksize);		

		int indicesCopy[KSIZE_MAX] = {
			 0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 
			11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 
			22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 
			33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 
			44, 45,                         52, 53, 
		};
		copyArray(indices, indicesCopy, ksize);
		
	}
	
	for (i = 0; i < ksize; ++i)
		isCenterOrOutside[i] = isCenter[i] || isOutside[i];

}

void initTypingData()
{
	int i;
	FILE *file;
	
	if (full_keyboard == FK_NO) file = fopen("30digraphs.txt", "r");
	else file = fopen("alldigraphs.txt", "r");
	
	char c = '\0';
	i = 0;
	totalDi = 0;
	diLen = 0;
	while (TRUE) {
		/* Skip any extra newlines. */
		while ((c = getc(file)) == '\n')
			;
		if (c == EOF) break;

		diKeys[i][0] = c;
		diKeys[i][1] = getc(file);
		c = getc(file); /* Read the space between the digraph and the value. */
		
		diValues[i] = 0;
		while ((c = getc(file)) != EOF && c >= '0' && c <= '9') {
			diValues[i] *= 10;
			diValues[i] += c - '0';
		}
		
		diValues[i] /= DIVISOR;
		totalDi += diValues[i++];
		
		/* Skip all extra characters. */
		while (c != EOF && c != '\n')
			c = getc(file);
	}
	
	diLen = i;
	fclose(file);
	

	if (full_keyboard == FK_NO) file = fopen("30chars.txt", "r");
	else file = fopen("allchars.txt", "r");
	
	c = '\0';
	i = 0;
	totalMon = 0;
	monLen = 0;
	while (TRUE) {
		/* Skip any extra newlines. */
		while ((c = getc(file)) == '\n')
			;
		if (c == EOF) break;

		monKeys[i] = c;
		c = getc(file);
		
		monValues[i] = 0;
		while ((c = getc(file)) != EOF && c >= '0' && c <= '9') {
			monValues[i] *= 10;
			monValues[i] += c - '0';
		}
		
		monValues[i] /= DIVISOR;
		totalMon += monValues[i++];
		/* Skip all extra characters. */
		while (c != EOF && c != '\n')
			c = getc(file);
	}
	
	monLen = i;
	fclose(file);
}

/* 
 * Compile the typing data from each of the given files into  a single file.
 * 
 * outfileName: The file to which the new typing data will be written.
 * filenames: The names of each file to be read.
 * multipliers: The multipliers corresponding with the filenames.
 * length: Length of filenames and multipliers.
 * unit: The unit size of the strings to be read (characters=1, digraphs=2, etc).
 * legalKeys: The keys to be included. For example if you only want to include letters, 
 *     legalKeys would be "abcdefghijklmnopqrstuvwxyz". If you want to include any 
 *     character, set legalKeys to NULL.
 * max: The maximum number of strings that can be put into the file.
 * 
 * Return Codes
 * 1: Null file.
 */
int compileTypingData(char *outfileName, char *filenames[], int multipliers[], int length, int unit, char *legalKeys, int max)
{
	size_t size = 5000;
	char **keys = malloc(sizeof(char *) * size);
	int *values = malloc(sizeof(int) * size);
	
	int linelen = 100;
	char line[linelen];
	
	FILE *outfile = fopen(outfileName, "w");
	if (outfile == NULL) {
		fprintf(stderr, "Error: null file %s.\n", outfileName);
		return 1;
	}
	
	int i, k, datalen = 0;
	for (i = 0; i < length; ++i) {
		printf("file %s,  multiplier %d\n", filenames[i], multipliers[i]);
		
		if (multipliers[i] == 0)
			continue;
		
		FILE *file = fopen(filenames[i], "r");
		if (file == NULL) {
			fprintf(stderr, "Error: null file %s.\n", filenames[i]);
			return 1;
		}
		
		while (fgets(line, linelen-1, file)) {
			int skip = FALSE;
			if (legalKeys)
				for (k = 0; k < unit; ++k)
					if (strchr(legalKeys, line[k]) == NULL) {
						skip = TRUE;
						break;
					}
			if (skip) continue;
			
			line[linelen-1] = '\0';
			if (datalen >= size) {
				size *= 2;
				keys = realloc(keys, sizeof(char) * size);
				values = realloc(values, sizeof(int) * size);
			}
			
			int found = FALSE;
			for (k = 0; k < datalen; ++k) {
				if (streqn(keys[k], line, unit)) {
					found = TRUE;
					values[k] += atoi(line + unit + 1) * multipliers[i];
				}
			}
			
			if (found == FALSE) {
				keys[datalen] = malloc(sizeof(char) * (unit + 1));
				strncpy(keys[datalen], line, unit);
				keys[datalen][unit] = '\0';
				values[k] = atoi(line + unit + 1) * multipliers[i];
				++datalen;
			}
			
		}
		
		fclose(file);
	}
	
	sortTypingData(keys, values, 0, datalen-1);
	
	for (i = 0; i < datalen && i < max; ++i) {
		strncpy(line, keys[i], unit);
		sprintf(line + unit, " %d\n", values[i]);
		
		fputs(line, outfile);
		free(keys[i]);
	}
	
	fclose(outfile);
	
	free(keys);
	free(values);
	
	return 0;
}

int sortTypingData(char **keys, int *values, int left, int right)
{
	int pivot = values[(left + right) / 2];
	int i = left, j = right;
	
	while (i <= j) {
		while (values[i] > pivot) ++i;
		while (values[j] < pivot) --j;
		
		if (i <= j) {
			char *ctemp = keys[i];
			keys[i] = keys[j];
			keys[j] = ctemp;
			int itemp = values[i];
			values[i] = values[j];
			values[j] = itemp;
			++i; --j;
		}
	}
	
	if (left < j) sortTypingData(keys, values, left, j);
	if (i < right) sortTypingData(keys, values, i, right);
	
	return 0;
}

/* 
 * Takes a string containing a name and a value. Sets the variable with the given name to
 * the given value.
 */
int setValue(char *str)
{
	int len = strlen(str);
	
	if (len == 0) {
		printf("No variable specified. Type \"variables\" for a complete listing of possible variables.\n\n");
		return 0;
	}
	
	char *name = str;
	char *valstr = strchr(str, ' ');
	*valstr = '\0'; ++valstr;
	
	int value = atoi(valstr);
	
	if (streq(name, "keepZXCV")) {
		keepZXCV = value;
	} else if (streq(name, "keepQWERTY")) {
		keepQWERTY = value;
	} else if (streq(name, "keepNumbers")) {
		keepNumbers = value;
	} else if (streq(name, "keepParentheses")) {
		keepParentheses = value;
	} else if (streq(name, "distance")) {
		distance = value;
	} else if (streq(name, "inRoll")) {
		inRoll = value;
	} else if (streq(name, "outRoll")) {
		outRoll = value;
	} else if (streq(name, "sameHand")) {
		sameHand = value;
	} else if (streq(name, "sameFingerP")) {
		sameFingerP = value;
	} else if (streq(name, "sameFingerR")) {
		sameFingerR = value;
	} else if (streq(name, "sameFingerM")) {
		sameFingerM = value;
	} else if (streq(name, "sameFingerI")) {
		sameFingerI = value;
	} else if (streq(name, "rowChange")) {
		rowChange = value;
	} else if (streq(name, "handWarp")) {
		handWarp = value;
	} else if (streq(name, "handSmooth")) {
		handSmooth = value;
	} else if (streq(name, "homeJump")) {
		homeJump = value;
	} else if (streq(name, "homeJumpIndex")) {
		homeJumpIndex = value;
	} else if (streq(name, "doubleJump")) {
		doubleJump = value;
	} else if (streq(name, "toCenter")) {
		toCenter = value;
	} else if (streq(name, "toOutside")) {
		toOutside = value;
	} else {
		printf("Unknown variable \"%s\". Type \"variables\" for a complete listing of possible variables.\n\n", name);
		return 1;
	}
	
	printf("%s set to %d.\n", name, value);
	
	return 0;
}

int getValue(char *name)
{
	if (streq(name, "keepZXCV")) {
		printf("%s = %d\n\n", name, keepZXCV);
	} else if (streq(name, "keepQWERTY")) {
		printf("%s = %d\n\n", name, keepQWERTY);
	} else if (streq(name, "keepNumbers")) {
		printf("%s = %d\n\n", name, keepNumbers);
	} else if (streq(name, "keepParentheses")) {
		printf("%s = %d\n\n", name, keepParentheses);
	} else if (streq(name, "distance")) {
		printf("%s = %d\n\n", name, distance);
	} else if (streq(name, "inRoll")) {
		printf("%s = %d\n\n", name, inRoll);
	} else if (streq(name, "outRoll")) {
		printf("%s = %d\n\n", name, outRoll);
	} else if (streq(name, "sameHand")) {
		printf("%s = %d\n\n", name, sameHand);
	} else if (streq(name, "sameFingerP")) {
		printf("%s = %d\n\n", name, sameFingerP);
	} else if (streq(name, "sameFingerR")) {
		printf("%s = %d\n\n", name, sameFingerR);
	} else if (streq(name, "sameFingerM")) {
		printf("%s = %d\n\n", name, sameFingerM);
	} else if (streq(name, "sameFingerI")) {
		printf("%s = %d\n\n", name, sameFingerI);
	} else if (streq(name, "rowChange")) {
		printf("%s = %d\n\n", name, rowChange);
	} else if (streq(name, "handWarp")) {
		printf("%s = %d\n\n", name, handWarp);
	} else if (streq(name, "handSmooth")) {
		printf("%s = %d\n\n", name, handSmooth);
	} else if (streq(name, "homeJump")) {
		printf("%s = %d\n\n", name, homeJump);
	} else if (streq(name, "homeJumpIndex")) {
		printf("%s = %d\n\n", name, homeJumpIndex);
	} else if (streq(name, "doubleJump")) {
		printf("%s = %d\n\n", name, doubleJump);
	} else if (streq(name, "toCenter")) {
		printf("%s = %d\n\n", name, toCenter);
	} else if (streq(name, "toOutside")) {
		printf("%s = %d\n\n", name, toOutside);
	} else {
		printf("Unknown variable \"%s\". Type \"variables\" for a complete listing of possible variables.\n\n", name);
		return 1;
	}
	
	return 0;
}

void setksize(int type)
{
	full_keyboard = type;
	
	switch (full_keyboard) {
	case FK_NO:
		ksize = 30;
		break;
	case FK_STANDARD:
		ksize = 56;
		break;
	case FK_KINESIS:
		ksize = 55;
		break;
	}
	
	initData();
}

