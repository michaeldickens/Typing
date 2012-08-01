#include "judy64.h"

JudySlot *judy_promote(Judy *judy, JudySlot *next, int idx, judyvalue value, int keysize);
void judy_splitnode(Judy *judy, JudySlot *next, uint size, uint keysize);
void judy_splitspan(Judy *judy, JudySlot *next, uchar *base);

#define JUDY_seg	65536

enum JUDY_types {
	JUDY_radix		= 0,	// inner and outer radix fan-out
	JUDY_1			= 1,	// linear list nodes of designated count
	JUDY_2			= 2,
	JUDY_4			= 3,
	JUDY_8			= 4,
	JUDY_16			= 5,
	JUDY_32			= 6,
	JUDY_span		= 7 	// up to 28 tail bytes of key contiguously stored
};

int JudySize[] = {
	(JUDY_slot_size * 16),						// JUDY_radix node size
	(JUDY_slot_size + JUDY_key_size),			// JUDY_1 node size
	(2 * JUDY_slot_size + 2 * JUDY_key_size),
	(4 * JUDY_slot_size + 4 * JUDY_key_size),
	(8 * JUDY_slot_size + 8 * JUDY_key_size),
	(16 * JUDY_slot_size + 16 * JUDY_key_size),
	(32 * JUDY_slot_size + 32 * JUDY_key_size),
	(JUDY_span_bytes + JUDY_slot_size)
};

judyvalue JudyMask[9] = {
0, 0xff, 0xffff, 0xffffff, 0xffffffff,
#if JUDY_key_size > 4
0xffffffffffULL, 0xffffffffffffULL, 0xffffffffffffffULL, 0xffffffffffffffffULL
#endif
};

#define JUDY_max	JUDY_32

//	allocate judy node

void *judy_alloc(Judy *judy, uint type)
{
uint amt, idx, min;
JudySeg *seg;
void **block;
void **rtn;

	if( !judy->seg )
#ifdef STANDALONE
			judy_abort("illegal allocation from judy clone");
#else
			return NULL;
#endif

	if( type == JUDY_radix )
		type = JUDY_radix_equiv;

	if( type == JUDY_span )
		type = JUDY_span_equiv;

	amt = JudySize[type];

	if( amt & 0x07 )
		amt |= 0x07, amt += 1;

	//	see if free block is already available

	if( (block = judy->reuse[type]) ) {
		judy->reuse[type] = *block;
		memset (block, 0, amt);
		return (void *)block;
	}

	//	break down available larger block
	//	for reuse into smaller blocks

	if( type >= JUDY_1 )
	  for( idx = type; idx++ < JUDY_max; )
		if( block = judy->reuse[idx] ) {
		  judy->reuse[idx] = *block;
		  while( idx-- > type) {
			judy->reuse[idx] = block + JudySize[idx] / sizeof(void *);
			block[JudySize[idx] / sizeof(void *)] = 0;
		  }
		  memset (block, 0, amt);
		  return (void *)block;
		}

	min = amt < JUDY_cache_line ? JUDY_cache_line : amt;

	if( judy->seg->next < min + sizeof(*seg) ) {
		if( (seg = malloc (JUDY_seg)) ) {
			seg->next = JUDY_seg;
			seg->seg = judy->seg;
			judy->seg = seg;
			seg->next -= (JudySlot)seg & (JUDY_cache_line - 1);
		} else {
#ifdef STANDALONE
			judy_abort("Out of virtual memory");
#else
			return NULL;
#endif
		}

#ifdef STANDALONE
		MaxMem += JUDY_seg;
#endif
	}

	//	generate additional free blocks
	//	to fill up to cache line size

	rtn = (void **)((uchar *) judy->seg + judy->seg->next - amt);

	for( idx = type; amt & (JUDY_cache_line - 1); amt <<= 1 ) {
		block = (void **)((uchar *) judy->seg + judy->seg->next - 2 * amt);
		judy->reuse[idx++] = block;
		*block = 0;
	}

	judy->seg->next -= amt;
	memset (rtn, 0, JudySize[type]);
	return (void *)rtn;
}

//	judy_cell: add string to judy array

