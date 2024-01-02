/* hash.c   January 2011
 *
 * Groestl implementation using MMX intrinsics.
 * Has been tested on Intel processors using the gcc compiler only
 * (include the '-std=c99' and '-mmmx' flags when compiling in gcc on a
 * 32-bit machine).
 * 
 * Author: Soeren S. Thomsen
 *
 * This code is placed in the public domain
 */
#define  __MMX__ 81
#if defined (__MMX__)
#include <stdio.h>
#include <stdlib.h>
//#include <mmintrin.h>
#include "hash.h"
#include "tables.h"
#include <string.h>
/* compute a round of P512 */
#define ROUNDP512(m_in, m, r) do {					\
    long long* T_m64 = (long long*)T;						\
    long long zero;								\
    u32* x = (u32*)m_in;						\
    zero = 0;						\
    m[0] = T_m64[0*256+((r)^EXT_BYTE(x[ 0],0))] ^ zero;	\
    m[7] = T_m64[1*256+EXT_BYTE(x[ 0],1)] ^ zero;		\
    m[6] = T_m64[2*256+EXT_BYTE(x[ 0],2)] ^ zero;		\
    m[5] = T_m64[3*256+EXT_BYTE(x[ 0],3)] ^ zero;		\
    m[4] = T_m64[4*256+EXT_BYTE(x[ 1],0)] ^ zero;		\
    m[3] = T_m64[5*256+EXT_BYTE(x[ 1],1)] ^ zero;		\
    m[2] = T_m64[6*256+EXT_BYTE(x[ 1],2)] ^ zero;		\
    m[1] = T_m64[7*256+EXT_BYTE(x[ 1],3)] ^ zero;		\
    m[7] = T_m64[0*256+((r)^0x70^EXT_BYTE(x[14],0))] ^ m[7]; \
    m[6] = T_m64[1*256+EXT_BYTE(x[14],1)] ^ m[6];		\
    m[5] = T_m64[2*256+EXT_BYTE(x[14],2)] ^ m[5];		\
    m[4] = T_m64[3*256+EXT_BYTE(x[14],3)] ^ m[4];		\
    m[3] = T_m64[4*256+EXT_BYTE(x[15],0)] ^ m[3];		\
    m[2] = T_m64[5*256+EXT_BYTE(x[15],1)] ^ m[2];		\
    m[1] = T_m64[6*256+EXT_BYTE(x[15],2)] ^ m[1];		\
    m[0] = T_m64[7*256+EXT_BYTE(x[15],3)] ^ m[0];		\
    m[6] = T_m64[0*256+((r)^0x60^EXT_BYTE(x[12],0))] ^ m[6]; \
    m[5] = T_m64[1*256+EXT_BYTE(x[12],1)] ^ m[5];		\
    m[4] = T_m64[2*256+EXT_BYTE(x[12],2)] ^ m[4];		\
    m[3] = T_m64[3*256+EXT_BYTE(x[12],3)] ^ m[3];		\
    m[2] = T_m64[4*256+EXT_BYTE(x[13],0)] ^ m[2];		\
    m[1] = T_m64[5*256+EXT_BYTE(x[13],1)] ^ m[1];		\
    m[0] = T_m64[6*256+EXT_BYTE(x[13],2)] ^ m[0];		\
    m[7] = T_m64[7*256+EXT_BYTE(x[13],3)] ^ m[7];		\
    m[5] = T_m64[0*256+((r)^0x50^EXT_BYTE(x[10],0))] ^ m[5]; \
    m[4] = T_m64[1*256+EXT_BYTE(x[10],1)] ^ m[4];		\
    m[3] = T_m64[2*256+EXT_BYTE(x[10],2)] ^ m[3];		\
    m[2] = T_m64[3*256+EXT_BYTE(x[10],3)] ^ m[2];		\
    m[1] = T_m64[4*256+EXT_BYTE(x[11],0)] ^ m[1];		\
    m[0] = T_m64[5*256+EXT_BYTE(x[11],1)] ^ m[0];		\
    m[7] = T_m64[6*256+EXT_BYTE(x[11],2)] ^ m[7];		\
    m[6] = T_m64[7*256+EXT_BYTE(x[11],3)] ^ m[6];		\
    m[4] = T_m64[0*256+((r)^0x40^EXT_BYTE(x[ 8],0))] ^ m[4]; \
    m[3] = T_m64[1*256+EXT_BYTE(x[ 8],1)] ^ m[3];		\
    m[2] = T_m64[2*256+EXT_BYTE(x[ 8],2)] ^ m[2];		\
    m[1] = T_m64[3*256+EXT_BYTE(x[ 8],3)] ^ m[1];		\
    m[0] = T_m64[4*256+EXT_BYTE(x[ 9],0)] ^ m[0];		\
    m[7] = T_m64[5*256+EXT_BYTE(x[ 9],1)] ^ m[7];		\
    m[6] = T_m64[6*256+EXT_BYTE(x[ 9],2)] ^ m[6];		\
    m[5] = T_m64[7*256+EXT_BYTE(x[ 9],3)] ^ m[5];		\
    m[3] = T_m64[0*256+((r)^0x30^EXT_BYTE(x[ 6],0))] ^ m[3]; \
    m[2] = T_m64[1*256+EXT_BYTE(x[ 6],1)] ^ m[2];		\
    m[1] = T_m64[2*256+EXT_BYTE(x[ 6],2)] ^ m[1];		\
    m[0] = T_m64[3*256+EXT_BYTE(x[ 6],3)] ^ m[0];		\
    m[7] = T_m64[4*256+EXT_BYTE(x[ 7],0)] ^ m[7];		\
    m[6] = T_m64[5*256+EXT_BYTE(x[ 7],1)] ^ m[6];		\
    m[5] = T_m64[6*256+EXT_BYTE(x[ 7],2)] ^ m[5];		\
    m[4] = T_m64[7*256+EXT_BYTE(x[ 7],3)] ^ m[4];		\
    m[2] = T_m64[0*256+((r)^0x20^EXT_BYTE(x[ 4],0))] ^ m[2]; \
    m[1] = T_m64[1*256+EXT_BYTE(x[ 4],1)] ^ m[1];		\
    m[0] = T_m64[2*256+EXT_BYTE(x[ 4],2)] ^ m[0];		\
    m[7] = T_m64[3*256+EXT_BYTE(x[ 4],3)] ^ m[7];		\
    m[6] = T_m64[4*256+EXT_BYTE(x[ 5],0)] ^ m[6];		\
    m[5] = T_m64[5*256+EXT_BYTE(x[ 5],1)] ^ m[5];		\
    m[4] = T_m64[6*256+EXT_BYTE(x[ 5],2)] ^ m[4];		\
    m[3] = T_m64[7*256+EXT_BYTE(x[ 5],3)] ^ m[3];		\
    m[1] = T_m64[0*256+((r)^0x10^EXT_BYTE(x[ 2],0))] ^ m[1]; \
    m[0] = T_m64[1*256+EXT_BYTE(x[ 2],1)] ^ m[0];		\
    m[7] = T_m64[2*256+EXT_BYTE(x[ 2],2)] ^ m[7];		\
    m[6] = T_m64[3*256+EXT_BYTE(x[ 2],3)] ^ m[6];		\
    m[5] = T_m64[4*256+EXT_BYTE(x[ 3],0)] ^ m[5];		\
    m[4] = T_m64[5*256+EXT_BYTE(x[ 3],1)] ^ m[4];		\
    m[3] = T_m64[6*256+EXT_BYTE(x[ 3],2)] ^ m[3];		\
    m[2] = T_m64[7*256+EXT_BYTE(x[ 3],3)] ^ m[2];		\
  } while (0)

