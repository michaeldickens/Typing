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

	if (full_keyboard == FK_NO) {
		// This default keyboard is QWERTY.
		setLayout(k, "qwertyuiopasdfghjkl;zxcvbnm,.'");
	} else if (full_keyboard == FK_STANDARD) {
		// This default keyboard is QWERTY with the punctuation rearranged so as to use the 
		// most common punctuation marks: , . ) ( _ " ; - ' = /
		setLayout(k, "_1234567890-=qwertyuiop()\"asdfghjkl;'zxcvbnm,./");
	} else {
		// This default keyboard is QWERTY with the punctuation rearranged so as to use the 
		// most common punctuation marks: , . ) ( _ " ; - ' = /
		setLayout(k, "1234567890-qwertyuiop_asdfghjkl;\"zxcvbnm,./'=()");		
	}
	
	k->layout[ksize] = '\0';
		
	for (i = 0; i < 8; ++i) k->fingerUsage[i] = 0;
	
	shuffleLayout(k->layout);
	
	/* If KEEP_ZXCV is enabled, move ZXCV back to their positions. */
	if (KEEP_ZXCV) {
		NOT_WORK_WITH_full_keyboard("KEEP_ZXCV")
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
	
	/* If KEEP_NUMBERS is enabled, move all numbers back to their positions. */
	if (KEEP_NUMBERS) {
		NOT_WORK_WITH_30_KEYBOARD("KEEP_NUMBERS")
		int numstart = full_keyboard == FK_STANDARD ? 1 : 0;
		for (i = 0; i < ksize; ++i) {
			if (k->layout[i] >= '0' && k->layout[i] <= '9') {
				char temp = k->layout[i];
				k->layout[i] = k->layout[(temp - '0' + 9) % 10 + numstart];
				k->layout[(temp - '0' + 9) % 10 + numstart] = temp;
			}
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

int printLayoutOnly(Keyboard *k)
{
	int i;
	for (i = 0; i < ksize; ++i)
		if (full_keyboard == FK_NO) {
			if (printIt[i] == FALSE) printf("  ");
			else if (i % 10 == 9) printf("%c\n", k->layout[i]);
			else if (i % 10 == 4) printf("%c  ", k->layout[i]);
			else printf("%c ", k->layout[i]);
		} else if (full_keyboard == FK_STANDARD) {
			if (printIt[i] == FALSE) {
				if (i % 14 == 13) printf("  \n");
				else printf("  ");
			} else if (i % 14 == 13) printf("%c\n", k->layout[i]);
			else if (i % 14 == 5) printf("%c  ", k->layout[i]);
			else printf("%c ", k->layout[i]);
		} else {
			if (printIt[i]) {
				if (i % 11 == 10) printf("%c\n", k->layout[i]);
				else if (i % 11 == 4) printf("%c  ", k->layout[i]);
				else printf("%c ", k->layout[i]);
			} else {
				if (i % 11 == 10) printf(" \n");
				else if (i % 11 == 4) printf("   ");
				else printf("  ");
			}

		}
	printf("\n");
	return 0;
}

int printKeyboard(Keyboard *k)
{
	printLayoutOnly(k);

	printf("Fitness: %lld\n", k->fitness);
	printf("Distance: %lld\n", k->distance);
	printf("Inward rolls: %lld\n", k->inRoll);
	printf("Outward rolls: %lld\n", k->outRoll);
	printf("Same hand: %lld\n", k->sameHand);
	printf("Same finger: %lld\n", k->sameFinger);
	printf("Row change: %lld\n", k->rowChange);
	printf("Home jump: %lld\n", k->homeJump);
	printf("To center: %lld\n", k->toCenter);
	if (ksize != 30) printf("To outside: %lld\n", k->toOutside);
	printf("\n");
	
	return 0;
}

int printPercentages(Keyboard *k)
{
	int i;

	// Figure out how much each finger is being used.
	long long total = 0;
	long long fingerUsage[8];
	for (i = 0; i < 8; ++i) fingerUsage[i] = 0;
	for (i = 0; i < monLen; ++i) {
		int lc = loc(k, monKeys[i]);
		if (hand[lc] == LEFT) {
			if (lc == -1) {
				printf("character %c not found\n", monKeys[i]);
			}
			fingerUsage[finger[lc]] += monValues[i];
		} else fingerUsage[7 - finger[lc]] += monValues[i];
	}
	for (i = 0; i < 8; ++i) total += fingerUsage[i];

	// Hand
	printf("\nHands: ");
	printf("%lld%% ", 100 * (fingerUsage[0] + fingerUsage[1] + fingerUsage[2] + fingerUsage[3]) / total);
	printf("%lld%%\n", 100 * (fingerUsage[4] + fingerUsage[5] + fingerUsage[6] + fingerUsage[7]) / total);

	if (total == 0) printf("warning: total = 0\n");
	// Finger
	printf("Fingers: ");
	for (i = 0; i < 8; ++i) {
		printf("%lld%% ", 100 * fingerUsage[i] / total);
	}
	printf("\n\n");
	
	// Print the keyboard layout.
	printLayoutOnly(k);
		
		
	// Print all the fitness criteria.
	printf("Fitness:       %lld\n",   k->fitness);
//	printf("Fitness:       %.2f\n",   ((double)(    k->fitness   ) / totalMon));
	printf("Distance:      %lld\n",   (        (    k->distance  )            ));
//	printf("Distance:      %.2f\n",   ((double)(100*k->distance  ) / totalMon));
	printf("Inward rolls:  %.2f%%\n", ((double)(100*k->inRoll    ) / totalDi ));
	printf("Outward rolls: %.2f%%\n", ((double)(100*k->outRoll   ) / totalDi ));
	printf("Same hand:     %.2f%%\n", ((double)(100*k->sameHand  ) / totalDi ));
	printf("Same finger:   %.2f%%\n", ((double)(100*k->sameFinger) / totalDi ));
	printf("Row change:    %.2f%%\n", ((double)(100*k->rowChange ) / totalDi ));
	printf("Home jump:     %.2f%%\n", ((double)(100*k->homeJump  ) / totalDi ));
	printf("To center:     %.2f%%\n", ((double)(100*k->toCenter  ) / totalDi ));
	if (ksize != 30) printf("To outside:    %.2f%%\n", ((double)(100*k->toOutside) / totalDi ));
	printf("\n");

	return 0;
}

int simplePrintKeyboard(Keyboard *k)
{
	printLayoutOnly(k);
		
	printf("\n");
	printf("Fitness: %lld\n", (long long) (k->fitness / pow(10, SIMPLE_SHIFT)));
	printf("Distance: %lld\n", (long long) (k->distance / pow(10, SIMPLE_SHIFT)));
	printf("Inward rolls: %lld\n", (long long) (k->inRoll / pow(10, SIMPLE_SHIFT)));
	printf("Outward rolls: %lld\n", (long long) (k->outRoll / pow(10, SIMPLE_SHIFT)));
	printf("Same hand: %lld\n", (long long) (k->sameHand / pow(10, SIMPLE_SHIFT)));
	printf("Same finger: %lld\n", (long long) (k->sameFinger / pow(10, SIMPLE_SHIFT)));
	printf("Row change: %lld\n", (long long) (k->rowChange / pow(10, SIMPLE_SHIFT)));
	printf("Home jump: %lld\n", (long long) (k->homeJump / pow(10, SIMPLE_SHIFT)));
	printf("To center: %lld\n", (long long) (k->toCenter / pow(10, SIMPLE_SHIFT)));
	if (ksize != 30) printf("To outside: %lld\n", (long long) (k->toOutside / pow(10, SIMPLE_SHIFT)));
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

