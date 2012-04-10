/*
 *  values.c
 *  Typing
 *
 *  Created by Michael Dickens on 8/23/09.
 *
 */

#include "values.h"

int initValues()
{
	int i;
	
	initCosts();
	
	if (fullKeyboard == FK_NO) {
		// Set keyboard position costs. These costs were determined by looking 
		// at how the positions were valued on some of the best alternative 
		// layouts.
		int64_t costsCopy[KSIZE_MAX] = {
			 40,  40,  30,  40,  70,  80,  40,  30,  40,  40, 
			  0,   0,   0,   0,  30,  30,   0,   0,   0,   0, 
			 70,  70,  70,  50,  95,  60,  40,  60,  70,  70, 
		};
	
		for (i = 0; i < ksize; ++i)
			distanceCosts[i] = costsCopy[i];
		
	} else if (fullKeyboard == FK_STANDARD) {
		
		// These costs are optimized for a full standard layout. Any cost that 
		// is 999 is not supposed to contain any character.
		int64_t costsCopy[KSIZE_MAX] = {
			110, 100,  90,  75, 100, 120, 160, 100,  75,  90, 100, 110, 120, 999,
			999,  40,  40,  30,  40,  70,  80,  40,  30,  40,  40,  60,  90, 140, 
			999,   0,   0,   0,   0,  30,  30,   0,   0,   0,   0,  50, 999, 999,
			999,  70,  70,  70,  50,  95,  60,  40,  60,  70,  70, 999, 999, 999,
		};
		for (i = 0; i < ksize; ++i)
			distanceCosts[i] = costsCopy[i];

	} else if (fullKeyboard == FK_KINESIS) {
		
		// These costs are optimized for Kinesis. Any cost that is 999 is not 
		// supposed to contain any character.
		int64_t costsCopy[KSIZE_MAX] = {
			120, 110, 100,  75, 100, 130, 130, 100,  75, 100, 110, 120, 
			 90,  40,  40,  30,  40,  70,  70,  40,  30,  40,  40,  90, 
			 60,   0,   0,   0,   0,  40,  40,   0,   0,   0,   0,  60, 
			999,  70,  70,  70,  50,  80,  80,  50,  70,  70,  70, 999, 
			999, 140, 140, 999, 999, 999, 999, 999, 999, 140, 140, 999, 	
			  0,  50, 999, 999, 999, 999, 999, 999, 999, 999,  50,   0, 
		};
		for (i = 0; i < ksize; ++i)
			distanceCosts[i] = costsCopy[i];
		
	} else if (fullKeyboard == FK_IPHONE) {
		
		// Thumbs are centered over the QWERTY D and K keys.
		int64_t costsCopy[KSIZE_MAX] = {
			  45,  30,  20,  10,  20,  20,  10,  20,  30,  40, 
			  20,  10,   0,   0,  10,  10,   0,   0,  10, 999,
			 999,  60,  50,  30,  20,  20,  50,  60, 999, 999, 
		};
		for (i = 0; i < ksize; ++i)
			distanceCosts[i] = costsCopy[i];
		
	}

	// Distance in mm.
	trueDistance[ 0] = 19; trueDistance[ 1] = 19; trueDistance[ 2] = 19; trueDistance[ 3] = 19; trueDistance[ 4] = 28; 
	trueDistance[ 5] = 28; trueDistance[ 6] = 19; trueDistance[ 7] = 19; trueDistance[ 8] = 19; trueDistance[ 9] = 19; 
	trueDistance[10] =  0; trueDistance[11] =  0; trueDistance[12] =  0; trueDistance[13] =  0; trueDistance[14] = 19; 
	trueDistance[15] = 19; trueDistance[16] =  0; trueDistance[17] =  0; trueDistance[18] =  0; trueDistance[19] =  0; 
	trueDistance[20] = 19; trueDistance[21] = 19; trueDistance[22] = 19; trueDistance[23] = 19; trueDistance[24] = 28; 
	trueDistance[25] = 28; trueDistance[26] = 19; trueDistance[27] = 19; trueDistance[28] = 19; trueDistance[29] = 19; 

	// Based on distance from the ctrl key and how much of a stretch it is.
	shortcutCosts[ 0] =  0; shortcutCosts[ 1] =  0; shortcutCosts[ 2] =  1; shortcutCosts[ 3] =  3; shortcutCosts[ 4] =  4; 
	shortcutCosts[ 5] =  8; shortcutCosts[ 6] = 10; shortcutCosts[ 7] = 10; shortcutCosts[ 8] = 10; shortcutCosts[ 9] = 10; 
	shortcutCosts[10] =  0; shortcutCosts[11] =  0; shortcutCosts[12] =  2; shortcutCosts[13] =  3; shortcutCosts[14] =  5; 
	shortcutCosts[15] =  8; shortcutCosts[16] = 10; shortcutCosts[17] = 10; shortcutCosts[18] = 10; shortcutCosts[19] = 10; 
	shortcutCosts[20] =  0; shortcutCosts[21] =  0; shortcutCosts[22] =  0; shortcutCosts[23] =  2; shortcutCosts[24] =  4; 
	shortcutCosts[25] =  8; shortcutCosts[26] = 10; shortcutCosts[27] = 10; shortcutCosts[28] = 10; shortcutCosts[29] = 10; 
	
	return 0;
}