/* compute a round of Q512 */
#define ROUNDQ512(m_in, m, r) do {					\
    long long* T_m64 = (long long*)T;						\
    long long zero;								\
    long long ff;								\
    u32* x = (u32*)m_in;						\
    zero = 0;						\
    ff   = -1;				\
									\
    m_in[0] = (m_in[0]^ff);					\
    m_in[1] = (m_in[1]^ff);					\
    m_in[2] = (m_in[2]^ff);					\
    m_in[3] = (m_in[3]^ff);					\
    m_in[4] = (m_in[4]^ff);					\
    m_in[5] = (m_in[5]^ff);					\
    m_in[6] = (m_in[6]^ff);					\
    m_in[7] = (m_in[7]^ff);					\
									\
    m[7] = T_m64[0*256+EXT_BYTE(x[ 0],0)] ^ zero;		\
    m[5] = T_m64[1*256+EXT_BYTE(x[ 0],1)] ^ zero;		\
    m[3] = T_m64[2*256+EXT_BYTE(x[ 0],2)] ^ zero;		\
    m[1] = T_m64[3*256+EXT_BYTE(x[ 0],3)] ^ zero;		\
    m[0] = T_m64[4*256+EXT_BYTE(x[ 1],0)] ^ zero;		\
    m[6] = T_m64[5*256+EXT_BYTE(x[ 1],1)] ^ zero;		\
    m[4] = T_m64[6*256+EXT_BYTE(x[ 1],2)] ^ zero;		\
    m[2] = T_m64[7*256+((r)^EXT_BYTE(x[ 1],3))] ^ zero;	\
									\
    m[6] = T_m64[0*256+EXT_BYTE(x[14],0)] ^ m[6];		\
    m[4] = T_m64[1*256+EXT_BYTE(x[14],1)] ^ m[4];		\
    m[2] = T_m64[2*256+EXT_BYTE(x[14],2)] ^ m[2];		\
    m[0] = T_m64[3*256+EXT_BYTE(x[14],3)] ^ m[0];		\
    m[7] = T_m64[4*256+EXT_BYTE(x[15],0)] ^ m[7];		\
    m[5] = T_m64[5*256+EXT_BYTE(x[15],1)] ^ m[5];		\
    m[3] = T_m64[6*256+EXT_BYTE(x[15],2)] ^ m[3];		\
    m[1] = T_m64[7*256+((r)^0x70^EXT_BYTE(x[15],3))] ^ m[1]; \
									\
    m[5] = T_m64[0*256+EXT_BYTE(x[12],0)] ^ m[5];		\
    m[3] = T_m64[1*256+EXT_BYTE(x[12],1)] ^ m[3];		\
    m[1] = T_m64[2*256+EXT_BYTE(x[12],2)] ^ m[1];		\
    m[7] = T_m64[3*256+EXT_BYTE(x[12],3)] ^ m[7];		\
    m[6] = T_m64[4*256+EXT_BYTE(x[13],0)] ^ m[6];		\
    m[4] = T_m64[5*256+EXT_BYTE(x[13],1)] ^ m[4];		\
    m[2] = T_m64[6*256+EXT_BYTE(x[13],2)] ^ m[2];		\
    m[0] = T_m64[7*256+((r)^0x60^EXT_BYTE(x[13],3))] ^ m[0]; \
									\
    m[4] = T_m64[0*256+EXT_BYTE(x[10],0)] ^ m[4];		\
    m[2] = T_m64[1*256+EXT_BYTE(x[10],1)] ^ m[2];		\
    m[0] = T_m64[2*256+EXT_BYTE(x[10],2)] ^ m[0];		\
    m[6] = T_m64[3*256+EXT_BYTE(x[10],3)] ^ m[6];		\
    m[5] = T_m64[4*256+EXT_BYTE(x[11],0)] ^ m[5];		\
    m[3] = T_m64[5*256+EXT_BYTE(x[11],1)] ^ m[3];		\
    m[1] = T_m64[6*256+EXT_BYTE(x[11],2)] ^ m[1];		\
    m[7] = T_m64[7*256+((r)^0x50^EXT_BYTE(x[11],3))] ^ m[7]; \
									\
    m[3] = T_m64[0*256+EXT_BYTE(x[ 8],0)] ^ m[3];		\
    m[1] = T_m64[1*256+EXT_BYTE(x[ 8],1)] ^ m[1];		\
    m[7] = T_m64[2*256+EXT_BYTE(x[ 8],2)] ^ m[7];		\
    m[5] = T_m64[3*256+EXT_BYTE(x[ 8],3)] ^ m[5];		\
    m[4] = T_m64[4*256+EXT_BYTE(x[ 9],0)] ^ m[4];		\
    m[2] = T_m64[5*256+EXT_BYTE(x[ 9],1)] ^ m[2];		\
    m[0] = T_m64[6*256+EXT_BYTE(x[ 9],2)] ^ m[0];		\
    m[6] = T_m64[7*256+((r)^0x40^EXT_BYTE(x[ 9],3))] ^ m[6]; \
									\
    m[2] = T_m64[0*256+EXT_BYTE(x[ 6],0)] ^ m[2];		\
    m[0] = T_m64[1*256+EXT_BYTE(x[ 6],1)] ^ m[0];		\
    m[6] = T_m64[2*256+EXT_BYTE(x[ 6],2)] ^ m[6];		\
    m[4] = T_m64[3*256+EXT_BYTE(x[ 6],3)] ^ m[4];		\
    m[3] = T_m64[4*256+EXT_BYTE(x[ 7],0)] ^ m[3];		\
    m[1] = T_m64[5*256+EXT_BYTE(x[ 7],1)] ^ m[1];		\
    m[7] = T_m64[6*256+EXT_BYTE(x[ 7],2)] ^ m[7];		\
    m[5] = T_m64[7*256+((r)^0x30^EXT_BYTE(x[ 7],3))] ^ m[5]; \
									\
    m[1] = T_m64[0*256+EXT_BYTE(x[ 4],0)] ^ m[1];		\
    m[7] = T_m64[1*256+EXT_BYTE(x[ 4],1)] ^ m[7];		\
    m[5] = T_m64[2*256+EXT_BYTE(x[ 4],2)] ^ m[5];		\
    m[3] = T_m64[3*256+EXT_BYTE(x[ 4],3)] ^ m[3];		\
    m[2] = T_m64[4*256+EXT_BYTE(x[ 5],0)] ^ m[2];		\
    m[0] = T_m64[5*256+EXT_BYTE(x[ 5],1)] ^ m[0];		\
    m[6] = T_m64[6*256+EXT_BYTE(x[ 5],2)] ^ m[6];		\
    m[4] = T_m64[7*256+((r)^0x20^EXT_BYTE(x[ 5],3))] ^ m[4]; \
									\
    m[0] = T_m64[0*256+EXT_BYTE(x[ 2],0)] ^ m[0];		\
    m[6] = T_m64[1*256+EXT_BYTE(x[ 2],1)] ^ m[6];		\
    m[4] = T_m64[2*256+EXT_BYTE(x[ 2],2)] ^ m[4];		\
    m[2] = T_m64[3*256+EXT_BYTE(x[ 2],3)] ^ m[2];		\
    m[1] = T_m64[4*256+EXT_BYTE(x[ 3],0)] ^ m[1];		\
    m[7] = T_m64[5*256+EXT_BYTE(x[ 3],1)] ^ m[7];		\
    m[5] = T_m64[6*256+EXT_BYTE(x[ 3],2)] ^ m[5];		\
    m[3] = T_m64[7*256+((r)^0x10^EXT_BYTE(x[ 3],3))] ^ m[3]; \
  } while (0)


