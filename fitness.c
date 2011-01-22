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
	calcFitness(k); // Otherwise, k->fitness would not get assigned properly.

	int i;
	for (i = 0; i < 8; ++i) k->fingerUsage[i] = 0;
	k->distance		= 0;
	k->inRoll		= 0;
	k->outRoll		= 0;
	k->sameHand		= 0;
	k->sameFinger	= 0;
	k->rowChange	= 0;
	k->homeJump		= 0;
	k->toCenter		= 0;
	k->toOutside	= 0;

	for (i = 0; i < diLen; ++i) scoreDigraphDirect(k, diKeys[i], diValues[i]);

	for (i = 0; i < monLen; ++i) { 
		k->distance += trueDistance[loc(k, monKeys[i])] * (monValues[i] / 100); // meters 
	}
	
	return 0;
}

int scoreDigraphDirect(Keyboard *k, char digraph[], int multiplier)
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
				costs[30*i + j] += calcToCenter  (i, j);
				costs[30*i + j] += calcToOutside (i, j);
			}
		}
		
	for (i = 0; i < 900; ++i)
		printf("costs[%d] = %d;\n", i, costs[i]);
		
	return 0;
}

// Calculates the fitness of k.
int calcFitness(Keyboard *k)
{
	int i;
	
	k->fitness		= 0;
	k->distance		= 0;
	k->inRoll		= 0;
	k->outRoll		= 0;
	k->sameHand		= 0;
	k->sameFinger	= 0;
	k->rowChange	= 0;
	k->homeJump		= 0;
	k->toCenter		= 0;
	k->toOutside	= 0;
	
	int locs[128];
	for (i = 0; i < 128; ++i) locs[i] = -1;
	
	/* Calculate all the locations before hand. This saves a lot of time. */
	for (i = 0; i < ksize; ++i)
		if (printIt[i]) locs[k->layout[i]] = i;

	for (i = 0; i < diLen; ++i) {
		scoreDigraph(k, diKeys[i], diValues[i], locs);
	}
	
	for (i = 0; i < monLen; ++i) {
		int lc = locs[monKeys[i]];
		if (lc != -1) k->distance += distanceCosts[lc] * monValues[i] * distance;
	}
	
	k->fitness = k->distance + k->inRoll + k->outRoll + k->sameHand + k->sameFinger + k->rowChange + k->homeJump + k->toCenter + k->toOutside;
	if (keepZXCV) k->fitness += calcShortcuts(k) * SHORTCUT;
	if (keepQWERTY) k->fitness += calcQWERTY(k);
	if (keepParentheses) k->fitness += calcParentheses(k);

	++totalCalcFitness;
	return 0;
}

int scoreDigraph(Keyboard *k, char digraph[], int multiplier, int allLocs[])
{
	int locs[2];
	locs[0] = allLocs[digraph[0]];
	locs[1] = allLocs[digraph[1]];
	
	// These all require that the hand be the same.
	if (hand[locs[0]] == hand[locs[1]]) {
		k->inRoll		+= calcInRoll    (locs[0], locs[1]) * multiplier;	
		k->outRoll		+= calcOutRoll   (locs[0], locs[1]) * multiplier;	
//		k->sameHand		+= calcSameHand  (locs[0], locs[1]) * multiplier;
		k->sameHand		+= sameHand						* multiplier;
		k->sameFinger	+= calcSameFinger(locs[0], locs[1]) * multiplier;
		k->rowChange	+= calcRowChange (locs[0], locs[1]) * multiplier;
		k->homeJump		+= calcHomeJump  (locs[0], locs[1]) * multiplier;
		k->toCenter		+= calcToCenter  (locs[0], locs[1]) * multiplier;
		k->toOutside	+= calcToOutside (locs[0], locs[1]) * multiplier;
	}
	
	return 0;
}

int calcShortcuts(Keyboard *k)
{
	int result;
	result = shortcutCosts[loc(k, 'z')] * Z_COST
		+ shortcutCosts[loc(k, 'x')] * X_COST
		+ shortcutCosts[loc(k, 'c')] * C_COST
		+ shortcutCosts[loc(k, 'v')] * V_COST;
	
	return result;
}

int calcQWERTY(Keyboard *k)
{
	NOT_WORK_WITH_full_keyboard("calcQWERTY")
	int result;

	int i, j, pos, value;
	for (i = 0; i < 30; ++i) {
		for (j = 0; j < monLen; ++j) if (monKeys[j] == qwerty[i]) value = (2*monValues[29] - monValues[j]) / 4; // Less common keys are harder to learn.
		
		if ((pos = loc(k, qwerty[i])) != i) result += QWERTY_POS_COST * value;
		if (finger[pos] != finger[i]) result += QWERTY_FINGER_COST * value;
		if (hand[pos] != hand[i]) result += QWERTY_HAND_COST * value;
	}
			
	return result;
}

