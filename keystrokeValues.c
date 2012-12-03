#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "keystroke.h"

extern void internalError( int code );

/* allocates the internal array for the given table
 *
 * table: the table into which the array should be allocated
 */
_Bool allocateTheTable(KeystrokeValueTable *table)
{
	int64_t allocated = 5000;
	size_t kvSize = sizeof(KeystrokeValue);
	_Bool success;

	table->kvt_table = (KeystrokeValue *) calloc(allocated, kvSize);
	if (table->kvt_table == NULL) {
		internalError(013);
		success = false;
	} else {
		table->kvt_allocated = allocated;
		success = true;
	}
	return success;
}

/* creates the actual KeystrokeValueTable pointer or issues a *
 * diagnostic otherwise. The return value is non-null if the table
 * is successfully created and null if not.
 */
KeystrokeValueTable *createTable()
{
	size_t tableSize = sizeof(KeystrokeValueTable);
	KeystrokeValueTable *theTable = (KeystrokeValueTable *) calloc(1, tableSize);
	_Bool success;

	if (theTable) {
		success = allocateTheTable(theTable);
		if (success == false) {
			free(theTable);
			theTable = NULL;
		}
	} else {
		internalError(5);
	}
	return theTable;
}

/* searches thru the given table and either finds the point where
 * to insert a keystroke with a given value and inserts it or adds
 * the value to the value already stored for that keystroke
 *
 * aKey: the keystroke for which to search, presumed to be non-null
 * aValue: the value to add/assign
 * table: the table thru which to search, presumed to be non-null
 */
void includeKeyInTable(Keystroke aKey, int aValue, KeystrokeValueTable *table)
{
	_Bool alreadyExists;
	int64_t keyIndex = findKeyInTable(aKey,
										table,
										0,
										table->kvt_used,
										&alreadyExists);

	if(alreadyExists) {
		table->kvt_table[keyIndex].theValue += aValue;
	} else {
		KeystrokeValue theKV = { aKey, aValue };
		insertIntoTable(&theKV, table, keyIndex);
   	}
}

/* searches thru a table of a known length for where a certain key
 * either is or should be. The function returns the index in which
 * the key is or should be found.
 * 
 * key: the string for which to look
 * table: the table in which to look
 * min: the minimum index in which to search
 * max: the maximum index in which to search
 * pExists: a pointer to a boolean to indicate if the key has been
 * 		found; this pointer is presumed to be non-null
 */
int64_t findKeyInTable(const Keystroke key,
						const KeystrokeValueTable *table,
						const int64_t min,
						const int64_t max,
						_Bool *pExists)
{
	if( max < min ) {
		 *pExists = false;
		 return min;
	}
	else {
		 int64_t mid = (min + max) / 2;
		 Keystroke stroke = table->kvt_table[mid].theStroke;
		 
		 if( stroke == NULL ) {
		 	 internalError(4);
		 exit(0);
		 }
		 else {
	 		  int relation = strcmp(key, stroke);

			  if( relation > 0 ) {
				  return findKeyInTable(key, table, mid + 1, max, pExists);
			  } else if( relation < 0 ) {
				  return findKeyInTable(key, table, min, mid - 1, pExists);
			  } else {
				  *pExists = true;
				  return mid;
			  }
		 }
	}
}

/* grows the given table by doubling the allocated number of items
 *
 * table: the table to grow
 * pCount: a pointer to the number of items already allocated,
 * 		   presumed to be non-null
 */
KeystrokeValue *growTable(KeystrokeValueTable *table, int64_t *pCount)
{
	static const int64_t kvSize = sizeof(KeystrokeValue);
	int64_t newCount = 2 * (*pCount);
	int64_t newMemory = newCount * kvSize;
	KeystrokeValue *newTable = (KeystrokeValue *) realloc(table->kvt_table, newMemory);

	if( newTable ) {
		int64_t previousCount = *pCount;
		int64_t previousMemory = previousCount * kvSize;
		int64_t addedMemory = newMemory - previousMemory;
		KeystrokeValue *startingPoint = newTable + previousCount;

		memset(startingPoint, 0, addedMemory);
		*pCount = newCount;
		return newTable;
	} else {
		   internalError(012);
		   exit(0);
	}
}

/* inserts a KeystrokeValue object into a KeystrokeValue table at
 * a given index
 *
 * pKV: a pointer to the KeystrokeValue object to insert
 * table: the table into which the KeystrokeValue object is to be
 *	 inserted
 * atIndex: the index at which to insert the KeystrokeValue object
 * allocated: the number of KeystrokeValue objects, used or unused, in
 *	 the entire table pUsed: the number of KeystrokeValue objects in
 *	 the table which have been used, presumed to be non-null
 */
void insertIntoTable(const KeystrokeValue *pKV,
	 				  KeystrokeValueTable *table,
				  int64_t atIndex)
{
	int64_t allocated = table->kvt_allocated;
	int64_t used = table->kvt_used;
	KeystrokeValue *innerTable = table->kvt_table;

	if(used >= (allocated * 0.9)) {
		innerTable = growTable(table, &allocated);
	}

	KeystrokeValue temp[allocated];
	KeystrokeValue *startingPoint = innerTable + atIndex;
	int64_t numberToBackup = allocated - atIndex;
	static const int64_t kvSize = sizeof(KeystrokeValue);
	int64_t memmoryToBackup = kvSize * numberToBackup;

	memcpy(temp, startingPoint, memmoryToBackup);

	KeystrokeValue *entry = innerTable + atIndex;
	Keystroke stroke = pKV->theStroke;

	entry->theStroke = strdup(stroke);
	entry->theValue = pKV->theValue;
	table->kvt_used = (used + 1);
	
	int64_t copyBackIndex = atIndex + 1;
	KeystrokeValue *copyBackPoint = innerTable + copyBackIndex;
	memcpy(copyBackPoint, temp, memmoryToBackup);
}

/* performs ascending comparison of KeystrokeValues by their value.
 *
 * kv1: pointer to one KeystrokeValue to compare, presumed non-null
 * kv2: pointer to another KeystrokeValue to compare, presumed non-null
 */
int kvComparingValues(const void *kv1, const void *kv2)
{
  KeystrokeValue *kvp1 = (KeystrokeValue *)kv1;
  KeystrokeValue *kvp2 = (KeystrokeValue *)kv2;
  int64_t v1 = kvp1->theValue;
  int64_t v2 = kvp2->theValue;
  int64_t diff = v1 - v2;
  int rv;

  if (diff == 0) {
	rv = 0;
  } else if (diff > 0) {
	rv = 1;
  } else {
	rv = -1;
  }

  return rv;
}

/* performs reverse (descending) comparison of KeystrokeValues by
 * their value.
 *
 * kv1: pointer to one KeystrokeValue to compare, presumed non-null
 * kv2: pointer to another KeystrokeValue to compare, presumed non-null
 */
int kvReverseComparingValues(const void *kv1, const void *kv2)
{
  int comparison = kvComparingValues(kv1, kv2);
  int rv = -1 * comparison;

  return rv;
}
