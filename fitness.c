/*
 *  fitness.c
 *  Typing
 *
 *  Created by Michael Dickens on 8/10/09.
 *
 *  Contains functions to calculate the fitness of a keyboard.
 */


#include "keyboard.h"

// For all score calculators, it is assumed that loc0 and loc1 are on the same hand.

// Calculates the stats for k. Useful for human-readable output.
int calcFitnessDirect(Keyboard *k)
{
	calcFitness(k); // Otherwise, k->fitness, k->fingerUsage, and k->fingerWork 
	                // would not get assigned properly.

	int i;
	k->distance		= 0;
	k->inRoll		= 0;
	k->outRoll		= 0;
	k->sameHand		= 0;
	k->sameFinger	= 0;
	k->rowChange	= 0;
	k->homeJump		= 0;
	k->ringJump     = 0;
	k->toCenter		= 0;
	k->toOutside	= 0;

	for (i = 0; i < diLen; ++i) scoreDigraphDirect(k, diKeys[i], diValues[i]);

	for (i = 0; i < monLen; ++i) { 
		k->distance += trueDistance[locWithoutShifted(k, monKeys[i])] * (monValues[i] / 100); // meters 
	}
	
	return 0;
}

int scoreDigraphDirect(Keyboard *k, char digraph[], int multiplier)
{
	int locs[2];
	int i;
	for (i = 0; i < 2; ++i) locs[i] = locWithoutShifted(k, digraph[i]);
	
	// These all require that the hand be the same.
	if (hand[locs[0]] == hand[locs[1]]) {
		if (calcInRoll    (locs[0], locs[1]) != 0) k->inRoll     += multiplier;
		if (calcOutRoll   (locs[0], locs[1]) != 0) k->outRoll    += multiplier;
												   k->sameHand   += multiplier;
		if (calcSameFinger(locs[0], locs[1]) != 0) k->sameFinger += multiplier;
		if (calcRowChange (locs[0], locs[1]) != 0) k->rowChange  += multiplier;
		if (calcHomeJump  (locs[0], locs[1]) != 0) k->homeJump   += multiplier;
		if (calcRingJump  (locs[0], locs[1]) != 0) k->ringJump   += multiplier;
		if (calcToCenter  (locs[0], locs[1]) != 0) k->toCenter   += multiplier;
		if (calcToOutside (locs[0], locs[1]) != 0) k->toOutside	 += multiplier;
	}
	
	return 0;
}

int preCalculateFitness()
{
	NOT_WORK_WITH_full_keyboard("preCalculateFitness")
	int i, j;
	
	int costs[900];
	for (i = 0; i < 900; ++i) costs[i] = 0;
	
	for (i = 0; i < 30; ++i)
		for (j = 0; j < 30; ++j) {
			costs[30*i + j] += (distanceCosts[i] + distanceCosts[j]) * monValues[i] * distance;
			if (hand[i] == hand[j]) {
				costs[30*i + j]	+= calcInRoll    (i, j);	
				costs[30*i + j] += calcOutRoll   (i, j);	
				costs[30*i + j] += sameHand		   ;
				costs[30*i + j] += calcSameFinger(i, j);
				costs[30*i + j] += calcRowChange (i, j);
				costs[30*i + j] += calcHomeJump  (i, j);
				costs[30*i + j] += calcRingJump  (i, j);
				costs[30*i + j] += calcToCenter  (i, j);
				costs[30*i + j] += calcToOutside (i, j);
			}
		}
		
	for (i = 0; i < 900; ++i)
		printf("costs[%d] = %d;\n", i, costs[i]);
		
	return 0;
}

int calcFitness(Keyboard *k)
{
	int i;
	
	for (i = 0; i < FINGER_COUNT; ++i) k->fingerUsage[i] = 0;
	k->fitness		= 0;
	k->distance		= 0;
	k->inRoll		= 0;
	k->outRoll		= 0;
	k->sameHand		= 0;
	k->sameFinger	= 0;
	k->rowChange	= 0;
	k->homeJump		= 0;
	k->ringJump     = 0;
	k->toCenter		= 0;
	k->toOutside	= 0;
	
	int locs[128];
	for (i = 0; i < 128; ++i) locs[i] = -1;
	
	/* Calculate all the locations beforehand. This saves a lot of time. */
	for (i = 0; i < ksize; ++i)
		if (printable[i]) {
			locs[k->layout[i]] = i;
			locs[k->shiftedLayout[i]] = ksize + i;
		}
	
	for (i = diLen - 1; i >= 0; --i) {
		scoreDigraph(k, diKeys[i], diValues[i], locs);
	}
	
	/* Calculate distance. Done here and not in scoreDigraph because it uses 
	 * monographs instead of digraphs.
	 */
	for (i = 0; i < monLen; ++i) {
		int lc = locs[monKeys[i]] % ksize;
		if (lc != -1) k->distance += distanceCosts[lc] * monValues[i] * distance;
		
		if (hand[lc] == LEFT) k->fingerUsage[finger[lc]] += monValues[i];
		else k->fingerUsage[FINGER_COUNT - 1 - finger[lc]] += monValues[i];
	}
	calcFingerWork(k);	
	
	k->fitness = k->distance + k->fingerWork + k->inRoll + k->outRoll + 
		k->sameHand + k->sameFinger + k->rowChange + k->homeJump + k->ringJump + 
		k->toCenter + k->toOutside;
	if (keepZXCV) k->fitness += calcShortcuts(k);
	if (keepQWERTY) k->fitness += calcQWERTY(k);
	if (keepParentheses) k->fitness += calcBrackets(k);
	if (keepNumbersShifted) k->fitness += calcNumbersShifted(k);

	++totalCalcFitness;
	return 0;
}