/* digest part of a message in short variants */
int Transform512(hashState *ctx, const u8 *msg, int msglen) {
  int i;
  long long m64_m[COLS512], *m64_h, m64_hm[COLS512], tmp[COLS512];
  u32 *msg_32;

  //_mm_empty();

  while (msglen >= SIZE512) {
    msg_32 = (u32*)msg;
    m64_h = (long long*)ctx->chaining;

    for (i = 0; i < COLS512; i++) {
      union {
        u32 msg_32[2];
        long long m64;
      } u;

      u.msg_32[0] = msg_32[2*i];
      u.msg_32[1] = msg_32[2*i+1];
      m64_m[i] = u.m64;
      m64_hm[i] = m64_h[i] ^ m64_m[i];
    }

    ROUNDP512(m64_hm, tmp, 0);
    ROUNDP512(tmp, m64_hm, 1);
    ROUNDP512(m64_hm, tmp, 2);
    ROUNDP512(tmp, m64_hm, 3);
    ROUNDP512(m64_hm, tmp, 4);
    ROUNDP512(tmp, m64_hm, 5);
    ROUNDP512(m64_hm, tmp, 6);
    ROUNDP512(tmp, m64_hm, 7);
    ROUNDP512(m64_hm, tmp, 8);
    ROUNDP512(tmp, m64_hm, 9);

    ROUNDQ512(m64_m, tmp, 0);
    ROUNDQ512(tmp, m64_m, 1);
    ROUNDQ512(m64_m, tmp, 2);
    ROUNDQ512(tmp, m64_m, 3);
    ROUNDQ512(m64_m, tmp, 4);
    ROUNDQ512(tmp, m64_m, 5);
    ROUNDQ512(m64_m, tmp, 6);
    ROUNDQ512(tmp, m64_m, 7);
    ROUNDQ512(m64_m, tmp, 8);
    ROUNDQ512(tmp, m64_m, 9);
    
    for (i = 0; i < COLS512; i++) {
      m64_h[i] = m64_h[i] ^ m64_m [i];
      m64_h[i] = m64_h[i] ^ m64_hm[i];
    }

    //_mm_empty();

    ctx->block_counter++;

    msg += SIZE512;
    msglen -= SIZE512;
  }

  return 0;
}