JudySlot *judy_cell(Judy *judy, uchar *buff, uint max)
{
int size, idx, slot, cnt, tst;
JudySlot *next = judy->root;
judyvalue test, value;
uint off = 0, start;
JudySlot *table;
JudySlot *node;
uint keysize;
uchar *base;

	judy->level = 0;

	while( *next ) {
		if( judy->level < judy->max )
			judy->level++;

		judy->stack[judy->level].off = off;
		judy->stack[judy->level].next = *next;
		size = JudySize[*next & 0x07];

		switch( *next & 0x07 ) {
		case JUDY_1:
		case JUDY_2:
		case JUDY_4:
		case JUDY_8:
		case JUDY_16:
		case JUDY_32:
			keysize = JUDY_key_size - (off & JUDY_key_mask);
			cnt = size / (sizeof(JudySlot) + keysize);
			base = (uchar *)(*next & JUDY_mask);
			node = (JudySlot *)((*next & JUDY_mask) + size);
			start = off;
			slot = cnt;
			value = 0;

			do {
				value <<= 8;
				if( off < max )
					value |= buff[off];
			} while( ++off & JUDY_key_mask );

			//  find slot > key

			while( slot-- ) {
				test = *(judyvalue *)(base + slot * keysize);
#if BYTE_ORDER == BIG_ENDIAN
				test >>= 8 * (JUDY_key_size - keysize); 
#else
				test &= JudyMask[keysize];
#endif
				if( test <= value )
					break;
			}

			judy->stack[judy->level].slot = slot;

			if( test == value ) {		// new key is equal to slot key
				next = &node[-slot-1];

				// is this a leaf?

				if( !(value & 0xFF) )
					return next;

				continue;
			}

			//	if this node is not full
			//	open up cell after slot

			if( !node[-1] ) { // if the entry before node is empty/zero
		 	  memmove(base, base + keysize, slot * keysize);	// move keys less than new key down one slot
#if BYTE_ORDER != BIG_ENDIAN
			  memcpy(base + slot * keysize, &value, keysize);	// copy new key into slot
#else
			  test = value;
			  idx = keysize;

			  while( idx-- )
				  base[slot * keysize + idx] = test, test >>= 8;
#endif
			  for( idx = 0; idx < slot; idx++ )
				node[-idx-1] = node[-idx-2];// copy tree ptrs/cells down one slot

			  node[-slot-1] = 0;			// set new tree ptr/cell
			  next = &node[-slot-1];

			  if( !(value & 0xFF) )
			  	return next;

			  continue;
			}

			if( size < JudySize[JUDY_max] ) {
			  next = judy_promote(judy, next, slot+1, value, keysize);

			  if( !(value & 0xFF) )
				return next;

			  continue;
			}

			//	split full maximal node into JUDY_radix nodes
			//  loop to reprocess new insert

			judy_splitnode(judy, next, size, keysize);
			judy->level--;
			off = start;
			continue;
		
		case JUDY_radix:
			table = (JudySlot *)(*next & JUDY_mask); // outer radix

			if( off < max )
				slot = buff[off++];
			else
				slot = 0;

			// allocate inner radix if empty

			if( !table[slot >> 4] )
				table[slot >> 4] = (JudySlot) judy_alloc(judy, JUDY_radix) | JUDY_radix;

			table = (JudySlot *)(table[slot >> 4] & JUDY_mask);
			judy->stack[judy->level].slot = slot;
			next = &table[slot & 0x0F];

			if( !slot ) // leaf?
				return next;
			continue;

		case JUDY_span:
			base = (uchar *)(*next & JUDY_mask);
			node = (JudySlot *)((*next & JUDY_mask) + JudySize[JUDY_span]);
			cnt = JUDY_span_bytes;
			tst = cnt;

			if( tst > (int)(max - off) )
				tst = max - off;

			value = strncmp((const char *)base, (const char *)(buff + off), tst);

			if( !value && tst < cnt && !base[tst] ) // leaf?
				return &node[-1];

			if( !value && tst == cnt ) {
				next = &node[-1];
				off += cnt;
				continue;
			}

			//	bust up JUDY_span node and produce JUDY_1 nodes
			//	then loop to reprocess insert

			judy_splitspan(judy, next, base);
			judy->level--;
			continue;
		}
	}

	// place JUDY_1 node under JUDY_radix node(s)

	if( off & JUDY_key_mask && off <= max ) {
		base = judy_alloc(judy, JUDY_1);
		keysize = JUDY_key_size - (off & JUDY_key_mask);
		node = (JudySlot  *)(base + JudySize[JUDY_1]);
		*next = (JudySlot)base | JUDY_1;

		//	fill in slot 0 with bytes of key

#if BYTE_ORDER != BIG_ENDIAN
		while( keysize )
			if( off + --keysize < max )
				*base++ = buff[off + keysize];
			else
				base++;
#else
		tst = keysize;

		if( tst > (int)(max - off) )
			tst = max - off;

		memcpy (base, buff + off, tst);
#endif
		if( judy->level < judy->max )
			judy->level++;

		judy->stack[judy->level].next = *next;
		judy->stack[judy->level].slot = 0;
		judy->stack[judy->level].off = off;
		next = &node[-1];
		off |= JUDY_key_mask;
		off++;
	}

	//	produce span nodes to consume rest of key

	while( off <= max ) {
		base = judy_alloc(judy, JUDY_span);
		*next = (JudySlot)base | JUDY_span;
		node = (JudySlot  *)(base + JudySize[JUDY_span]);
		cnt = tst = JUDY_span_bytes;
		if( tst > (int)(max - off) )
			tst = max - off;
		memcpy (base, buff + off, tst);

		if( judy->level < judy->max )
			judy->level++;

		judy->stack[judy->level].next = *next;
		judy->stack[judy->level].slot = 0;
		judy->stack[judy->level].off = off;

		next = &node[-1];
		off += tst;
		if( !base[cnt-1] )	// done on leaf
			break;
	}
	return next;
}