int scoreDigraph(Keyboard *k, char digraph[], int multiplier, int allLocs[])
{
	int loc0 = allLocs[digraph[0]] % ksize;
	int loc1 = allLocs[digraph[1]] % ksize;
	
	if (loc0 == -1 || loc1 == -1) {
		return 1;
	}
	
	/* TODO: Once modifier keys are added, change this to include the cost of 
	 * hitting Shift, plus an extra penalty.
	 */
	if (allLocs[digraph[0]] >= ksize && allLocs[digraph[1]] >= ksize) {
		k->distance += doubleShiftCost * multiplier;
	} else if (allLocs[digraph[0]] >= ksize ^ allLocs[digraph[1]] >= ksize) {
		k->distance += shiftCost * multiplier;
	}
	
	/* These all require that the hand be the same. */
	if (hand[loc0] == hand[loc1]) {
		k->sameHand		+= sameHand					  * multiplier;
		k->sameFinger	+= calcSameFinger(loc0, loc1) * multiplier;
		
		/* These factors are meaningless for the thumbs, because thumbs are 
		 * relatively separate from the rest of the fingers.
		 */
		if (finger[loc0] != THUMB && finger[loc1] != THUMB) {
			k->inRoll		+= calcInRoll    (loc0, loc1) * multiplier;	
			k->outRoll		+= calcOutRoll   (loc0, loc1) * multiplier;	
			k->rowChange	+= calcRowChange (loc0, loc1) * multiplier;
			k->homeJump		+= calcHomeJump  (loc0, loc1) * multiplier;
			k->ringJump     += calcRingJump  (loc0, loc1) * multiplier;
			k->toCenter		+= calcToCenter  (loc0, loc1) * multiplier;
			k->toOutside	+= calcToOutside (loc0, loc1) * multiplier;		
		}
	}
	
	return 0;
}

int64_t calcShortcuts(Keyboard *k)
{
	int64_t result;
	result = 
	      shortcutCosts[locWithoutShifted(k, 'z')] * (int64_t) zCost
		+ shortcutCosts[locWithoutShifted(k, 'x')] * (int64_t) xCost
		+ shortcutCosts[locWithoutShifted(k, 'c')] * (int64_t) cCost
		+ shortcutCosts[locWithoutShifted(k, 'v')] * (int64_t) vCost;
		
	return result * (totalMon / monLen);
}

int64_t calcQWERTY(Keyboard *k)
{
	NOT_WORK_WITH_full_keyboard("calcQWERTY")
	int64_t result = 0;
	int64_t averageMon = totalMon / 30;

	int i, pos;
	for (i = 0; i < 30; ++i) {
		if ((pos = locWithoutShifted(k, qwerty[i])) != i) result += qwertyPosCost * averageMon;
		if (finger[pos] != finger[i]) result += qwertyFingerCost * averageMon;
		if (hand[pos] != hand[i]) result += qwertyHandCost * averageMon;
	}
	
	return result;
}

int64_t calcBrackets(Keyboard *k)
{
	return calcBracketsGeneric(k, '(', ')') + calcBracketsGeneric(k, '<', '>') + 
			calcBracketsGeneric(k, '[', ']') + calcBracketsGeneric(k, '{', '}');
}

int64_t calcBracketsGeneric(Keyboard *k, char openChar, char closeChar)
{
	int openPar = locWithShifted(k, openChar);
	if (openPar == -1) return -1;
	int closePar = locWithShifted(k, closeChar);
	if (closePar == -1) return -1;
	
	int openShifted = openPar >= ksize;
	int closeShifted = closePar >= ksize;
	
	openPar %= ksize;
	closePar %= ksize;
	
	/* Parenthses have to be either next to each other or on opposite side of 
	 * the keyboard, and must both be either shifted or unshifted.
	 */
	if (openShifted == closeShifted && 
			((openPar+1 == closePar && row[openPar] == row[closePar]) ||
			(hand[openPar] == LEFT && hand[closePar] == RIGHT && 
			column[openPar] == column[closePar] && 
			row[openPar] == row[closePar])))
		return 0;
	else return bracketsCost / DIVISOR;
}