int calcParentheses(Keyboard *k)
{
	if (full_keyboard != FK_NO) {
		int open_par = loc(k, '(');
		int close_par = loc(k, ')');
		
		if ((open_par+1)%11 == close_par%11 && row[open_par] == row[close_par]) return 0;
		else return PARENTHESES_COST / DIVISOR;
	}
	
	return 0;
}

/* 
 * In and out rolls could be defined in two ways: either two keys 
 * pressed next to each other, or two keys moving in one direction.
 * The former definition was used at first, but the latter may be 
 * more useful.
 */
int calcInRoll(int loc0, int loc1)
{
	if (finger[loc1] == finger[loc0] + 1 && row[loc0] == row[loc1] && !isCenterOrOutside[loc0] && !isCenterOrOutside[loc1]) return inRoll;
	else return 0;
//	if (finger[loc1] > finger[loc0] && row[loc0] == row[loc1] && !isCenterOrOutside[loc0] && !isCenterOrOutside[loc1]) return inRoll;
//	else return 0;
}

int calcOutRoll(int loc0, int loc1)
{
	if (finger[loc1] == finger[loc0] - 1 && row[loc0] == row[loc1] && !isCenterOrOutside[loc0] && !isCenterOrOutside[loc1]) return outRoll;
	else return 0;
//	if (finger[loc1] < finger[loc0] && row[loc0] == row[loc1] && !isCenterOrOutside[loc0] && !isCenterOrOutside[loc1]) return outRoll;
//	else return 0;
}

/* Adds to the score if the same hand is used twice in a row.
 */
int calcSameHand(int loc0, int loc1)
{	
	return sameHand;
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
		int finger0 = finger[loc0];
		int finger1 = finger[loc1];
		if (row[loc0] < row[loc1]) { // loc0 is a higher row
			if (finger0 == PINKY && finger1 == RING  ) return (rowChange + handWarp);
			if (finger0 == INDEX && finger1 == MIDDLE) return (rowChange + handWarp);
			if (finger1 == INDEX && finger0 == MIDDLE) return (rowChange + handSmooth);
		} else { // loc0 is a lower row
			if (finger1 == PINKY && finger0 == RING  ) return (rowChange + handWarp);
			if (finger1 == INDEX && finger0 == MIDDLE) return (rowChange + handWarp);
			if (finger0 == INDEX && finger1 == MIDDLE) return (rowChange + handSmooth);
		}

		return rowChange;
	}
	return 0;
}

int calcHomeJump(int loc0, int loc1)
{
	if ((row[loc0] == 0 && row[loc1] == 2) || (row[loc0] == 2 && row[loc1] == 0))
		if ((row[loc0] == 2 && finger[loc0] == INDEX && (finger[loc1] == MIDDLE || finger[loc1] == RING)) || 
			(row[loc1] == 2 && finger[loc1] == INDEX && (finger[loc0] == MIDDLE || finger[loc0] == RING))) return homeJump + homeJumpIndex;
		else return homeJump;
	else if (abs(row[loc0] - row[loc1]) > 2)
		return doubleJump;
	
//	if (ksize == 30) {
//		if ((loc0 < 10 && loc1 >= 20) || (loc0 >= 20 && loc1 < 10))
//			if ((loc1 == 23 && (loc0 == 1 || loc0 == 2)) || (loc0 == 23 && (loc1 == 1 || loc1 == 2)) || 
//				((loc1 == 25 || loc1 == 26) && (loc0 == 7 || loc0 == 8)) || ((loc0 == 25 || loc0 == 26) && (loc1 == 7 || loc1 == 8))) return homeJump + homeJumpIndex;
//			else return homeJump;
//	} else {
//		if (abs(row[loc0] - row[loc1]) > 2)
//			return doubleJump;
//		else if ((loc0 < 11 && loc1 >= 22) || (loc0 >= 22 && loc1 < 11))
//			if ((loc1 == 25 && (loc0 == 1 || loc0 == 2)) || (loc0 == 25 && (loc1 == 1 || loc1 == 2)) || 
//				(loc1 == 28 && (loc0 == 7 || loc0 == 8)) || (loc0 == 28 && (loc1 == 7 || loc1 == 8))) return homeJump + homeJumpIndex;
//			else return homeJump;
//	}

	return 0;
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
	if (full_keyboard != FK_NO && (isOutside[loc0] ^ isOutside[loc1])) return toOutside;
	else return 0;
}