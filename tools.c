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
    
    strncpy(qwerty, "qwertyuiopasdfghjkl;zxcvbnm,./", 30);
	
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

	if (fullKeyboard == K_NO) strcpy(keysToInclude, DEFAULT_KEYBOARD_30);
	else if (fullKeyboard == K_STANDARD) strcpy(keysToInclude, DEFAULT_KEYBOARD_STANDARD);
	else if (fullKeyboard == K_KINESIS) strcpy(keysToInclude, DEFAULT_KEYBOARD_KINESIS);
	
	initKeyboardData();
	initTypingData();
	
	return 0;
} // int initData();

void initKeyboardData()
{
	int i;
	
	if (fullKeyboard == K_NO) {
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
	
	} else if (fullKeyboard == K_STANDARD) {
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
		
	} else if (fullKeyboard == K_KINESIS) {
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

	} else if (fullKeyboard == K_IPHONE) {
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
    
    /* Find firstNumberIndex. */
    char *ptrToOne = strchr(keysToInclude, '1');
	if (ptrToOne) firstNumberIndex = (int) (ptrToOne - keysToInclude);
	else firstNumberIndex = -1;
    
    /* Adjust firstNumberIndex to fix for non-printable indices. */
    if (firstNumberIndex >= 0) {
        int saved = firstNumberIndex;
        int i;
        for (i = 0; i <= saved; ++i)
            if (!printable[i]) ++firstNumberIndex;
    }

}

/* Reads typing data from allChars.txt and allDigraphs.txt.
 * 
 * Produces monographs and digraphs involving backspace by assuming that all 
 * characters are equally mistyped.
 */
int initTypingData()
{
	int i;	
	FILE *file = fopen(DIGRAPH_FILE, "r");
	CHECK_FILE_FOR_NULL(file, DIGRAPH_FILE);
	
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
			fprintf(stderr, "Error: In file %s, unknown escape character \\%c.\n",
                    DIGRAPH_FILE, c);
			fclose(file);
			return 1;
		}
		digraphs[i].key[0] = c;

		c = getc(file);
		if (c == '\\') c = convertEscapeChar(getc(file));
		if (c == 0) {
			fprintf(stderr, "Error: In file %s, unknown escape character \\%c.\n",
                    DIGRAPH_FILE, c);
			fclose(file);
			return 1;
		}
		digraphs[i].key[1] = c;
		
		c = getc(file); /* Skip the space between the digraph and the value. */
		
		if (strchr(keysToInclude, digraphs[i].key[0]) &&
				strchr(keysToInclude, digraphs[i].key[1])) {
			digraphs[i].value = 0;
			while ((c = getc(file)) != EOF && c >= '0' && c <= '9') {
				digraphs[i].value *= 10;
				digraphs[i].value += c - '0';
			}
			
			digraphs[i].value /= DIVISOR;
			totalDi += digraphs[i++].value;
			
			if (i >= DI_LEN_MAX)
				break;
		}

		/* Skip all extra characters. */
		while (c != EOF && c != '\n')
			c = getc(file);
	}
	
	diLen = i;
	fclose(file);
    
    if (diLen == 0)
        fprintf(stderr, "Warning: In file %s, no digraphs found.\n",
                DIGRAPH_FILE);
	

	file = fopen(MONOGRAPHFILE, "r");
	CHECK_FILE_FOR_NULL(file, MONOGRAPHFILE);
    
    initVariables();
	
	c = '\0';
	totalMon = 0;
	monLen = 0;
	for (i = 0; i < MON_LEN_MAX; ) {
		/* Skip any extra newlines. */
		while ((c = getc(file)) == '\n')
			;
		if (c == EOF) break;
		
		if (c == '\\') c = convertEscapeChar(getc(file));
		if (c == 0) {
			fprintf(stderr, "Error: In file %s, unknown escape character \\%c.\n",
                    MONOGRAPHFILE, c);
			fclose(file);
			return 1;
		}
		monographs[i].key = (char) c;
		
		c = getc(file); /* Skip the space between the char and the value. */
		
		if (strchr(keysToInclude, monographs[i].key)) {
			monographs[i].value = 0;
			while ((c = getc(file)) != EOF && c >= '0' && c <= '9') {
				monographs[i].value *= 10;
				monographs[i].value += c - '0';
			}
			
			monographs[i].value /= DIVISOR;
			totalMon += monographs[i].value;

			++i;
		}
		
		/* Skip all extra characters. */
		while (c != EOF && c != '\n')
			c = getc(file);
	}
	
	monLen = i;
	fclose(file);
    
    if (monLen == 0)
        fprintf(stderr, "Warning: In file %s, no monographs found.\n",
                MONOGRAPHFILE);
	
	/* If necessary, add the stats for backspace. */
	if (strchr(keysToInclude, '\b')) {
		/* Add backpace to the digraph list. */
		for (i = 0; i < monLen; ++i) {
			int64_t errorRate = monographs[i].value * ERROR_RATE_PERCENT / 100;
			digraphs[diLen].key[0] = '\b';
			digraphs[diLen].key[1] = monographs[i].key;
			digraphs[diLen].value = errorRate;
			++diLen;
			
			digraphs[diLen].key[0] = monographs[i].key;
			digraphs[diLen].key[1] = '\b';
			digraphs[diLen].value = errorRate;
			++diLen;
		}
	 
		/* Add backspace to the monograph list. */
		monographs[monLen].key = '\b';
		monographs[monLen].value = totalMon * ERROR_RATE_PERCENT / 100;
		++monLen;
	}
    
    qsort(monographs, monLen, sizeof(struct monograph), &cmpMonographsByValue);
    qsort(digraphs, diLen, sizeof(struct digraph), &cmpDigraphsByValue);
	
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
 * 
 * TODO: Refactor this to use monograph and digraph structs instead of two 
 *   parallel arrays of keys and values.
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

