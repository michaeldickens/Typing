#include <stdint.h>

typedef const char *Keystroke;
typedef int64_t Value;
typedef struct KeystrokeValue {
	Keystroke theStroke;
	Value theValue;
} KeystrokeValue;
typedef struct KeystrokeValueTable {
    int64_t kvt_allocated;      /*  the number of elements
                                    allocated for kvt_table */
    uint64_t kvt_used;          /*  the number of elements used in
                                    kvt_table   */
    KeystrokeValue *kvt_table;  /*  the actual table    */
} KeystrokeValueTable;

KeystrokeValueTable *createTable();
int64_t findKeyInTable(const Keystroke key,
                        const KeystrokeValueTable *table,
                        const int64_t min,
                        const int64_t max,
                        _Bool *pExists);
void insertIntoTable(const KeystrokeValue *pKV,
                        KeystrokeValueTable *table,
                        int64_t atIndex);
int kvReverseComparingValues(const void *kv1, const void *kv2);
KeystrokeValue *growTable(KeystrokeValueTable *table, int64_t *pCount);
