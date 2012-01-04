/*
 *  keyboard.c
 *  Typing
 *
 *  Created by Michael Dickens on 8/7/09.
 *
 */

#include "keyboard.h"

int initKeyboard(Keyboard *k)
{
	int i;
	
	memset(k->layout, 0xff, ksize * sizeof(char));

	if (full_keyboard == FK_STANDARD) {
		// This default keyboard is QWERTY with the punctuation rearranged so as to use the 
		// most common punctuation marks: , . ) ( _ " ; - ' = /
		setLayout(k, "_1234567890-=qwertyuiop()\"asdfghjkl;'zxcvbnm,./");
	} else if (full_keyboard == FK_KINESIS) {
		// This default keyboard is QWERTY with the punctuation rearranged so as to use the 
		// most common punctuation marks: , . ) ( _ " ; - ' = /
		setLayout(k, "1234567890-qwertyuiop_asdfghjkl;\"zxcvbnm,./'=()");
	} else if (full_keyboard == FK_IPHONE) {
		setLayout(k, "qwertyuiopasdfghjklzxcvbnm");
	} else {
		// The main 30 characters, sorted by puncuation then by letter frequency.
		setLayout(k, "etaoinsrhldcumfpgywbvkxjqz,.';");
	}

	
	k->layout[ksize] = '\0';
		
	for (i = 0; i < 8; ++i) k->fingerUsage[i] = 0;
	
	shuffleLayout(k->layout);
	
	/* If keepZXCV is enabled, move ZXCV back to their positions. */
	if (keepZXCV && ksize == 30) {
		for (i = 0; i < ksize; ++i) {
			if (k->layout[i] == 'z') {
				k->layout[i] = k->layout[20];
				k->layout[20] = 'z';
			} else if (k->layout[i] == 'x') {
				k->layout[i] = k->layout[21];
				k->layout[21] = 'x';
			} else if (k->layout[i] == 'c') {
				k->layout[i] = k->layout[22];
				k->layout[22] = 'c';
			} else if (k->layout[i] == 'v') {
				k->layout[i] = k->layout[23];
				k->layout[23] = 'v';
			}
		}
	}
	
	/* If keepNumbers is enabled, move all numbers back to their positions. */
	if (keepNumbers && ksize != 30) {
		int numstart = full_keyboard == FK_STANDARD ? 1 : 0;
		char c;
		for (c = '0'; c <= '9'; ++c) {
			i = loc(k, c);
			int n = (c - '0' + 9) % 10 + numstart;
			char temp = k->layout[n];
			k->layout[n] = k->layout[i];
			k->layout[i] = temp;
		}
	}

	k->fitness = 0;
	k->distance = 0;
	k->inRoll = 0;
	k->outRoll = 0;
	k->sameHand = 0;
	k->sameFinger = 0;
	k->rowChange = 0;
	k->homeJump = 0;
	k->toCenter = 0;
	k->toOutside = 0;
	
	return 0;
}

int setLayout(Keyboard *k, char *layout)
{
	int i;
	for (i = 0; i < ksize; ++i)
		if (printIt[i]) k->layout[i] = *(layout++);
		else k->layout[i] = '*';

	return 0;
}

int copy(Keyboard *k, Keyboard *original)
{
	int i;
	for (i = 0; i < ksize; ++i) k->layout[i] = original->layout[i];
	k->fitness = original->fitness;
	k->inRoll = original->inRoll;
	k->outRoll = original->outRoll;
	k->sameHand = original->sameHand;
	k->sameFinger = original->sameFinger;
	k->rowChange = original->rowChange;
	k->homeJump = original->homeJump;
	k->toCenter = original->toCenter;
	k->toOutside = original->toOutside;
	
	return 0;
}