int64_t calcNumbersShifted(Keyboard *k)
{
	int64_t score = 0;
	
	char c;
	for (c = '0'; c <= '9'; ++c)
		if (strchr(k->shiftedLayout, c))
			score += numbersShiftedCost;
	
	return score;
}

/* Requires that k->fingerUsage has been calculated. */
int calcFingerWork(Keyboard *k)
{
	int64_t total = 0;
	int i;
	for (i = 0; i < FINGER_COUNT; ++i) total += k->fingerUsage[i];

	k->fingerWork = 0;
	
	for (i = 0; i < FINGER_COUNT; ++i) {
		if (1000*k->fingerUsage[i]/total > fingerPercentMaxes[i]*10) {
			k->fingerWork += (k->fingerUsage[i] - 
					(fingerPercentMaxes[i]*total/100))
					* fingerWorkCosts[i];
		}
	}
		
	return 0;
}

/* 
 * In and out rolls could be defined in two ways: either two keys 
 * pressed next to each other, or two keys moving in one direction.
 * The former definition is currently used.
 * 
 * Rolls must occur on the same row.
 */
int calcInRoll(int loc0, int loc1)
{
	if (finger[loc1] == finger[loc0] + 1 && row[loc0] == row[loc1] && 
			!isCenterOrOutside[loc0] && !isCenterOrOutside[loc1])
		return inRoll;
	else return 0;
//	if (finger[loc1] > finger[loc0] && row[loc0] == row[loc1] && !isCenterOrOutside[loc0] && !isCenterOrOutside[loc1]) return inRoll;
//	else return 0;
}

int calcOutRoll(int loc0, int loc1)
{
	if (finger[loc1] == finger[loc0] - 1 && row[loc0] == row[loc1] && 
			!isCenterOrOutside[loc0] && !isCenterOrOutside[loc1])
		return outRoll;
	else return 0;
//	if (finger[loc1] < finger[loc0] && row[loc0] == row[loc1] && !isCenterOrOutside[loc0] && !isCenterOrOutside[loc1]) return outRoll;
//	else return 0;
}

/* Adds to the score if the same hand is used twice in a row.
 */
int calcSameHand(int loc0, int loc1)
{	
	return hand[loc0] == hand[loc1] ? sameHand : 0;
}

int calcSameFinger(int loc0, int loc1)
{
	if (finger[loc0] == finger[loc1]) {
		if (loc0 == loc1) return 0;
		else switch (finger[loc0]) {
			case PINKY:
				return sameFingerP;
			case RING:
				return sameFingerR;
			case MIDDLE:
				return sameFingerM;
			case INDEX:
				return sameFingerI;
			case THUMB:
				return sameFingerT;
		}
	}
	
	return 0;
}

// If the row changes on the same hand, there is a penalty.
// Jumping over the home row is an additional penalty, but 
// isn't calculated here (see calcHomeJump()).
int calcRowChange(int loc0, int loc1)
{
	if (row[loc0] != row[loc1]) {
		if (row[loc0] < row[loc1]) { // loc1 is a lower row
			return rowChangeTableDown[finger[loc0]][finger[loc1]];
		} else { // loc1 is a higher row
			return rowChangeTableUp[finger[loc0]][finger[loc1]];
		}
	}
	return 0;
}

int calcHomeJump(int loc0, int loc1)
{
	int row0 = row[loc0];
	int row1 = row[loc1];
	
	if (row0 < row1) {
		if (homeRow <= row0 || homeRow >= row1) return 0;
	} else if (row0 > row1) {
		if (homeRow <= row1 || homeRow >= row0) return 0;
	} else return 0;
	
	if (abs(row0 - row1) == 2) {
		if ((row0 > row1 && finger[loc0] == INDEX && (finger[loc1] == MIDDLE || finger[loc1] == RING)) || 
			(row1 > row0 && finger[loc1] == INDEX && (finger[loc0] == MIDDLE || finger[loc0] == RING))) return homeJump + homeJumpIndex;
		else return homeJump;
	} else if (abs(row0 - row1) > 2) {
		if ((row0 > row1 && finger[loc0] == INDEX && (finger[loc1] == MIDDLE || finger[loc1] == RING)) || 
			(row1 > row0 && finger[loc1] == INDEX && (finger[loc0] == MIDDLE || finger[loc0] == RING))) return doubleJump + homeJumpIndex;
		else return doubleJump;
	}

	return 0;
}

int calcRingJump(int loc0, int loc1)
{
	if ((finger[loc0] == PINKY && finger[loc1] == MIDDLE) || 
			(finger[loc0] == MIDDLE && finger[loc1] == PINKY)) return ringJump;
	else return 0;
	
}

int calcToCenter(int loc0, int loc1)
{
	if (isCenter[loc0] ^ isCenter[loc1]) return toCenter;
	else return 0;
}

/* Only applies for the extended keyboard.
 */
int calcToOutside(int loc0, int loc1)
{
	if (fullKeyboard != FK_NO && (isOutside[loc0] ^ isOutside[loc1])) return toOutside;
	else return 0;
}