/* digest part of a message after identifying variant */
int Transform(hashState *ctx, const u8 *msg, int msglen) {
  if (ctx->hashbitlen <= 256) {
    return Transform512(ctx, msg, msglen);
  }
  else {
    // return Transform1024(ctx, msg, msglen);
  }
}

/* apply the output transformation of short variants */
void OutputTransformation512(hashState *ctx) {
  int i;
  long long *m64_h, tmp1[COLS512], tmp2[COLS512];
  m64_h = (long long*)ctx->chaining;

  for (i = 0; i < COLS512; i++) {
    tmp1[i] = m64_h[i];
  }

  ROUNDP512(tmp1, tmp2, 0);
  ROUNDP512(tmp2, tmp1, 1);
  ROUNDP512(tmp1, tmp2, 2);
  ROUNDP512(tmp2, tmp1, 3);
  ROUNDP512(tmp1, tmp2, 4);
  ROUNDP512(tmp2, tmp1, 5);
  ROUNDP512(tmp1, tmp2, 6);
  ROUNDP512(tmp2, tmp1, 7);
  ROUNDP512(tmp1, tmp2, 8);
  ROUNDP512(tmp2, tmp1, 9);

  for (i = 0; i < COLS512; i++) {
    m64_h[i] = m64_h[i] ^ tmp1[i];
  }

  //_mm_empty();
}


