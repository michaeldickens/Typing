/*
 *  charsets.c
 *  Typing
 *
 *  Created by Michael Dickens on 8/7/09.
 *
 */

#include "tools.h"


void copyArray(int dest[], int src[], int length)
{
	memcpy(dest, src, length * sizeof(int));
}

void printTime(time_t start)
{
    time_t finish = time(NULL);
    printf("Time elapsed: %ld hours, %ld minutes, %ld seconds\n",
           (finish-start) / 3600, ((finish - start) % 3600) / 60,
           (finish-start) % 60);
}

int initData()
{
	initValues();
	
	int i, j;
	
	srand((unsigned int) time(NULL));
	
	for (i = 0; i <= ksize; ++i)
		nilKeyboard.layout[i] = nilKeyboard.shiftedLayout[i] = '\0';
	nilKeyboard.fitness = FITNESS_MAX;
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
	
	for (i = 0; i < 5; ++i)
		for (j = 0; j < 5; ++j) {
			rowChangeTableDown[i][j] = rowChangeDown;
			rowChangeTableUp[i][j] = rowChangeUp;
			if (i != THUMB && j != THUMB) {
				if (i == MIDDLE && j == INDEX) rowChangeTableDown[i][j] += handSmooth;
				else if (abs(i - j) == 1)      rowChangeTableDown[i][j] += handWarp;
				
				if (i == INDEX && j == MIDDLE) rowChangeTableUp[i][j] += handSmooth;
				else if (abs(i - j) == 1)      rowChangeTableUp[i][j] += handWarp;
			}
		}

	if (fullKeyboard == FK_NO) strcpy(keysToInclude, DEFAULT_KEYBOARD_30);
	else if (fullKeyboard == FK_STANDARD) strcpy(keysToInclude, DEFAULT_KEYBOARD_STANDARD);
	else if (fullKeyboard == FK_KINESIS) strcpy(keysToInclude, DEFAULT_KEYBOARD_KINESIS);
	
	initKeyboardData();
	initTypingData();
	
	return 0;
} // int initData();