//	return first leaf

JudySlot *judy_first(Judy *judy, JudySlot next, uint off)
{
JudySlot *table, *inner;
uint keysize, size;
JudySlot *node;
int slot, cnt;
uchar *base;

	while( next ) {
		if( judy->level < judy->max )
			judy->level++;

		judy->stack[judy->level].off = off;
		judy->stack[judy->level].next = next;
		size = JudySize[next & 0x07];

		switch( next & 0x07 ) {
		case JUDY_1:
		case JUDY_2:
		case JUDY_4:
		case JUDY_8:
		case JUDY_16:
		case JUDY_32:
			keysize = JUDY_key_size - (off & JUDY_key_mask);
			node = (JudySlot *)((next & JUDY_mask) + size);
			base = (uchar *)(next & JUDY_mask);
			cnt = size / (sizeof(JudySlot) + keysize);

			for( slot = 0; slot < cnt; slot++ )
				if( node[-slot-1] )
					break;

			judy->stack[judy->level].slot = slot;
#if BYTE_ORDER != BIG_ENDIAN
			if( !base[slot * keysize] )
				return &node[-slot-1];
#else
			if( !base[slot * keysize + keysize - 1] )
				return &node[-slot-1];
#endif
			next = node[-slot - 1];
			off = (off | JUDY_key_mask) + 1;
			continue;
		case JUDY_radix:
			table = (JudySlot *)(next & JUDY_mask);
			for( slot = 0; slot < 256; slot++ )
			  if( (inner = (JudySlot *)(table[slot >> 4] & JUDY_mask)) ) {
				if( (next = inner[slot & 0x0F]) ) {
				  judy->stack[judy->level].slot = slot;
				  if( !slot )
					return &inner[slot & 0x0F];
				  else
					break;
				}
			  } else
				slot |= 0x0F;
			off++;
			continue;
		case JUDY_span:
			node = (JudySlot *)((next & JUDY_mask) + JudySize[JUDY_span]);
			base = (uchar *)(next & JUDY_mask);
			cnt = JUDY_span_bytes;
			if( !base[cnt - 1] )	// leaf node?
				return &node[-1];
			next = node[-1];
			off += cnt;
			continue;
		}
	}
	return NULL;
}

void judy_free(Judy *judy, void *block, int type)
{
	if( type == JUDY_radix )
		type = JUDY_radix_equiv;

	if( type == JUDY_span )
		type = JUDY_span_equiv;

	*((void **)(block)) = judy->reuse[type];
	judy->reuse[type] = (void **)block;
	return;
}
		
//	assemble key from current path

