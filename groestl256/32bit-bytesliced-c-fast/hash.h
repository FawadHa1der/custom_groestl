#ifndef __hash_h
#define __hash_h

// #include "crypto_uint8.h"
// #include "crypto_uint32.h"
// #include "crypto_uint64.h"
// #include "crypto_hash.h"

typedef unsigned char uint8_t; 
typedef unsigned int uint32_t; 
//typedef long long uint64_t; 


typedef unsigned char   uchar;
typedef unsigned int    uint;   /* assuming sizeof(uint) == 4 */


/* some sizes (number of bytes) */
#define ROWS 8
#define COLS512 8

#define SIZE512 (ROWS*COLS512)

#define HASH_BIT_LEN 256


typedef unsigned char BitSequence;
typedef unsigned long long DataLength;

int crypto_hash(unsigned char *out,
		const unsigned char *in,
		unsigned long long len);

#endif /* __hash_h */