void initKeyboardData()
{
	int i;
	
	char *ptrToOne = strchr(keysToInclude, '1');
	if (ptrToOne) numStart = (int) (ptrToOne - keysToInclude);
	else numStart = -1;
	
	if (fullKeyboard == FK_NO) {
		static int fingerCopy[KSIZE_MAX] = {
			PINKY, RING, MIDDLE, INDEX, INDEX, INDEX, INDEX, MIDDLE, RING, PINKY, 
			PINKY, RING, MIDDLE, INDEX, INDEX, INDEX, INDEX, MIDDLE, RING, PINKY, 
			PINKY, RING, MIDDLE, INDEX, INDEX, INDEX, INDEX, MIDDLE, RING, PINKY, 
		};
		copyArray(finger, fingerCopy, ksize);
		
		static int columnCopy[KSIZE_MAX] = {
			0, 1, 2, 3, 4, 4, 3, 2, 1, 0, 
			0, 1, 2, 3, 4, 4, 3, 2, 1, 0, 
			0, 1, 2, 3, 4, 4, 3, 2, 1, 0, 
		};
		copyArray(column, columnCopy, ksize);
		
		static int rowCopy[] = {
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
			1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
			2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 
		};
		copyArray(row, rowCopy, ksize);
		
		homeRow = 1;
		
		static int handCopy[KSIZE_MAX] = {
			LEFT, LEFT, LEFT, LEFT, LEFT, RIGHT, RIGHT, RIGHT, RIGHT, RIGHT, 
			LEFT, LEFT, LEFT, LEFT, LEFT, RIGHT, RIGHT, RIGHT, RIGHT, RIGHT, 
			LEFT, LEFT, LEFT, LEFT, LEFT, RIGHT, RIGHT, RIGHT, RIGHT, RIGHT, 
		};
		copyArray(hand, handCopy, ksize);
		
		static int isCenterCopy[KSIZE_MAX] = {
			FALSE, FALSE, FALSE,  FALSE, TRUE, TRUE, FALSE, FALSE, FALSE, FALSE, 
			FALSE, FALSE, FALSE,  FALSE, TRUE, TRUE, FALSE, FALSE, FALSE, FALSE, 
			FALSE, FALSE, FALSE,  FALSE, TRUE, TRUE, FALSE, FALSE, FALSE, FALSE, 
		};
		copyArray(isCenter, isCenterCopy, ksize);
		
		for (i = 0; i < KSIZE_MAX; ++i)
			isOutside[i] = FALSE;
		
		static int printableCopy[KSIZE_MAX] = {
			TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, 
			TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, 
			TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, 
		};
		copyArray(printable, printableCopy, ksize);
	
	} else if (fullKeyboard == FK_STANDARD) {
		static int fingerCopy[KSIZE_MAX] = {
			PINKY, PINKY, RING,  MIDDLE, INDEX, INDEX, INDEX, INDEX, MIDDLE, RING,  PINKY, PINKY, PINKY, PINKY, 
			PINKY, PINKY, RING,  MIDDLE, INDEX, INDEX, INDEX, INDEX, MIDDLE, RING,  PINKY, PINKY, PINKY, PINKY, 
			PINKY, PINKY, RING,  MIDDLE, INDEX, INDEX, INDEX, INDEX, MIDDLE, RING,  PINKY, PINKY, PINKY, PINKY, 
			PINKY, PINKY, RING,  MIDDLE, INDEX, INDEX, INDEX, INDEX, MIDDLE, RING,  PINKY, PINKY, PINKY, PINKY, 
			THUMB, THUMB, THUMB, THUMB,  THUMB, THUMB, THUMB, THUMB, THUMB,  THUMB, THUMB, THUMB, THUMB, THUMB, 
		};
		copyArray(finger, fingerCopy, ksize);
		
		static int columnCopy[KSIZE_MAX] = {
			-1, 0, 1, 2, 3, 4, 4, 3, 2, 1, 0, -1, -2, -3, 
			-1, 0, 1, 2, 3, 4, 4, 3, 2, 1, 0, -1, -2, -3, 
			-1, 0, 1, 2, 3, 4, 4, 3, 2, 1, 0, -1, -2, -3, 
			-1, 0, 1, 2, 3, 4, 4, 3, 2, 1, 0, -1, -2, -3, 
			-1, 0, 1, 2, 3, 4, 4, 3, 2, 1, 0, -1, -2, -3, 
		};
		copyArray(column, columnCopy, ksize);
		
		static int rowCopy[] = {
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
			1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
			2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 
			3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 
			4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, /* thumb row */
		};
		copyArray(row, rowCopy, ksize);
		
		homeRow = 2;
		
		static int handCopy[KSIZE_MAX] = {
			LEFT, LEFT, LEFT, LEFT, LEFT, LEFT, RIGHT, RIGHT, RIGHT, RIGHT, RIGHT, RIGHT, RIGHT, RIGHT, 
			LEFT, LEFT, LEFT, LEFT, LEFT, LEFT, RIGHT, RIGHT, RIGHT, RIGHT, RIGHT, RIGHT, RIGHT, RIGHT, 
			LEFT, LEFT, LEFT, LEFT, LEFT, LEFT, RIGHT, RIGHT, RIGHT, RIGHT, RIGHT, RIGHT, RIGHT, RIGHT, 
			LEFT, LEFT, LEFT, LEFT, LEFT, LEFT, RIGHT, RIGHT, RIGHT, RIGHT, RIGHT, RIGHT, RIGHT, RIGHT, 
			LEFT, LEFT, LEFT, LEFT, LEFT, LEFT, RIGHT, RIGHT, RIGHT, RIGHT, RIGHT, RIGHT, RIGHT, RIGHT, 
		};
		copyArray(hand, handCopy, ksize);
		
		static int isCenterCopy[KSIZE_MAX] = {
			FALSE, FALSE, FALSE, FALSE,  FALSE, TRUE, TRUE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 
			FALSE, FALSE, FALSE, FALSE,  FALSE, TRUE, TRUE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 
			FALSE, FALSE, FALSE, FALSE,  FALSE, TRUE, TRUE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 
			FALSE, FALSE, FALSE, FALSE,  FALSE, TRUE, TRUE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 
			FALSE, FALSE, FALSE, FALSE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 
		};
		copyArray(isCenter, isCenterCopy, ksize);
		
		static int isOutsideCopy[KSIZE_MAX] = {
			TRUE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, TRUE, TRUE, TRUE, 
			TRUE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, TRUE, TRUE, TRUE, 
			TRUE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, TRUE, TRUE, TRUE, 
			TRUE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, TRUE, TRUE, TRUE, 
			FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 
		};
		copyArray(isOutside, isOutsideCopy, ksize);
		
		static int printableCopy[KSIZE_MAX] = {
			TRUE,  TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE,  TRUE, FALSE, 
			FALSE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE,  TRUE, TRUE,  
			FALSE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE,  FALSE, FALSE, 
			FALSE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, FALSE, FALSE, FALSE, 
			TRUE,  FALSE,FALSE,FALSE,FALSE,FALSE,FALSE,FALSE,FALSE,FALSE,FALSE,FALSE, FALSE, FALSE,
		};
		copyArray(printable, printableCopy, ksize);
		
	} else if (fullKeyboard == FK_KINESIS) {
		static int fingerCopy[KSIZE_MAX] = {
			PINKY, PINKY, RING, MIDDLE, INDEX, INDEX, INDEX, INDEX, MIDDLE, RING, PINKY, PINKY, 
			PINKY, PINKY, RING, MIDDLE, INDEX, INDEX, INDEX, INDEX, MIDDLE, RING, PINKY, PINKY, 
			PINKY, PINKY, RING, MIDDLE, INDEX, INDEX, INDEX, INDEX, MIDDLE, RING, PINKY, PINKY, 
			PINKY, PINKY, RING, MIDDLE, INDEX, INDEX, INDEX, INDEX, MIDDLE, RING, PINKY, PINKY, 
			PINKY, PINKY, RING, MIDDLE, INDEX, INDEX, INDEX, INDEX, MIDDLE, RING, PINKY, PINKY, 
			THUMB, THUMB, THUMB, THUMB, THUMB, THUMB, THUMB, THUMB, THUMB, THUMB, THUMB, THUMB, 
		};
		copyArray(finger, fingerCopy, ksize);
		
		static int columnCopy[KSIZE_MAX] = {
			-1, 0, 1, 2, 3, 4, 4, 3, 2, 1, 0, -1,
			-1, 0, 1, 2, 3, 4, 4, 3, 2, 1, 0, -1,
			-1, 0, 1, 2, 3, 4, 4, 3, 2, 1, 0, -1,
			-1, 0, 1, 2, 3, 4, 4, 3, 2, 1, 0, -1,
			-1, 0, 1, 2, 3, 4, 4, 3, 2, 1, 0, -1,
			-1, 0, 1, 2, 3, 4, 4, 3, 2, 1, 0, -1,
		};
		copyArray(column, columnCopy, ksize);
		
		static int rowCopy[KSIZE_MAX] = {
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
			1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
			2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 
			3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 
			4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 
			5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, /* thumb row */
		};
		copyArray(row, rowCopy, ksize);
		
		homeRow = 2;
		
		static int handCopy[KSIZE_MAX] = {
			LEFT, LEFT, LEFT, LEFT, LEFT, LEFT, RIGHT, RIGHT, RIGHT, RIGHT, RIGHT, RIGHT, 
			LEFT, LEFT, LEFT, LEFT, LEFT, LEFT, RIGHT, RIGHT, RIGHT, RIGHT, RIGHT, RIGHT, 
			LEFT, LEFT, LEFT, LEFT, LEFT, LEFT, RIGHT, RIGHT, RIGHT, RIGHT, RIGHT, RIGHT, 
			LEFT, LEFT, LEFT, LEFT, LEFT, LEFT, RIGHT, RIGHT, RIGHT, RIGHT, RIGHT, RIGHT, 
			LEFT, LEFT, LEFT, LEFT, LEFT, LEFT, RIGHT, RIGHT, RIGHT, RIGHT, RIGHT, RIGHT, 
			LEFT, LEFT, LEFT, LEFT, LEFT, LEFT, RIGHT, RIGHT, RIGHT, RIGHT, RIGHT, RIGHT, 
		};
		copyArray(hand, handCopy, ksize);	
		
		static int isCenterCopy[KSIZE_MAX] = {
			FALSE, FALSE, FALSE, FALSE, FALSE, TRUE, TRUE, FALSE, FALSE, FALSE, FALSE, FALSE, 
			FALSE, FALSE, FALSE, FALSE, FALSE, TRUE, TRUE, FALSE, FALSE, FALSE, FALSE, FALSE, 
			FALSE, FALSE, FALSE, FALSE, FALSE, TRUE, TRUE, FALSE, FALSE, FALSE, FALSE, FALSE, 
			FALSE, FALSE, FALSE, FALSE, FALSE, TRUE, TRUE, FALSE, FALSE, FALSE, FALSE, FALSE, 
			FALSE, FALSE, FALSE, FALSE, FALSE, TRUE, TRUE, FALSE, FALSE, FALSE, FALSE, FALSE, 
			FALSE, FALSE, FALSE, FALSE, FALSE, TRUE, TRUE, FALSE, FALSE, FALSE, FALSE, FALSE, 
		};
		copyArray(isCenter, isCenterCopy, ksize);
		
		static int isOutsideCopy[KSIZE_MAX] = {
			TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, TRUE, 
			TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, TRUE, 
			TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, TRUE, 
			TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, TRUE, 
			TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, TRUE, 
			FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 
		};
		copyArray(isOutside, isOutsideCopy, ksize);

		static int printableCopy[KSIZE_MAX] = {
			FALSE, TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  
			TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  
			FALSE, TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  
			FALSE, TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  FALSE, 
			FALSE, TRUE,  TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, TRUE,  TRUE,  FALSE, 
			FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, TRUE,  TRUE,  
		};
		copyArray(printable, printableCopy, ksize);		

	} else if (fullKeyboard == FK_IPHONE) {
		static int fingerCopy[KSIZE_MAX] = {
			THUMB, THUMB, THUMB, THUMB, THUMB, THUMB, THUMB, THUMB, THUMB, THUMB, 
			THUMB, THUMB, THUMB, THUMB, THUMB, THUMB, THUMB, THUMB, THUMB, THUMB, 
			THUMB, THUMB, THUMB, THUMB, THUMB, THUMB, THUMB, THUMB, THUMB, THUMB, 
		};
		copyArray(finger, fingerCopy, ksize);
		
		static int rowCopy[] = {
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
		};
		copyArray(row, rowCopy, ksize);
		
		homeRow = 1;
		
		static int handCopy[KSIZE_MAX] = {
			LEFT, LEFT, LEFT, LEFT, LEFT, RIGHT, RIGHT, RIGHT, RIGHT, RIGHT, 
			LEFT, LEFT, LEFT, LEFT, LEFT, RIGHT, RIGHT, RIGHT, RIGHT, RIGHT, 
			LEFT, LEFT, LEFT, LEFT, LEFT, RIGHT, RIGHT, RIGHT, RIGHT, RIGHT, 
		};
		copyArray(hand, handCopy, ksize);
		
		static int isCenterCopy[KSIZE_MAX] = {
			FALSE, FALSE, FALSE,  FALSE, TRUE, TRUE, FALSE, FALSE, FALSE, FALSE, 
			FALSE, FALSE, FALSE,  FALSE, TRUE, TRUE, FALSE, FALSE, FALSE, FALSE, 
			FALSE, FALSE, FALSE,  FALSE, TRUE, TRUE, FALSE, FALSE, FALSE, FALSE, 
		};
		copyArray(isCenter, isCenterCopy, ksize);
		
		for (i = 0; i < KSIZE_MAX; ++i)
			isOutside[i] = FALSE;
		
		static int printableCopy[KSIZE_MAX] = {
			TRUE,  TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE,  TRUE,  
			TRUE,  TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE,  FALSE, 
			FALSE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, FALSE, FALSE, 
		};
		copyArray(printable, printableCopy, ksize);
	}
	
	for (i = 0; i < ksize; ++i)
		isCenterOrOutside[i] = isCenter[i] || isOutside[i];

}

