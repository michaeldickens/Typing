#include <stdlib.h>
#include <memory.h>
#include <string.h>

#ifdef linux
	#define _FILE_OFFSET_BITS 64
	#define _LARGEFILE_SOURCE
	#define __USE_FILE_OFFSET64

	#include <endian.h>
#else
	#ifdef __BIG_ENDIAN__
		#ifndef BYTE_ORDER
			#define BYTE_ORDER 4321
		#endif
	#else
		#ifndef BYTE_ORDER
			#define BYTE_ORDER 1234
		#endif
	#endif
	#ifndef BIG_ENDIAN
		#define BIG_ENDIAN 4321
	#endif
#endif

typedef unsigned char uchar;
typedef unsigned int uint;

#if defined(__LP64__) || \
	defined(__x86_64__) || \
	defined(__amd64__) || \
	defined(_WIN64) || \
	defined(__sparc64__) || \
	defined(__arch64__) || \
	defined(__powerpc64__) || \
	defined (__s390x__) 
	//	defines for 64 bit
	
	typedef unsigned long long judyvalue;
	typedef unsigned long long JudySlot;
	#define JUDY_key_mask (0x07)
	#define JUDY_key_size 8
	#define JUDY_slot_size 8
	#define JUDY_span_bytes (3 * JUDY_key_size)
	#define JUDY_span_equiv JUDY_2
	#define JUDY_radix_equiv JUDY_8

#else
	//	defines for 32 bit
	
	typedef uint judyvalue;
	typedef uint JudySlot;
	#define JUDY_key_mask (0x03)
	#define JUDY_key_size 4
	#define JUDY_slot_size 4
	#define JUDY_span_bytes (7 * JUDY_key_size)
	#define JUDY_span_equiv JUDY_4
	#define JUDY_radix_equiv JUDY_8

#endif

#define JUDY_mask (~(JudySlot)0x07)

//	define the alignment factor for judy nodes and allocations
//	to enable this feature, set to 64

#define JUDY_cache_line 8	// minimum size is 8 bytes

typedef struct {
	void *seg;			// next used allocator
	uint next;			// next available offset
} JudySeg;

typedef struct {
	JudySlot next;		// judy object
	uint off;			// offset within key
	int slot;			// slot within object
} JudyStack;

typedef struct {
	JudySlot root[1];	// root of judy array
	void **reuse[8];	// reuse judy blocks
	JudySeg *seg;		// current judy allocator
	uint level;			// current height of stack
	uint max;			// max height of stack
	JudyStack stack[1];	// current cursor
} Judy;