/* apply the output transformation after identifying variant */
void OutputTransformation(hashState *ctx) {
  if (ctx->hashbitlen <= 256) {
    OutputTransformation512(ctx);
  }
  else {
    // OutputTransformation1024(ctx);
  }
}

/* initialise context */
HashReturn Init(hashState* ctx,
		int hashbitlen) {
  /* output size (in bits) must be a positive integer less than or
     equal to 512, and divisible by 8 */
  if (hashbitlen <= 0 || (hashbitlen%8) || hashbitlen > 512)
    return BAD_HASHLEN;

  /* set number of state columns and state size depending on
     variant */
  if (hashbitlen <= 256) {
    ctx->columns = COLS512;
    ctx->statesize = SIZE512;
  }
  else {
    // ctx->columns = COLS1024;
    // ctx->statesize = SIZE1024;
  }

  /* allocate memory for state and data buffer */
  ctx->chaining = calloc(ctx->statesize,1);
  ctx->buffer = malloc(ctx->statesize);
  if (ctx->chaining == NULL || ctx->buffer == NULL)
    return FAIL;

  /* set initial value */
  ctx->chaining[2*ctx->columns-1] = U32BIG((u32)hashbitlen);

  /* set other variables */
  ctx->hashbitlen = hashbitlen;
  ctx->buf_ptr = 0;
  ctx->block_counter = 0;
  ctx->bits_in_last_byte = 0;

  return SUCCESS;
}