int swap(Keyboard *k, int loc1, int loc2)
{
	if (loc1 >= ksize || loc2 >= ksize) return -1;
	if (printIt[loc1] ^ printIt[loc2]) return -2;
	if (isLegalSwap(loc1, loc2) == FALSE) return -3;
	
	int temp = k->layout[loc1];
	k->layout[loc1] = k->layout[loc2];
	k->layout[loc2] = temp;
	return 0;
}

int numberOfSameKeys(Keyboard *k, Keyboard *m)
{
	int i;
	int result;
	for (i = 0, result = 0; i < ksize; i++) if (k->layout[i] == m->layout[i]) ++result;
	return result;
}

int isEqual(Keyboard *k, Keyboard *m)
{
	int i;
	for (i = 0; i < ksize; ++i) if (k->layout[i] != m->layout[i]) return FALSE;
	if (k->fitness != m->fitness) return FALSE;
	if (k->inRoll != m->inRoll) return FALSE;
	if (k->outRoll != m->outRoll) return FALSE;
	if (k->sameHand != m->sameHand) return FALSE;
	if (k->sameFinger != m->sameFinger) return FALSE;
	if (k->rowChange != m->rowChange) return FALSE;
	if (k->homeJump != m->homeJump) return FALSE;
	if (k->toCenter != m->toCenter) return FALSE;
	if (k->toOutside != m->toOutside) return FALSE;
	
	return TRUE;
}

int qwertyPositions(Keyboard *k)
{
	NOT_WORK_WITH_full_keyboard("qwertyPositions()");
	
	int count = 0;
	
	int i;
	for (i = 0; i < ksize; ++i)
		if (k->layout[i] == qwerty[i])
			count++;
	
	return count;
}

int printLayoutOnly(Keyboard *k)
{
	int i;
	
	char str[10];
		
	for (i = 0; i < ksize; ++i) {
		sprintf(str, "%2c", k->layout[i]);
		
		if (full_keyboard == FK_KINESIS) {
			if (printIt[i]) {
				if (i % 11 == 10) printf("%s\n", str);
				else if (i % 11 == 4) printf("%s  ", str);
				else printf("%s ", str);
			} else {
				if (i % 11 == 10) printf(" \n");
				else if (i % 11 == 4) printf("   ");
				else printf("  ");
			}
		} else if (full_keyboard == FK_STANDARD) {
			if (printIt[i] == FALSE) {
				if (i % 14 == 13) printf("  \n");
				else printf("  ");
			} else if (i % 14 == 13) printf("%s\n", str);
			else if (i % 14 == 5) printf("%s  ", str);
			else printf("%s ", str);
		} else {
			if (printIt[i] == FALSE) {
				if (i % 10 == 9) printf("  \n");
				else printf("   ");
			} else if (i % 10 == 9) printf("%s\n", str);
			else if (i % 10 == 4) printf("%s  ", str);
			else printf("%s ", str);
		}
	}
	printf("\n");
	return 0;
}

int printKeyboard(Keyboard *k)
{
	printLayoutOnly(k);

	printf("Fitness: %lld\n", k->fitness);
	if (keepQWERTY) printf("QWERTY positions: %d\n", qwertyPositions(k));
	printf("Distance: %lld\n", k->distance);
	printf("Finger work: %lld\n", k->fingerWork);
	printf("Inward rolls: %lld\n", k->inRoll);
	printf("Outward rolls: %lld\n", k->outRoll);
	printf("Same hand: %lld\n", k->sameHand);
	printf("Same finger: %lld\n", k->sameFinger);
	printf("Row change: %lld\n", k->rowChange);
	printf("Home jump: %lld\n", k->homeJump);
	printf("Ring jump: %lld\n", k->ringJump);
	printf("To center: %lld\n", k->toCenter);
	if (ksize != 30) printf("To outside: %lld\n", k->toOutside);
	printf("\n");
	
	return 0;
}