/* Reads typing data from allChars.txt and allDigraphs.txt.
 * 
 * Produces monographs and digraphs involving backspace by assuming that all 
 * characters are equally mistyped.
 */
int initTypingData()
{
	int i;	
	FILE *file = fopen("allDigraphs.txt", "r");
	CHECK_FILE_FOR_NULL(file, "allDigraphs.txt");
	
	int c = '\0';
	i = 0;
	totalDi = 0;
	diLen = 0;
	while (TRUE) {
		/* Skip any extra newlines. */
		while ((c = getc(file)) == '\n')
			;
		if (c == EOF) break;

		if (c == '\\') c = convertEscapeChar(getc(file));
		if (c == 0) {
			fprintf(stderr, "Error: In digraph file, unknown escape character \\%c.\n", c);
			fclose(file);
			return 1;
		}
		diKeys[i][0] = c;

		c = getc(file);
		if (c == '\\') c = convertEscapeChar(getc(file));
		if (c == 0) {
			fprintf(stderr, "Error: In digraph file, unknown escape character \\%c.\n", c);
			fclose(file);
			return 1;
		}
		diKeys[i][1] = c;
		
		c = getc(file); /* Skip the space between the digraph and the value. */
		
		if (strchr(keysToInclude, diKeys[i][0]) && 
				strchr(keysToInclude, diKeys[i][1])) {
			diValues[i] = 0;
			while ((c = getc(file)) != EOF && c >= '0' && c <= '9') {
				diValues[i] *= 10;
				diValues[i] += c - '0';
			}
			
			diValues[i] /= DIVISOR;
			totalDi += diValues[i++];
			
			if (i >= MAX_DI_LEN)
				break;
		}

		/* Skip all extra characters. */
		while (c != EOF && c != '\n')
			c = getc(file);
	}
	
	diLen = i;
	fclose(file);
	

	file = fopen("allChars.txt", "r");
	CHECK_FILE_FOR_NULL(file, "allChars.txt");
	
	c = '\0';
	totalMon = 0;
	monLen = 0;
	for (i = 0; i < MAX_MON_LEN; ) {
		/* Skip any extra newlines. */
		while ((c = getc(file)) == '\n')
			;
		if (c == EOF) break;
		
		if (c == '\\') c = convertEscapeChar(getc(file));
		if (c == 0) {
			fprintf(stderr, "Error: In monograph file, unknown escape character \\%c.\n", c);
			fclose(file);
			return 1;
		}
		monKeys[i] = (char) c;
		
		c = getc(file); /* Skip the space between the char and the value. */
		
		if (strchr(keysToInclude, monKeys[i])) {
			monValues[i] = 0;
			while ((c = getc(file)) != EOF && c >= '0' && c <= '9') {
				monValues[i] *= 10;
				monValues[i] += c - '0';
			}
			
			monValues[i] /= DIVISOR;
			totalMon += monValues[i];

			++i;
		}
		
		/* Skip all extra characters. */
		while (c != EOF && c != '\n')
			c = getc(file);
	}
	
	monLen = i;
	fclose(file);
	
	/* If necessary, add the stats for backspace. */
	if (strchr(keysToInclude, '\b')) {
		/* Add backpace to the digraph list. */
		for (i = 0; i < monLen; ++i) {
			int64_t error_rate = monValues[i] * ERROR_RATE_PERCENT / 100;
			diKeys[diLen][0] = '\b';
			diKeys[diLen][1] = monKeys[i];
			diValues[diLen] = error_rate;
			++diLen;
			
			diKeys[diLen][1] = '\b';
			diKeys[diLen][0] = monKeys[i];
			diValues[diLen] = error_rate;
			++diLen;
		}
	 
		/* Add backspace to the monograph list. */
		monKeys[monLen] = '\b';
		monValues[monLen] = totalMon * ERROR_RATE_PERCENT / 100;
		++monLen;
	}
	
	sortDigraphs(diKeys, diValues, 0, diLen - 1);
	sortMonographs(monKeys, monValues, 0, monLen - 1);
	
	return 0;
}