uint judy_key (Judy *judy, uchar *buff, uint max)
{
uint len = 0, idx = 0;
int slot, off, type;
uchar *base;
int keysize;

	max--;		// leave room for zero terminator

	while( len < max && ++idx <= judy->level ) {
		slot = judy->stack[idx].slot;
		type = judy->stack[idx].next & 0x07;

		switch( type ) {
		case JUDY_1:
		case JUDY_2:
		case JUDY_4:
		case JUDY_8:
		case JUDY_16:
		case JUDY_32:
			keysize = JUDY_key_size - (judy->stack[idx].off & JUDY_key_mask);
			base = (uchar *)(judy->stack[idx].next & JUDY_mask);
			off = keysize;
#if BYTE_ORDER != BIG_ENDIAN
			while( off-- && len < max )
				if( buff[len] = base[slot * keysize + off] )
					len++;
				else
					break;
#else
			for( off = 0; off < keysize && len < max; off++ )
				if( buff[len] = base[slot * keysize + off] )
					len++;
				else
					break;
#endif
			continue;
		case JUDY_radix:
			if( !slot )
				break;
			buff[len++] = (uchar)slot;
			continue;
		case JUDY_span:
			base = (uchar *)(judy->stack[idx].next & JUDY_mask);

			for( slot = 0; slot < JUDY_span_bytes && base[slot]; slot++ )
			  if( len < max )
				buff[len++] = base[slot];
			continue;
		}
	}
	buff[len] = 0;
	return len;
}

//	judy_nxt: return next entry

JudySlot *judy_nxt (Judy *judy)
{
JudySlot *table, *inner;
int slot, size, cnt;
JudySlot *node;
JudySlot next;
uint keysize;
uchar *base;
uint off;

	if( !judy->level )
		return judy_first (judy, *judy->root, 0);

	while( judy->level ) {
		next = judy->stack[judy->level].next;
		slot = judy->stack[judy->level].slot;
		off = judy->stack[judy->level].off;
		keysize = JUDY_key_size - (off & JUDY_key_mask);
		size = JudySize[next & 0x07];

		switch( next & 0x07 ) {
		case JUDY_1:
		case JUDY_2:
		case JUDY_4:
		case JUDY_8:
		case JUDY_16:
		case JUDY_32:
			cnt = size / (sizeof(JudySlot) + keysize);
			node = (JudySlot *)((next & JUDY_mask) + size);
			base = (uchar *)(next & JUDY_mask);
			if( ++slot < cnt )
#if BYTE_ORDER != BIG_ENDIAN
				if( !base[slot * keysize] )
#else
				if( !base[slot * keysize + keysize - 1] )
#endif
				{
					judy->stack[judy->level].slot = slot;
					return &node[-slot - 1];
				} else {
					judy->stack[judy->level].slot = slot;
					return judy_first (judy, node[-slot-1], (off | JUDY_key_mask) + 1);
				}
			judy->level--;
			continue;

		case JUDY_radix:
			table = (JudySlot *)(next & JUDY_mask);

			while( ++slot < 256 )
			  if( (inner = (JudySlot *)(table[slot >> 4] & JUDY_mask)) ) {
				if( inner[slot & 0x0F] ) {
				  judy->stack[judy->level].slot = slot;
				  return judy_first(judy, inner[slot & 0x0F], off + 1);
				}
			  } else
				slot |= 0x0F;

			judy->level--;
			continue;
		case JUDY_span:
			judy->level--;
			continue;
		}
	}
	return NULL;
}

void *judy_open(uint max)
{
JudySeg *seg;
Judy *judy;
uint amt;

	max++;		// allow for zero terminator on keys

	if( (seg = malloc(JUDY_seg)) ) {
		seg->seg = NULL;
		seg->next = JUDY_seg;
	} else {
#ifdef STANDALONE
		judy_abort("No virtual memory");
#else
		return NULL;
#endif
	}

	amt = sizeof(Judy) + max * sizeof(JudyStack);

	if( amt & (JUDY_cache_line - 1) )
		amt |= JUDY_cache_line - 1, amt++;

#ifdef STANDALONE
	MaxMem += JUDY_seg;
#endif

	seg->next -= (JudySlot)seg & (JUDY_cache_line - 1);
	seg->next -= amt;

	judy = (Judy *)((uchar *)seg + seg->next);
	memset(judy, 0, amt);
 	judy->seg = seg;
	judy->max = max;
	return judy;
}

//	promote full nodes to next larger size