int printPercentages(Keyboard *k)
{
	int i;
	
	int64_t total = 0;
	for (i = 0; i < 8; ++i) total += k->fingerUsage[i];

	// Hand
	printf("\nHands: ");
	printf("%lld%% ", 100 * (k->fingerUsage[0] + k->fingerUsage[1] + 
			k->fingerUsage[2] + k->fingerUsage[3]) / total);
	printf("%lld%%\n", 100 * (k->fingerUsage[4] + k->fingerUsage[5] + 
			k->fingerUsage[6] + k->fingerUsage[7]) / total);

	if (total == 0) printf("warning: total = 0\n");
	// Finger
	printf("Fingers: ");
	for (i = 0; i < 8; ++i) {
		printf("%lld%% ", 100 * k->fingerUsage[i] / total);
	}
	printf("\n\n");
	
	// Print the keyboard layout.
	printLayoutOnly(k);
		
		
	// Print all the fitness criteria.
	printf("Fitness:       %lld\n",   k->fitness);
	if (keepQWERTY) printf("QWERTY positions: %d\n", qwertyPositions(k));
	printf("Distance:      %lld\n",   (        (    k->distance  )           ));
	printf("Finger work:   %lld\n",   (        (    k->fingerWork)           ));
	printf("Inward rolls:  %.2f%%\n", ((double)(100*k->inRoll    ) / totalDi ));
	printf("Outward rolls: %.2f%%\n", ((double)(100*k->outRoll   ) / totalDi ));
	printf("Same hand:     %.2f%%\n", ((double)(100*k->sameHand  ) / totalDi ));
	printf("Same finger:   %.2f%%\n", ((double)(100*k->sameFinger) / totalDi ));
	printf("Row change:    %.2f%%\n", ((double)(100*k->rowChange ) / totalDi ));
	printf("Home jump:     %.2f%%\n", ((double)(100*k->homeJump  ) / totalDi ));
	printf("Ring jump:     %.2f%%\n", ((double)(100*k->ringJump  ) / totalDi ));
	printf("To center:     %.2f%%\n", ((double)(100*k->toCenter  ) / totalDi ));
	if (ksize != 30) printf("To outside:    %.2f%%\n", ((double)(100*k->toOutside) / totalDi ));
	printf("\n");

	return 0;
}

int simplePrintKeyboard(Keyboard *k)
{
	printLayoutOnly(k);
		
	printf("\n");
	printf("Fitness: %lld\n", (int64_t) (k->fitness / pow(10, SIMPLE_SHIFT)));
	printf("Distance: %lld\n", (int64_t) (k->distance / pow(10, SIMPLE_SHIFT)));
	printf("Inward rolls: %lld\n", (int64_t) (k->inRoll / pow(10, SIMPLE_SHIFT)));
	printf("Outward rolls: %lld\n", (int64_t) (k->outRoll / pow(10, SIMPLE_SHIFT)));
	printf("Same hand: %lld\n", (int64_t) (k->sameHand / pow(10, SIMPLE_SHIFT)));
	printf("Same finger: %lld\n", (int64_t) (k->sameFinger / pow(10, SIMPLE_SHIFT)));
	printf("Row change: %lld\n", (int64_t) (k->rowChange / pow(10, SIMPLE_SHIFT)));
	printf("Home jump: %lld\n", (int64_t) (k->homeJump / pow(10, SIMPLE_SHIFT)));
	printf("To center: %lld\n", (int64_t) (k->toCenter / pow(10, SIMPLE_SHIFT)));
	if (ksize != 30) printf("To outside: %lld\n", (int64_t) (k->toOutside / pow(10, SIMPLE_SHIFT)));
	printf("\n");
	
	return 0;
}

int loc(Keyboard *k, char c)
{
//	char *ptr;
//	if ((ptr = strchr(k->layout, c)) != NULL)
//		return ptr - k->layout;
//	else return -1;
	
	int i;
	for (i = 0; i < ksize; ++i) {
		if (k->layout[i] == c)
			return i;
	}
	
	return -1;
}

