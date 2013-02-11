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
// The function returns 0 if completion occurs as expected and
// non-0 otherwise.
int calcFitnessDirect(Keyboard *const k)
{
	if( digraphs == NULL ) {
		internalError(022);
		return 1;
	}
	if( digraphs->kvt_table == NULL ) {
		internalError(027);
		return 1;
	}
	calcFitness(k); // Otherwise, k->fitness, k->fingerUsage, and k->fingerWork 
	                // would not get assigned properly.

	uint64_t i;
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

	extern void scoreAllDigraphsDirectly(Keyboard *const k);

	scoreAllDigraphsDirectly(k);

	char key;
	Value value;
	if( monographs == NULL ) {
		internalError(032);
		exit(0);
	}
	const uint64_t used = monographs->kvt_used;

	for (i = 0; i < used; ++i) { 
		key = getFirstKeystroke(monographs, i);
		value = getKVValue(monographs, i);
		k->distance += trueDistance[loc(k, key)] * (value / 100); // meters 
	}
	
	return 0;
}

// Scores all digraphs for the Keyboard object pointed to by
// parameter 'k' using the location information in parameter
// 'locs'.
void scoreAllDigraphs(Keyboard *k, int64_t locs[128]) {
	if( digraphs == NULL ) {
		internalError(033);
		exit(0);
	}
	const int64_t used = digraphs->kvt_used;
	const KeystrokeValue *theTable = digraphs->kvt_table;
	KeystrokeValue kv;
	Keystroke theStroke;
	Value value;
	int64_t i;

	for (i = used - 1; i >= 0; --i) {
		kv = theTable[i];
		theStroke = kv.theStroke;
		value = kv.theValue;
		scoreDigraph(k, theStroke, value, locs);
	}
}

// Directly scores all digraphs for the Keyboard object pointed to by
// parameter 'k'.
void scoreAllDigraphsDirectly(Keyboard *const k)
{
	if( digraphs == NULL ) {
		internalError(034);
		exit(0);
	}
	const int64_t used = digraphs->kvt_used;
	const KeystrokeValue *theTable = digraphs->kvt_table;
	KeystrokeValue kv;
	Keystroke theStroke;
	Value value;
	int64_t i;

	for (i = 0; i < used; ++i) {
		kv = theTable[i];
		theStroke = kv.theStroke;
		value = kv.theValue;
		scoreDigraphDirect(k, theStroke, value);
	}
}