/* update state with databitlen bits of input */
HashReturn Update(hashState* ctx,
		  const BitSequence* input,
		  DataLength databitlen) {
  int index = 0;
  int msglen = (int)(databitlen/8);
  int rem = (int)(databitlen%8);

  /* non-integral number of message bytes can only be supplied in the
     last call to this function */
  if (ctx->bits_in_last_byte) return FAIL;

  /* if the buffer contains data that has not yet been digested, first
     add data to buffer until full */
  if (ctx->buf_ptr) {
    while (ctx->buf_ptr < ctx->statesize && index < msglen) {
      ctx->buffer[(int)ctx->buf_ptr++] = input[index++];
    }
    if (ctx->buf_ptr < ctx->statesize) {
      /* buffer still not full, return */
      if (rem) {
	ctx->bits_in_last_byte = rem;
	ctx->buffer[(int)ctx->buf_ptr++] = input[index];
      }
      return SUCCESS;
    }

    /* digest buffer */
    ctx->buf_ptr = 0;
    Transform(ctx, ctx->buffer, ctx->statesize);
  }

  /* digest bulk of message */
  Transform(ctx, input+index, msglen-index);
  index += ((msglen-index)/ctx->statesize)*ctx->statesize;

  /* store remaining data in buffer */
  while (index < msglen) {
    ctx->buffer[(int)ctx->buf_ptr++] = input[index++];
  }

  /* if non-integral number of bytes have been supplied, store
     remaining bits in last byte, together with information about
     number of bits */
  if (rem) {
    ctx->bits_in_last_byte = rem;
    ctx->buffer[(int)ctx->buf_ptr++] = input[index];
  }
  return SUCCESS;
}

#define BILB ctx->bits_in_last_byte

/* finalise: process remaining data (including padding), perform
   output transformation, and write hash result to 'output' */