/* 
 * Compile the typing data from each of the given files into  a single file.
 * 
 * outfileName: The file to which the new typing data will be written.
 * filenames: The names of each file to be read.
 * multipliers: The multipliers corresponding with the filenames.
 * length: Length of filenames and multipliers.
 * unit: The unit size of the strings to be read (characters=1, digraphs=2, etc).
 * max: The maximum number of strings that can be put into the file.
 * 
 * Return Codes
 * 1: Null file.
 */
int compileTypingData(char *outfileName, const char *filenames[], 
		int multipliers[], int length, int unit, int max)
{
	size_t size = 5000;
	/* Number of elements in keys and values. */
	int datalen = 0;

	int linelen = 100;
	char line[linelen];
	
	FILE *outfile = fopen(outfileName, "w");
	CHECK_FILE_FOR_NULL(outfile, outfileName);
	if (outfile == NULL) {
		fprintf(stderr, "Error: null file %s.\n", outfileName);
		return 1;
	}
	
	char **keys = malloc(sizeof(char *) * size);
	int *values = malloc(sizeof(int) * size);
	if (keys == NULL) {
		fprintf(stderr, "Error: In compileTypingData(), keys is null (malloc failure).\n");
		return 1;
	}
	if (values == NULL) {
		fprintf(stderr, "Error: In compileTypingData(), values is null (malloc failure).\n");
		return 1;
	}

	int i, k;
	for (i = 0; i < length; ++i) {
		printf("file %s,  multiplier %d\n", filenames[i], multipliers[i]);
		
		if (multipliers[i] == 0)
			continue;
		
		FILE *file = fopen(filenames[i], "r");
		if (file == NULL) {
			fprintf(stderr, "Error: In compileTypingData(), null file %s.\n", filenames[i]);
			fclose(outfile);
			free(keys);
			free(values);
			return 1;
		}
		
		while (fgets(line, linelen-1, file)) {			
			line[linelen-1] = '\0';
			if (datalen >= size) {
				size *= 2;
				keys = realloc(keys, sizeof(char) * size);
				values = realloc(values, sizeof(int) * size);
				
				if (keys == NULL) {
					fprintf(stderr, "Error: In compileTypingData(), keys is null (realloc failure).\n");
					return 1;
				}
				if (values == NULL) {
					fprintf(stderr, "Error: In compileTypingData(), values is null (realloc failure).\n");
					return 1;
				}
			}
			
			/* If the n-graph already exists, add to its value. */
			int found = FALSE;
			for (k = 0; k < datalen; ++k) {
				if (streqn(keys[k], line, unit)) {
					found = TRUE;
					values[k] += atoi(line + unit + 1) * multipliers[i];
				}
			}
			
			/* If the n-graph does not already exist, add it. */
			if (found == FALSE) {
				keys[datalen] = malloc(sizeof(char) * (unit + 1));
				if (keys[datalen] == NULL) {
					free(keys);
					free(values);
					fprintf(stderr, "Error: In compileTypingData(), keys[%d] is null (malloc failure).\n", datalen);
					return 1;
				}
				
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

/* Give left = 0 and right = (length of arrays minus 1). */
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
 * Accepts the following escape sequences: 
 * \n (newline)
 * \t (tab)
 * \s (shift)
 * \\ (backslash)
 * \b (backspace)
 * 
 * return 0: Unknown escape character.
 */
int convertEscapeChar(int c)
{
	if (c == 'n') {
		return '\n';
	} else if (c == 't') {
		return '\t';
	} else if (c == 's') {
		return ASCII_SHIFT;
	} else if (c == '\\') {
		return '\\';
	} else if (c == 'b') {
		return '\b'; /* backspace */
	} else return 0;
}

/* 
 * Takes a string containing a name and a value. Sets the variable with the given name to
 * the given value.
 */
int setValue(char *str)
{
	size_t len = strlen(str);
	
	char *name = str;
	char *valstr = strchr(str, ' ');

	if (len == 0 || valstr == NULL) {
		printf("No variable specified. Type \"variables\" for a complete listing of possible variables.\n\n");
		return 0;
	}

	*valstr = '\0'; ++valstr;
	
	int value = atoi(valstr);
	
	if (streq(name, "detailedOutput")) {
		detailedOutput = value;
	} else if (streq(name, "keepZXCV")) {
		keepZXCV = value;
	} else if (streq(name, "keepQWERTY")) {
		keepQWERTY = value;
	} else if (streq(name, "keepNumbers")) {
		keepNumbers = value;
	} else if (streq(name, "keepParentheses")) {
		keepParentheses = value;
	} else if (streq(name, "keepShiftPairs")) {
		keepShiftPairs = value;
	} else if (streq(name, "keepTab")) {
		keepTab = value;
	} else if (streq(name, "keepNumbersShifted")) {
		keepNumbersShifted = value;
    } else if (streq(name, "threadCount")) {
        threadCount = value;
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
	} else if (streq(name, "sameFingerT")) {
		sameFingerT = value;
	} else if (streq(name, "rowChangeUp")) {
		rowChangeUp = value;
	} else if (streq(name, "rowChangeDown")) {
		rowChangeDown = value;
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
    
    /* If the user changes a cost, it is necessary to redo 
     * preCalculateFitness().
     */
    if (USE_COST_ARRAY)
        preCalculateFitness();
	
	printf("%s set to %d.\n", name, value);
	
	return 0;
}

int getValue(char *name)
{
	if (streq(name, "detailedOutput")) {
		printf("%s = %d\n\n", name, detailedOutput);
	} else if (streq(name, "keepZXCV")) {
		printf("%s = %d\n\n", name, keepZXCV);
	} else if (streq(name, "keepQWERTY")) {
		printf("%s = %d\n\n", name, keepQWERTY);
	} else if (streq(name, "keepNumbers")) {
		printf("%s = %d\n\n", name, keepNumbers);
	} else if (streq(name, "keepParentheses")) {
		printf("%s = %d\n\n", name, keepParentheses);
	} else if (streq(name, "keepShiftPairs")) {
		printf("%s = %d\n\n", name, keepShiftPairs);
	} else if (streq(name, "keepTab")) {
		printf("%s = %d\n\n", name, keepTab);
	} else if (streq(name, "keepNumbersShifted")) {
		printf("%s = %d\n\n", name, keepNumbersShifted);
    } else if (streq(name, "threadCount")) {
		printf("%s = %d\n\n", name, threadCount);
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
	} else if (streq(name, "sameFingerT")) {
		printf("%s = %d\n\n", name, sameFingerT);
	} else if (streq(name, "rowChangeUp")) {
		printf("%s = %d\n\n", name, rowChangeUp);
	} else if (streq(name, "rowChangeDown")) {
		printf("%s = %d\n\n", name, rowChangeDown);
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

int sortDigraphs(char keys[][2], int64_t values[], int left, int right)
{	
	int64_t vtemp;
	char ctemp;
	
	int64_t pivot = values[(left + right) / 2];
	int i = left, j = right;
	do {
		while (values[i] > pivot) ++i;
		while (values[j] < pivot) --j;
		
		if (i <= j) {
			vtemp = values[i];
			values[i] = values[j];
			values[j] = vtemp;
			
			ctemp = keys[i][0];
			keys[i][0] = keys[j][0];
			keys[j][0] = ctemp;

			ctemp = keys[i][1];
			keys[i][1] = keys[j][1];
			keys[j][1] = ctemp;
			
			++i;
			--j;
		}
		
	} while (i <= j);
	
	if (i < right) sortDigraphs(keys, values, i, right);
	if (left < j) sortDigraphs(keys, values, left, j);
	
	return 0;
}

int sortMonographs(char keys[], int64_t values[], int left, int right)
{	
	int64_t vtemp;
	char ctemp;
	
	int64_t pivot = values[(left + right) / 2];
	int i = left, j = right;
	do {
		while (values[i] > pivot) ++i;
		while (values[j] < pivot) --j;
		
		if (i <= j) {
			vtemp = values[i];
			values[i] = values[j];
			values[j] = vtemp;
			
			ctemp = keys[i];
			keys[i] = keys[j];
			keys[j] = ctemp;
			
			++i;
			--j;
		}
		
	} while (i <= j);
	
	if (i < right) sortMonographs(keys, values, i, right);
	if (left < j) sortMonographs(keys, values, left, j);
	
	return 0;
}

/* Returns the matching bracket for c. If c is not a bracket, returns 0.
 */
char getMatchingBracket(char c)
{
	switch (c) {
	case '(':
		return ')';
	case ')':
		return '(';
	case '[':
		return ']';
	case ']':
		return '[';
	case '{':
		return '}';
	case '}':
		return '{';
	case '<':
		return '>';
	case '>':
		return '<';
	default:
		return 0;
	}
}

int alwaysKeepShiftPairP(char c)
{
	return isalpha(c) || isspace(c) || c == '\b';
}

void setksize(int type)
{
	fullKeyboard = type;
	
	switch (fullKeyboard) {
	case FK_NO:
		ksize = 30;
		trueksize = 30;
		kbdFilename = "layoutStore.txt";
		break;
	case FK_STANDARD:
		ksize = 56;
		trueksize = 47;
		kbdFilename = "fullLayoutStore.txt";
		break;
	case FK_KINESIS:
		ksize = 72;
		trueksize = 50;
		kbdFilename = "kinesisLayoutStore.txt";
		break;
	case FK_IPHONE:
		ksize = 30;
		trueksize = 26;
		kbdFilename = NULL;
		break;
	}

	initData();
	if (USE_COST_ARRAY)
		preCalculateFitness();
}