int scoreDigraphDirect(Keyboard *const k, const char digraph[], const Value multiplier)
{
	int locs[2];
	int i;
	for (i = 0; i < 2; ++i) locs[i] = loc(k, digraph[i]);
	
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

int calcFitness(Keyboard *const k)
{
	int64_t i;
	
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
	
	int64_t locs[128];
	for (i = 0; i < 128; ++i) locs[i] = -1;
	
	/* Calculate all the locations beforehand. This saves a lot of time. */
	for (i = 0; i < ksize; ++i)
		if (printable[i]) {
			locs[k->layout[i]] = i;
			locs[k->shiftedLayout[i]] = ksize + i;
		}
	
	scoreAllDigraphs(k, locs);

	char key;
	Value value;
	if( monographs == NULL ) {
		internalError(035);
		exit(0);
	}
	const int64_t used = monographs->kvt_used;

	/* Calculate distance. Done here and not in scoreDigraph because it uses 
	 * monographs instead of digraphs.
	 */
	for (i = 0; i < used; ++i) {
		key = getFirstKeystroke(monographs, i);
		value = getKVValue(monographs, i);
		const int lc = locs[key] % ksize;
		if (lc >= 0) k->distance += distanceCosts[lc] * value * distance;
		else {
			internalError( 9 );
			return 0;
		}
		
		if (hand[lc] == LEFT) k->fingerUsage[finger[lc]] += value;
		else k->fingerUsage[FINGER_COUNT - 1 - finger[lc]] += value;
	}
	calcFingerWork(k);	
	
	k->fitness = k->distance + k->fingerWork + k->inRoll + k->outRoll + 
		k->sameHand + k->sameFinger + k->rowChange + k->homeJump + k->ringJump + 
		k->toCenter + k->toOutside;
	if (keepZXCV) k->fitness += calcShortcuts(k);
	if (keepQWERTY) k->fitness += calcQWERTY(k);
	if (keepParentheses) k->fitness += calcParentheses(k);
	if (keepNumbersShifted) k->fitness += calcNumbersShifted(k);

	return 0;
}

int scoreDigraph(Keyboard *const k, const char digraph[], const int64_t multiplier, const int64_t allLocs[])
{
	const int loc0 = allLocs[digraph[0]] % ksize;
	const int loc1 = allLocs[digraph[1]] % ksize;
	
	if (loc0 == -1 || loc1 == -1) {
		return 1;
	}
	
	/* TODO: Once modifier keys are added, change this to include the cost of 
	 * hitting Shift, plus an extra penalty.
	 */
	if (allLocs[digraph[0]] >= ksize && allLocs[digraph[1]] >= ksize) {
		k->distance += doubleShiftCost * multiplier;
	} else if ((allLocs[digraph[0]] >= ksize) != (allLocs[digraph[1]] >= ksize)) {
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

int64_t calcShortcuts(const Keyboard *const k)
{
	if( monographs == NULL ) {
		internalError(036);
		exit(0);
	}
	int64_t result;
	result = 
	      shortcutCosts[loc(k, 'z')] * (int64_t) zCost
		+ shortcutCosts[loc(k, 'x')] * (int64_t) xCost
		+ shortcutCosts[loc(k, 'c')] * (int64_t) cCost
		+ shortcutCosts[loc(k, 'v')] * (int64_t) vCost;
	const int64_t used = monographs->kvt_used;
	return result * (totalMon / used);
}

int64_t calcQWERTY(const Keyboard *const k)
{
	NOT_WORK_WITH_full_keyboard("calcQWERTY")
	int64_t result = 0;
	const int64_t averageMon = totalMon / 30;

	int i, pos;
	for (i = 0; i < 30; ++i) {
		if ((pos = loc(k, qwerty[i])) != i) result += qwertyPosCost * averageMon;
		if (finger[pos] != finger[i]) result += qwertyFingerCost * averageMon;
		if (hand[pos] != hand[i]) result += qwertyHandCost * averageMon;
	}
	
	return result;
}

int64_t calcParentheses(const Keyboard *const k)
{
	return calcParensGeneric(k, '(', ')') + calcParensGeneric(k, '<', '>') + 
			calcParensGeneric(k, '[', ']') + calcParensGeneric(k, '{', '}');
}

int64_t calcParensGeneric(const Keyboard *const k, const char openChar, const char closeChar)
{
	int openPar = locWithShifted(k, openChar);
	if (openPar == -1) return -1;
	int closePar = locWithShifted(k, closeChar);
	if (closePar == -1) return -1;
	
	const int openShifted = openPar >= ksize;
	const int closeShifted = closePar >= ksize;
	
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
	else return parenthesesCost / DIVISOR;
}

int64_t calcNumbersShifted(const Keyboard *const k)
{
	int64_t score = 0;
	
	char c;
	for (c = '0'; c <= '9'; ++c)
		if (strchr(k->shiftedLayout, c))
			score += numbersShiftedCost;
	
	return score;
}

/* Requires that k->fingerUsage has been calculated. */
int calcFingerWork(Keyboard *const k)
{
	int64_t total = 0;
	int i;
	double usage;
	double percentMax;
	double difference;
	for (i = 0; i < FINGER_COUNT; ++i) total += k->fingerUsage[i];

	k->fingerWork = 0;
	
	for (i = 0; i < FINGER_COUNT; ++i) {
		usage = k->fingerUsage[i];
		percentMax = fingerPercentMaxes[i];
		if (1000*usage/total > percentMax*10) {
			difference = usage - (percentMax*total/100);
			k->fingerWork += (int64_t) difference * fingerWorkCosts[i];
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
int calcInRoll(const int loc0, const int loc1)
{
	if (finger[loc1] == finger[loc0] + 1 && row[loc0] == row[loc1] && 
			!isCenterOrOutside[loc0] && !isCenterOrOutside[loc1])
		return inRoll;
	else return 0;
//	if (finger[loc1] > finger[loc0] && row[loc0] == row[loc1] && !isCenterOrOutside[loc0] && !isCenterOrOutside[loc1]) return inRoll;
//	else return 0;
}

int calcOutRoll(const int loc0, const int loc1)
{
	if (finger[loc1] == finger[loc0] - 1 && row[loc0] == row[loc1] && 
			!isCenterOrOutside[loc0] && !isCenterOrOutside[loc1])
		return outRoll;
	else return 0;
//	if (finger[loc1] < finger[loc0] && row[loc0] == row[loc1] && !isCenterOrOutside[loc0] && !isCenterOrOutside[loc1]) return outRoll;
//	else return 0;
}

int calcSameFinger(const int loc0, const int loc1)
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
int calcRowChange(const int loc0, const int loc1)
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

int calcHomeJump(const int loc0, const int loc1)
{
	const int row0 = row[loc0];
	const int row1 = row[loc1];
	
	if (row0 < row1) {
		if (homeRow <= row0 || homeRow >= row1) return 0;
	} else if (row0 > row1) {
		if (homeRow <= row1 || homeRow >= row0) return 0;
	} else return 0;
	
	if (abs(row0 - row1) == 2)
		if ((row0 > row1 && finger[loc0] == INDEX && (finger[loc1] == MIDDLE || finger[loc1] == RING)) || 
			(row1 > row0 && finger[loc1] == INDEX && (finger[loc0] == MIDDLE || finger[loc0] == RING))) return homeJump + homeJumpIndex;
		else return homeJump;
	else if (abs(row0 - row1) > 2)
		if ((row0 > row1 && finger[loc0] == INDEX && (finger[loc1] == MIDDLE || finger[loc1] == RING)) || 
			(row1 > row0 && finger[loc1] == INDEX && (finger[loc0] == MIDDLE || finger[loc0] == RING))) return doubleJump + homeJumpIndex;
		else return doubleJump;

	return 0;
}

int calcRingJump(const int loc0, const int loc1)
{
	if ((finger[loc0] == PINKY && finger[loc1] == MIDDLE) || 
			(finger[loc0] == MIDDLE && finger[loc1] == PINKY)) return ringJump;
	else return 0;
	
}

int calcToCenter(const int loc0, const int loc1)
{
	if (isCenter[loc0] ^ isCenter[loc1]) return toCenter;
	else return 0;
}

/* Only applies for the extended keyboard.
 */
int calcToOutside(const int loc0, const int loc1)
{
	if (fullKeyboard != FK_NO && (isOutside[loc0] ^ isOutside[loc1])) return toOutside;
	else return 0;
}