HashReturn Final(hashState* ctx,
		 BitSequence* output) {
  int i, j = 0, hashbytelen = ctx->hashbitlen/8;
  u8 *s = (BitSequence*)ctx->chaining;

  /* pad with '1'-bit and first few '0'-bits */
  if (BILB) {
    ctx->buffer[(int)ctx->buf_ptr-1] &= ((1<<BILB)-1)<<(8-BILB);
    ctx->buffer[(int)ctx->buf_ptr-1] ^= 0x1<<(7-BILB);
    BILB = 0;
  }
  else ctx->buffer[(int)ctx->buf_ptr++] = 0x80;

  /* pad with '0'-bits */
  if (ctx->buf_ptr > ctx->statesize-LENGTHFIELDLEN) {
    /* padding requires two blocks */
    while (ctx->buf_ptr < ctx->statesize) {
      ctx->buffer[(int)ctx->buf_ptr++] = 0;
    }
    /* digest first padding block */
    Transform(ctx, ctx->buffer, ctx->statesize);
    ctx->buf_ptr = 0;
  }
  while (ctx->buf_ptr < ctx->statesize-LENGTHFIELDLEN) {
    ctx->buffer[(int)ctx->buf_ptr++] = 0;
  }

  /* length padding */
  ctx->block_counter++;
  ctx->buf_ptr = ctx->statesize;
  while (ctx->buf_ptr > ctx->statesize-LENGTHFIELDLEN) {
    ctx->buffer[(int)--ctx->buf_ptr] = (u8)ctx->block_counter;
    ctx->block_counter >>= 8;
  }

  /* digest final padding block */
  Transform(ctx, ctx->buffer, ctx->statesize);
  /* perform output transformation */
  OutputTransformation(ctx);

  /* store hash result in output */
  for (i = ctx->statesize-hashbytelen; i < ctx->statesize; i++,j++) {
    output[j] = s[i];
  }

  /* zeroise relevant variables and deallocate memory */
  for (i = 0; i < ctx->columns; i++) {
    ctx->chaining[i] = 0;
  }
  for (i = 0; i < ctx->statesize; i++) {
    ctx->buffer[i] = 0;
  }
  free(ctx->chaining);
  free(ctx->buffer);

  return SUCCESS;
}

/* hash bit sequence */
HashReturn Hash(int hashbitlen,
		const BitSequence* data, 
		DataLength databitlen,
		BitSequence* hashval) {
  HashReturn ret;
  hashState context;

  /* initialise */
  if ((ret = Init(&context, hashbitlen)) != SUCCESS)
    return ret;

  /* process message */
  if ((ret = Update(&context, data, databitlen)) != SUCCESS)
    return ret;

  /* finalise */
  ret = Final(&context, hashval);

  return ret;
}

void PrintHash(const BitSequence* hash,
	       int hashbitlen) {
  int i;
  for (i = 0; i < hashbitlen/8; i++) {
    printf("%02x", hash[i]);
  }
  printf("\n");
}

// /* eBash API */
#define crypto_hash_BYTES 32u
#ifdef crypto_hash_BYTES
int crypto_hash(unsigned char *out, const unsigned char *in, unsigned long long inlen)
{
  if (Hash(256, in, inlen * 8,out) == SUCCESS) return 0;
  return -1;
}
#endif

void printHexArray(unsigned char *array, uint size) {
    int i;
    for(i=0 ; i< size; i++)
    	printf("%02x", array[i]);
    printf("\n");
}


int main(int argc, char **argv) {
    uint *ct, *pt;
    uchar pt_debug[] = { 0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xa, 0xb, 0xc, 0xc, 0xe, 0xe };

    pt = (uint*)pt_debug;
    ct = (uint*)malloc(8 * sizeof(uint)); // Allocating memory for 8 uints

    int dataSize; // Total data size
    size_t maxSharedMemory;
    FILE *file = fopen("groestl256.blb", "r");

    if (file == NULL) {
      printf("Error opening the file.\n");
      return -1;
    }

    fseek(file, 0, SEEK_END);
    dataSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    // Host array
    unsigned char *hostData = (unsigned char*)malloc(dataSize);
    if (hostData == NULL) {
      printf("Error allocating memory.\n");
      fclose(file);
      return -1;
    }

    fread(hostData, sizeof(unsigned char), dataSize, file);
    fclose(file);

    const char* message = "my message";
    size_t size = strlen(message);

    unsigned char* data = (unsigned char*)malloc(size );
    memcpy(data, message, size);
    // data[size] = '\0';

    printf("Data: %s\n", hostData);
    printf("Size: %zu\n", dataSize);
    // crypto_hash(ct, data, size);

    crypto_hash(ct, hostData, dataSize);
    printHexArray(ct, 32);
    printf("yelllooooow\n");
    return 1;
}


#else
#error "MMX instructions must be enabled"
#endif /* __MMX__ */