void initCosts()
{
	detailedOutput = TRUE;
	
	keepZXCV = FALSE;
	keepQWERTY = FALSE;
	keepNumbers = 1;
	keepParentheses = TRUE;
	keepShiftPairs = FALSE;
	keepTab = FALSE;
	keepConsonantsRight = FALSE;
	
	if (fullKeyboard == FK_KINESIS) {
		fingerPercentMaxes[0] = fingerPercentMaxes[FINGER_COUNT - 1] =  7.5;
		fingerPercentMaxes[1] = fingerPercentMaxes[FINGER_COUNT - 2] = 10.0;
		fingerPercentMaxes[2] = fingerPercentMaxes[FINGER_COUNT - 3] = 20.0;
		fingerPercentMaxes[3] = fingerPercentMaxes[FINGER_COUNT - 4] = 20.0;
		fingerPercentMaxes[4] = fingerPercentMaxes[FINGER_COUNT - 5] = 18.0;
	} else {
		fingerPercentMaxes[0] = fingerPercentMaxes[FINGER_COUNT - 1] =  9.0;
		fingerPercentMaxes[1] = fingerPercentMaxes[FINGER_COUNT - 2] = 11.5;
		fingerPercentMaxes[2] = fingerPercentMaxes[FINGER_COUNT - 3] = 22.0;
		fingerPercentMaxes[3] = fingerPercentMaxes[FINGER_COUNT - 4] = 22.0;
		fingerPercentMaxes[4] = fingerPercentMaxes[FINGER_COUNT - 5] = 18.0;		
	}

	
	fingerWorkCosts[0] = fingerWorkCosts[FINGER_COUNT - 1] = 40;
	fingerWorkCosts[1] = fingerWorkCosts[FINGER_COUNT - 2] = 30;
	fingerWorkCosts[2] = fingerWorkCosts[FINGER_COUNT - 3] = 20;
	fingerWorkCosts[3] = fingerWorkCosts[FINGER_COUNT - 4] = 20;
	fingerWorkCosts[4] = fingerWorkCosts[FINGER_COUNT - 5] = 20;
	
	// All values are compounding. For example, say we jump over the home row 
	// on the index finger. The cost is sameHand + rowChange + homeJump + homeJumpIndex.
	distance =		  1;
	inRoll =		-40;
	outRoll =		  5;
	sameHand =		  5;
	sameFingerP =	150;
	sameFingerR =	140;
	sameFingerM =	110;
	sameFingerI =	 90;
	sameFingerT =	100;
	rowChangeDown =  10;
	rowChangeUp =    15;
	handWarp =		 25;
	handSmooth =	- 5;
	homeJump =		100;
	homeJumpIndex = -90;
	doubleJump =	220; // Does not compound with homeJump.
	ringJump =       40;
	toCenter =		 30;
	toOutside =		 30;
	
	shiftCost =		100;
	doubleShiftCost=150;
}
