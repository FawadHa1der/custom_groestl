#ifndef __hash_h
#define __hash_h

#include <stdio.h>
#include <stdlib.h>
//#include "brg_endian.h"
#include "bs.h"


#if defined( __BIG_ENDIAN__ ) && defined( __LITTLE_ENDIAN__ )
#  if defined( __BYTE_ORDER__ ) && __BYTE_ORDER__ == __BIG_ENDIAN__
#    define PLATFORM_BYTE_ORDER IS_BIG_ENDIAN
#  elif defined( __BYTE_ORDER__ ) && __BYTE_ORDER__ == __LITTLE_ENDIAN__
#    define PLATFORM_BYTE_ORDER IS_LITTLE_ENDIAN
#  endif
#elif defined( __BIG_ENDIAN__ )
#  define PLATFORM_BYTE_ORDER IS_BIG_ENDIAN
#elif defined( __LITTLE_ENDIAN__ )
#  define PLATFORM_BYTE_ORDER IS_LITTLE_ENDIAN
#endif

typedef unsigned char uint8_t; 
typedef unsigned int uint32_t; 
//typedef long long uint64_t; 


typedef unsigned char   uchar;
typedef unsigned int    uint;   /* assuming sizeof(uint) == 4 */

// #ifndef BRG_UI32
// #  define BRG_UI32
// #  if UINT_MAX == 4294967295u
// #    define li_32(h) 0x##h##u
//      typedef unsigned int uint_32t;
// #  elif ULONG_MAX == 4294967295u
// #    define li_32(h) 0x##h##ul
//      typedef unsigned long uint_32t;
// #  elif defined( _CRAY )
// #    error This code needs 32-bit data types, which Cray machines do not provide
// #  else
// #    error Please define uint_32t as a 32-bit unsigned integer type in brg_types.h
// #  endif
// #endif

#define li_32(h) 0x##h##u // TODO : make sure this logic works on all platforms

// #if (PLATFORM_BYTE_ORDER == IS_BIG_ENDIAN)
// #define EXT_BYTE(var,n) ((uchar)((uint64_t)(var) >> (8*(7-(n)))))
// #define U32BIG(a) (a)
// #endif /* IS_BIG_ENDIAN */

#define ROTL32(a,n) ((((a)<<(n))|((a)>>(32-(n))))&li_32(ffffffff))

#if (PLATFORM_BYTE_ORDER == IS_LITTLE_ENDIAN)
#define EXT_BYTE(var,n) ((uchar)((uint64_t)(var) >> (8*n)))
#define U32BIG(a) \
  ((ROTL32(a, 8) & li_32(00FF00FF)) |		\
   (ROTL32(a,24) & li_32(FF00FF00)))
#endif /* IS_LITTLE_ENDIAN */

/* some sizes (number of bytes) */
#define ROWS 8
#define LENGTHFIELDLEN ROWS
#define COLS512 8
#define COLS1024 16
#define SIZE512 (ROWS*COLS512)
#define SIZE1024 (ROWS*COLS1024)

#define ROUNDS512 10
#define ROUNDS1024 14

#define ROTL32(a,n) ((((a)<<(n))|((a)>>(32-(n))))&li_32(ffffffff))



typedef unsigned char BitSequence;
typedef unsigned long long DataLength;
typedef enum { SUCCESS = 0, FAIL = 1, BAD_HASHLEN = 2 } HashReturn;
typedef struct {
  int hashbitlen;           /* output length in bits */
  int columns;              /* no. of columns in state */
  int statesize;            /* total no. of bytes in state */
} hashState;

void printAllResultsHashes(word_t* array, int block_counter);
/* helper functions */
void PrintHash(const BitSequence*, int);
void printHexArray(unsigned char *array, uint size);




/////////////////RUST INTEGRATION/////////////////////

int groestl_bs_hash(unsigned char *result_hashes, const unsigned char *in, word_t total_length_bytes, word_t chunk_length_bytes );
typedef struct {
    uint64_t low;
    uint64_t high;
} M128;

typedef struct {
    M128 value;
} PackedPrimitiveType;

typedef struct {
    PackedPrimitiveType elements[2]; // For N=2
} ScaledPackedField;

void process_packed_array(PackedPrimitiveType *array, size_t total_length, size_t chunk_size) ;
void populate_scaled_packed_fields(ScaledPackedField *array, size_t length);
///////////////////END OF RUST INTEGRATION/////////////////////

#endif /* __hash_h */
