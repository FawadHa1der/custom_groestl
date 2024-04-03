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
#include <pthread.h>
#include <stdint.h>
#include <inttypes.h>
#include "bs.h"

void bs_OutputTransformation512(word_t *outputTransformation);



/* compute a round of P512 */
#define ROUNDP512(m_in, m, r) do {					\
    u64* T_m64 = (u64*)T;						\
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
    u64* T_m64 = (u64*)T;						\
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



typedef struct {
    // hashState *ctx;        // Pointer to the hash state
    const u8 *msg_block;   // Pointer to the current message block
    long long *resultBlock ;
    pthread_t thread_id;        // Thread ID
    // ... any other arguments needed for processing ...
} ThreadArgs;

// /* apply the output transformation after identifying variant */
// void OutputTransformation(u32 *output) {
//     OutputTransformation512(output);
// }

// size is in bits
int Transform512BitSliced(uint8_t * outputb, uint8_t * inputb, size_t size) {
    // word_t input_space[BLOCK_SIZE];
    // // word_t rk[11][BLOCK_SIZE];

    // size = size / 8;

    // memset(outputb,0,size);
    // word_t * state = (word_t *)outputb;

    // // bs_expand_key(rk, key);
    // while (size > 0)
    // {
    //     if (size < BS_BLOCK_SIZE)
    //     {
    //         memset(input_space,0,BS_BLOCK_SIZE);
    //         memmove(input_space, inputb, size);
    //         bs_cipher(input_space);
    //         memmove(outputb, input_space, size);
    //         size = 0;
    //         state += size;
    //     }
    //     else
    //     {
    //         memmove(state,inputb,BS_BLOCK_SIZE);
    //         bs_cipher(state);
    //         size -= BS_BLOCK_SIZE;
    //         state += BS_BLOCK_SIZE;
    //     }
    // }
}


/* digest part of a message in short variants */
// total msglen in bytes
int Transform512Combined(word_t *bs_state, const u8 *msg, int msglen) {
  int i;
  long long m64_m[COLS512], *m64_h, m64_hm[COLS512], tmp[COLS512];
  u64 *msg_64;
  int offset = 0;


    word_t input_space[BLOCK_SIZE]; // gets bitsliced soon after copy
    // word_t bs_input_space[BLOCK_SIZE];
    // word_t bs_state[BLOCK_SIZE];
    int size_left = msglen;

    // memset(outputb,0,size);
    // word_t * state = (word_t *)outputb;

    // bs_expand_key(rk, key);

    while (size_left > 0)
    {
        if (size_left < BS_BLOCK_SIZE)
        {
            // memset(input_space,0,BS_BLOCK_SIZE);
            memmove(input_space, msg +  offset, BS_BLOCK_SIZE);
            bs_transpose(input_space);
            bs_cipher(bs_state, input_space); // output state is in bs_state
            // memmove(outputb, input_space, size);
            offset += BS_BLOCK_SIZE;
            size_left -= BS_BLOCK_SIZE;
        }
        else
        {
            memmove(input_space, msg +  offset, BS_BLOCK_SIZE);
            bs_transpose(input_space);
            bs_cipher(bs_state, input_space);
            offset += BS_BLOCK_SIZE;
            size_left -= BS_BLOCK_SIZE;
            break;
        }
    }


  bs_OutputTransformation512(bs_state);
  bs_transpose_rev(bs_state);

  // return 0;
}