JudySlot *judy_promote(Judy *judy, JudySlot *next, int idx, judyvalue value, int keysize)
{
uchar *base = (uchar *)(*next & JUDY_mask);
int oldcnt, newcnt, slot;
#if BYTE_ORDER == BIG_ENDIAN
	int i;
#endif
JudySlot *newnode, *node;
JudySlot *result;
uchar *newbase;
uint type;

	type = (*next & 0x07) + 1;
	node = (JudySlot *)((*next & JUDY_mask) + JudySize[type-1]);
	oldcnt = JudySize[type-1] / (sizeof(JudySlot) + keysize);
	newcnt = JudySize[type] / (sizeof(JudySlot) + keysize);

	// promote node to next larger size

	newbase = judy_alloc(judy, type);
	newnode = (JudySlot *)(newbase + JudySize[type]);
	*next = (JudySlot)newbase | type;

	//	open up slot at idx

	memcpy(newbase + (newcnt - oldcnt - 1) * keysize, base, idx * keysize);	// copy keys

	for( slot = 0; slot < idx; slot++ )
		newnode[-(slot + newcnt - oldcnt)] = node[-(slot + 1)];	// copy ptr

	//	fill in new node

#if BYTE_ORDER != BIG_ENDIAN
	memcpy(newbase + (idx + newcnt - oldcnt - 1) * keysize, &value, keysize);	// copy key
#else
	i = keysize;

	while( i-- )
	  newbase[(idx + newcnt - oldcnt - 1) * keysize + i] = value, value >>= 8;
#endif
	result = &newnode[-(idx + newcnt - oldcnt)];

	//	copy rest of old node

	memcpy(newbase + (idx + newcnt - oldcnt) * keysize, base + (idx * keysize), (oldcnt - slot) * keysize);	// copy keys

	for( ; slot < oldcnt; slot++ )
		newnode[-(slot + newcnt - oldcnt + 1)] = node[-(slot + 1)];	// copy ptr

	judy->stack[judy->level].next = *next;
	judy->stack[judy->level].slot = idx + newcnt - oldcnt - 1;
	judy_free(judy, (void **)base, type - 1);
	return result;
}

//	construct new node for JUDY_radix entry
//	make node with slot - start entries
//	moving key over one offset

void judy_radix(Judy *judy, JudySlot *radix, uchar *old, int start, int slot, int keysize, uchar key)
{
int size, idx, cnt = slot - start, newcnt;
JudySlot *node, *oldnode;
uint type = JUDY_1 - 1;
JudySlot *table;
uchar *base;

	//	if necessary, setup inner radix node

	if( !(table = (JudySlot *)(radix[key >> 4] & JUDY_mask)) ) {
		table = judy_alloc(judy, JUDY_radix);
		radix[key >> 4] = (JudySlot)table | JUDY_radix;
	}

	oldnode = (JudySlot *)(old + JudySize[JUDY_max]);

	// is this slot a leaf?

	if( !key || !keysize ) {
		table[key & 0x0F] = oldnode[-start-1];
		return;
	}

	//	calculate new node big enough to contain slots

	do {
		type++;
		size = JudySize[type];
		newcnt = size / (sizeof(JudySlot) + keysize);
	} while( cnt > newcnt && type < JUDY_max );

	//	store new node pointer in inner table

	base = judy_alloc(judy, type);
	node = (JudySlot *)(base + size);
	table[key & 0x0F] = (JudySlot)base | type;

	//	allocate node and copy old contents
	//	shorten keys by 1 byte during copy

	for( idx = 0; idx < cnt; idx++ ) {
#if BYTE_ORDER != BIG_ENDIAN
		memcpy (base + (newcnt - idx - 1) * keysize, old + (start + cnt - idx - 1) * (keysize + 1), keysize);
#else
		memcpy (base + (newcnt - idx - 1) * keysize, old + (start + cnt - idx - 1) * (keysize + 1) + 1, keysize);
#endif
		node[-(newcnt - idx)] = oldnode[-(start + cnt - idx)];
	}
}

//	find slot & setup cursor