void initVariables()
{
    variablesLength = 0;
    
#define ADD_VAR(varName, desc) \
    variables[variablesLength].name = #varName; \
    variables[variablesLength].addr = &varName; \
    variables[variablesLength].description = desc; \
    ++variablesLength;
    
    ADD_VAR(detailedOutput, "(bool) provide additional information while running the algorithm");
    ADD_VAR(keepZXCV, "(bool) keep keys Z, X, C, and V in place");
    ADD_VAR(keepQWERTY, "(bool) try to keep keys in their QWERTY positions");
    ADD_VAR(keepNumbers, "(bool) keep numbers in place");
    ADD_VAR(keepBrackets, "(bool) keep brackets symmetrical");
    ADD_VAR(keepShiftPairs, "(bool) shifted/unshifted pairs of non-alphabetic characters stay together");
    ADD_VAR(keepTab, "(bool) keep Tab in place");
    ADD_VAR(keepNumbersShifted, "(bool) numbers do not move between shifted and unshifted");
    ADD_VAR(numThreads, "number of threads to create\n");
    ADD_VAR(distance, NULL);
    ADD_VAR(inRoll, NULL);
    ADD_VAR(outRoll, NULL);
    ADD_VAR(sameHand, NULL);
    ADD_VAR(sameFingerP, NULL);
    ADD_VAR(sameFingerR, NULL);
    ADD_VAR(sameFingerM, NULL);
    ADD_VAR(sameFingerI, NULL);
    ADD_VAR(rowChangeUp, NULL);
    ADD_VAR(rowChangeDown, NULL);
    ADD_VAR(handWarp, NULL);
    ADD_VAR(handSmooth, NULL);
    ADD_VAR(homeJump, NULL);
    ADD_VAR(homeJumpIndex, NULL);
    ADD_VAR(doubleJump, NULL);
    ADD_VAR(toCenter, NULL);
    ADD_VAR(toOutside, NULL);
    
#undef ADD_VAR
}

int getValue(const char *name)
{
    /* This is O(n), but it's a smallish n and speed is not essential. */
    int i;
    for (i = 0; i < variablesLength; ++i) {
        if (streq(name, variables[i].name)) {
            printf("%s = %d\n\n", variables[i].name, *variables[i].addr);
            return 0;
        }
    }
    
    printf("Unknown variable \"%s\". Type \"variables\" for a complete listing of possible variables.\n\n", name);
    return 1;
}

/*
 * Takes a string containing a name and a value. Sets the variable with the 
 * given name to the given value.
 * 
 * If the variable is successfully changed, returns 0. Otherwise, returns 
 * nonzero.
 */
int setValue(char *str)
{
	size_t len = strlen(str);
	
	char *name = str;
	char *valstr = strchr(str, ' ');
    
	if (len == 0 || valstr == NULL) {
		printf("No variable specified. Type \"variables\" for a complete listing of possible variables.\n\n");
		return 2;
	}
    
	*valstr = '\0'; ++valstr;
	
	int value = atoi(valstr);
    
    /* This is O(n), but it's a smallish n and speed is not essential. */
    int i;
    for (i = 0; i < variablesLength; ++i) {
        if (streq(name, variables[i].name)) {
            *variables[i].addr = value;
            /* If the user changes a cost, it is necessary to redo
             * preCalculateFitness().
             */
            if (USE_COST_ARRAY)
                preCalculateFitness();
            
            printf("%s set to %d.\n\n", name, value);
            return 0;
        }
    }
	
    printf("Unknown variable \"%s\". Type \"variables\" for a complete listing of possible variables.\n\n", name);
    return 1;
}

inline int cmpDigraphsByValue(const void *one, const void *two)
{
    int64_t val1 = ((struct digraph *) one)->value;
    int64_t val2 = ((struct digraph *) two)->value;
    if (val1 > val2) return -1;
    else if (val1 < val2) return 1;
    return 0;
}

inline int cmpMonographsByValue(const void *one, const void *two)
{
    int64_t val1 = ((struct monograph *) one)->value;
    int64_t val2 = ((struct monograph *) two)->value;
    if (val1 > val2) return -1;
    else if (val1 < val2) return 1;
    return 0;
}

/* Returns the matching bracket for c. If c is not a bracket, returns 0.
 */
inline char getMatchingBracket(char c)
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

/* 
 * If c should be kept in a pair with its shifted or unshifted character, 
 * returns true. Otherwise, returns false.
 * 
 * Alphabetical characters and whitespace are kept in shifted pairs. Other keys 
 * are not, unless the keepShiftPairs variable is set to TRUE.
 */
inline int keepShiftPair(char c)
{
	return keepShiftPairs || isalpha(c) || isspace(c) || c == '\b';
}

void setksize(int type)
{
	fullKeyboard = type;
	
	switch (fullKeyboard) {
	case K_NO:
		ksize = 30;
		trueksize = 30;
		kbdFilename = "layoutStore.txt";
		break;
	case K_STANDARD:
		ksize = 56;
		trueksize = 47;
		kbdFilename = "fullLayoutStore.txt";
		break;
	case K_KINESIS:
		ksize = 72;
		trueksize = 50;
		kbdFilename = "kinesisLayoutStore.txt";
		break;
	case K_IPHONE:
		ksize = 30;
		trueksize = 26;
		kbdFilename = NULL;
		break;
	}

	initData();
	if (USE_COST_ARRAY)
		preCalculateFitness();
}