/* digest part of a message in short variants */
// msglen in bits
int Transform512(u32 *outputTransformation, const u8 *msg, int msglen) {
  int i;
  long long m64_m[COLS512], *m64_h, m64_hm[COLS512], tmp[COLS512];
  u64 *msg_64;

  // Determine the number of blocks
  int num_blocks = msglen / SIZE512;
  //  pthread_t threads[num_blocks];
  // ThreadArgs threads_args[num_blocks];

  // bs_generate_roundc_matrix();

  m64_h = (uint64_t*)outputTransformation;
  while (msglen >= SIZE512) {
    msg_64 = (u64*)msg;

    for (i = 0; i < COLS512; i++) {
      m64_m[i] = msg_64[i];
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

    msg += SIZE512;
    msglen -= SIZE512;   

  }
//  OutputTransformation(outputTransformation);
  return 0;
}


/* digest part of a message after identifying variant */
int Transform(u32 *outputTransformation, const u8 *msg, int msglen) {
    return Transform512(outputTransformation, msg, msglen);
}

/* apply the output transformation of short variants */
void bs_OutputTransformation512(word_t *state) {
  int i;
  long long *m64_h, tmp1[COLS512], tmp2[COLS512];

    word_t bs_p_round_constant[BLOCK_SIZE];
    word_t bs_q_round_constant[BLOCK_SIZE];

    // word_t bs_m64_m[BLOCK_SIZE];
    word_t bs_m64_hm[BLOCK_SIZE];


    for (int word_index = 0; word_index < BLOCK_SIZE; word_index ++) {
        // bs_m64_m[word_index] = input[word_index];
        bs_m64_hm[word_index] = state[word_index] ;
    }

    for (word_t round = 1; round < 10; round++)
    {
        bs_generate_roundc_matrix(bs_p_round_constant, bs_q_round_constant, round);
        // XOR with round constants
        for (int word_index = 0; word_index < BLOCK_SIZE; word_index ++) {
            bs_m64_hm[word_index] ^= bs_p_round_constant[word_index]; // for P
        }

        // P 
        bs_apply_sbox(bs_m64_hm);
        bs_shiftrows_p(bs_m64_hm);
        bs_mixbytes(bs_m64_hm);


        for (int word_index = 0; word_index < BLOCK_SIZE; word_index ++) {
            state[word_index] = state[word_index] ^ bs_m64_hm[word_index];
        }
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

  /* set other variables */
  ctx->hashbitlen = hashbitlen;
  ctx->block_counter = 0;

  return SUCCESS;
}

/* update state with databitlen bits of input */
HashReturn Update(hashState* ctx,
		  const BitSequence* input,
		  DataLength databitlen, u32* transformedOutput) {
  int index = 0;
  const int msglen = (int)(databitlen/8);
  int newMsgLen = msglen;
  int rem = (int)(databitlen%8);
  uchar* byteInput = input;

  ctx->block_counter = msglen / ctx->statesize;
  byteInput[newMsgLen] = 0x80;
  newMsgLen++;

  const int remainder = (newMsgLen)%ctx->statesize;
  int remainderIndex = remainder;
  /* store remaining data in buffer */
  if (remainderIndex > ctx->statesize - LENGTHFIELDLEN) {
    // extra buffer
    while (remainderIndex < ctx->statesize) {
      byteInput[newMsgLen] = 0;
      remainderIndex++;
      newMsgLen++;
    }
    // newMsgLen = newMsgLen + (ctx->statesize - remainder);
    remainderIndex = 0;
    ctx->block_counter++;
  }

  while (remainderIndex < ctx->statesize-LENGTHFIELDLEN) {
    byteInput[newMsgLen] = 0;
    remainderIndex++;
    newMsgLen++;
  }
  ctx->block_counter++;

  // byteInput[newMsgLen + (remainderIndex -1 )] = (u8)ctx->block_counter;
  newMsgLen += LENGTHFIELDLEN;

  int lengthPad = LENGTHFIELDLEN;
  int lengthPadIndex = 1;
  while (lengthPadIndex <= LENGTHFIELDLEN) {
    byteInput[newMsgLen - lengthPadIndex] = (u8)ctx->block_counter;
    lengthPadIndex++;
    ctx->block_counter >>= 8;
  }
  /***********************START OF MESSAGE REPLICATION?TRANSFORMATION JUST FOR TESTING*/

  int msgLenInBytes = newMsgLen/8;
  int NO_OF_PARALLEL_INPUTS = 64;
  uchar* transformedInput = calloc(msgLenInBytes * NO_OF_PARALLEL_INPUTS, 0);
  for (int transformIndex = 0; transformIndex < NO_OF_PARALLEL_INPUTS; transformIndex++) {
    // first block of all the parallel inputs are placed first and then the second blocks for all the parallel inputs and so on
    for (int blockIndex = 0; blockIndex < ctx->block_counter; blockIndex++) {
      // ctx->statesize should be equal to 64 or msgLenInBytes/ ctx->block_counter
      memcpy(transformedInput + ((transformIndex * msgLenInBytes) + (ctx->statesize * blockIndex)), input + (ctx->statesize * blockIndex) ,  ctx->statesize);
    }
  }

 // uchar* combinedTransformedOutput = calloc(ctx->statesize * NO_OF_PARALLEL_INPUTS, 0);
 word_t combinedTransformedOutput[BLOCK_SIZE];

  Transform512Combined(combinedTransformedOutput, transformedInput, msgLenInBytes * NO_OF_PARALLEL_INPUTS);

/*****************************/
  // prev call below
  // Transform(transformedOutput, input, newMsgLen);
  return SUCCESS;
}


/* finalise: process remaining data (including padding), perform
   output transformation, and write hash result to 'output' */
HashReturn Final(hashState* ctx, u32* input,
		 BitSequence* output) {
  int i, j = 0, hashbytelen = ctx->hashbitlen/8;
  u8 *s = input;

  /* store hash result in output */
  for (i = ctx->statesize-hashbytelen; i < ctx->statesize; i++,j++) {
    output[j] = s[i];
  }

  /* zeroise relevant variables and deallocate memory */
  for (i = 0; i < ctx->columns; i++) {
    input[i] = 0;
  }
  // free(ctx->chaining);
  // free(ctx->buffer);
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
    
  u32* transformedOutput = calloc(context.statesize,1);
  /* allocate memory for state and data buffer */
  transformedOutput[2*context.columns-1] = U32BIG((u32)context.hashbitlen);

  /* process message */
  if ((ret = Update(&context, data, databitlen, transformedOutput)) != SUCCESS)
    return ret;

  /* finalise */
  ret = Final(&context, transformedOutput, hashval);


  free(transformedOutput);

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
    ct = (uint*)malloc(8 * sizeof(uint)); // Allocating memory for 8 uints

    int dataSize; // Total data size
    size_t maxSharedMemory;
    FILE *file = fopen("text_generator/pt_1MB.txt", "r");

    if (file == NULL) {
      printf("Error opening the file.\n");
      return -1;
    }

    fseek(file, 0, SEEK_END);
    dataSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    // Host array
    unsigned char *hostData = (unsigned char*)malloc(dataSize + (SIZE512 * 2));
    if (hostData == NULL) {
      printf("Error allocating memory.\n");
      fclose(file);
      return -1;
    }

    fread(hostData, sizeof(unsigned char), dataSize, file);
    fclose(file);

    const char* message = "my message gdfjhghjkfdhgjklfdshgjklfdhgjkfdshkfjsdhgjfdlshgjkfdsghfjdklhgjfkdlghfjdkslhgfdjksgsdfhj    dsdscxcd3232322cc";
    size_t size = strlen(message);

    unsigned char* data = (unsigned char*)malloc(size + (SIZE512 * 2));
    memcpy(data, message, size);
    crypto_hash(ct, data, size);

    // printf("Data: %s\n", hostData);
    // printf("Size: %zu\n", dataSize);
    // crypto_hash(ct, hostData, dataSize);

    printHexArray(ct, 32);
    printf("done done\n");
    return 1;
}


#else
#error "MMX instructions must be enabled"
#endif /* __MMX__ */