JudySlot *judy_slot (Judy *judy, uchar *buff, uint max)
{
int slot, size, keysize, tst, cnt;
JudySlot next = *judy->root;
judyvalue value, test = 0;
JudySlot *table;
JudySlot *node;
uint off = 0;
uchar *base;

	judy->level = 0;

	while( next ) {
		if( judy->level < judy->max )
			judy->level++;

		judy->stack[judy->level].off = off;
		judy->stack[judy->level].next = next;
		size = JudySize[next & 0x07];

		switch( next & 0x07 ) {

		case JUDY_1:
		case JUDY_2:
		case JUDY_4:
		case JUDY_8:
		case JUDY_16:
		case JUDY_32:
			base = (uchar *)(next & JUDY_mask);
			node = (JudySlot *)((next & JUDY_mask) + size);
			keysize = JUDY_key_size - (off & JUDY_key_mask);
			cnt = size / (sizeof(JudySlot) + keysize);
			slot = cnt;
			value = 0;

			do {
				value <<= 8;
				if( off < max )
					value |= buff[off];
			} while( ++off & JUDY_key_mask );

			//  find slot > key

			while( slot-- ) {
				test = *(judyvalue *)(base + slot * keysize);
#if BYTE_ORDER == BIG_ENDIAN
				test >>= 8 * (JUDY_key_size - keysize); 
#else
				test &= JudyMask[keysize];
#endif
				if( test <= value )
					break;
			}

			judy->stack[judy->level].slot = slot;

			if( test == value ) {

				// is this a leaf?

				if( !(value & 0xFF) )
					return &node[-slot-1];

				next = node[-slot-1];
				continue;
			}

			return NULL;

		case JUDY_radix:
			table = (JudySlot  *)(next & JUDY_mask); // outer radix

			if( off < max )
				slot = buff[off];
			else
				slot = 0;

			//	put radix slot on judy stack

			judy->stack[judy->level].slot = slot;

			if( (next = table[slot >> 4]) )
				table = (JudySlot  *)(next & JUDY_mask); // inner radix
			else
				return NULL;

			if( !slot )	// leaf?
				if( table[slot & 0x0F] )	// occupied?
					return &table[slot & 0x0F];
				else
					return NULL;

			next = table[slot & 0x0F];
			off += 1;
			break;

		case JUDY_span:
			node = (JudySlot *)((next & JUDY_mask) + JudySize[JUDY_span]);
			base = (uchar *)(next & JUDY_mask);
			cnt = tst = JUDY_span_bytes;
			if( tst > (int)(max - off) )
				tst = max - off;
			value = strncmp((const char *)base, (const char *)(buff + off), tst);
			if( !value && tst < cnt && !base[tst] ) // leaf?
				return &node[-1];

			if( !value && tst == cnt ) {
				next = node[-1];
				off += cnt;
				continue;
			}
			return NULL;
		}
	}

	return NULL;
}
			
//	decompose full node to radix nodes

void judy_splitnode(Judy *judy, JudySlot *next, uint size, uint keysize)
{
int cnt, slot, start = 0;
uint key = 0x0100, nxt;
JudySlot *newradix;
uchar *base;

	base = (uchar  *)(*next & JUDY_mask);
	cnt = size / (sizeof(JudySlot) + keysize);

	//	allocate outer judy_radix node

	newradix = judy_alloc(judy, JUDY_radix);
	*next = (JudySlot)newradix | JUDY_radix;

	for( slot = 0; slot < cnt; slot++ ) {
#if BYTE_ORDER != BIG_ENDIAN
		nxt = base[slot * keysize + keysize - 1];
#else
		nxt = base[slot * keysize];
#endif

		if( key > 0xFF )
			key = nxt;
		if( nxt == key )
			continue;

		//	decompose portion of old node into radix nodes

		judy_radix(judy, newradix, base, start, slot, keysize - 1, (uchar)key);
		start = slot;
		key = nxt;
	}

	judy_radix(judy, newradix, base, start, slot, keysize - 1, (uchar)key);
	judy_free(judy, (void **)base, JUDY_max);
}

//	split open span node

void judy_splitspan(Judy *judy, JudySlot *next, uchar *base)
{
JudySlot *node = (JudySlot *)(base + JudySize[JUDY_span]);
uint cnt = JUDY_span_bytes;
uchar *newbase;
uint off = 0;
#if BYTE_ORDER != BIG_ENDIAN
int i;
#endif

	do {
		newbase = judy_alloc(judy, JUDY_1);
		*next = (JudySlot)newbase | JUDY_1;

#if BYTE_ORDER != BIG_ENDIAN
		i = JUDY_key_size;
		while( i-- )
			*newbase++ = base[off + i];
#else
		memcpy (newbase, base + off, JUDY_key_size);
		newbase += JUDY_key_size;
#endif
		next = (JudySlot *)newbase;

		off += JUDY_key_size;
		cnt -= JUDY_key_size;
	} while( cnt && base[off - 1] );

	*next = node[-1];
	judy_free(judy, base, JUDY_span);
}

//	return cell for first key greater than or equal to given key

JudySlot *judy_strt(Judy *judy, uchar *buff, uint max)
{
JudySlot *cell;

	judy->level = 0;

	if( !max )
		return judy_first(judy, *judy->root, 0);

	if( (cell = judy_slot(judy, buff, max)) )
		return cell;

	return judy_nxt(judy);
}
