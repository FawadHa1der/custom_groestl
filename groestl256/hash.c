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

void bs_OutputTransformation512(word_t *outputTransformation);




// /* apply the output transformation after identifying variant */
// void OutputTransformation(u32 *output) {
//     OutputTransformation512(output);
// }


/* digest part of a message in short variants */
// total msglen in bytes
int Transform512Combined(word_t *bs_state, const u8 *msg, int msglen) {

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
            memcpy(input_space, msg +  offset, BS_BLOCK_SIZE);
            bs_transpose(input_space);
            bs_cipher(bs_state, input_space);
            offset += BS_BLOCK_SIZE;
            size_left -= BS_BLOCK_SIZE;
            break;
        }
        else
        {
            // memset(input_space,0,BS_BLOCK_SIZE);
            memcpy(input_space, msg +  offset, BS_BLOCK_SIZE);

            /////////only for test, remove///////////

            // bs_transpose(msg);
            // bs_transpose_rev(msg);
            ////////////////////
            bs_transpose(input_space);
            bs_cipher(bs_state, input_space); // output state is in bs_state
            // memmove(outputb, input_space, size);
            offset += BS_BLOCK_SIZE;
            size_left -= BS_BLOCK_SIZE;
        }
    }


  bs_OutputTransformation512(bs_state);
  bs_transpose_rev(bs_state);

  // return 0;
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

    for (word_t round = 0; round < 10; round++)
    {
        bs_generate_roundc_matrix(bs_p_round_constant, bs_q_round_constant, round);
        // XOR with round constants
        for (int word_index = 0; word_index < BLOCK_SIZE; word_index ++) {
            bs_m64_hm[word_index] ^= bs_p_round_constant[word_index]; // for P
        }

        // P 
     //  bs_apply_sbox(bs_m64_hm);
     //  bs_shiftrows_p(bs_m64_hm);
   //    bs_mixbytes(bs_m64_hm);


    }
    for (int word_index = 0; word_index < BLOCK_SIZE; word_index ++) {
      state[word_index] = state[word_index] ^ bs_m64_hm[word_index];
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
		  DataLength databitlen, word_t* transformedOutput) {
  int index = 0;
  const int msglen = (int)(databitlen/8);
  int newMsgLen = msglen;
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

  const int completeBlockCounter =  ctx->block_counter; // ctx->block_counter gets modiefied below so storing in a  temp var here
  while (lengthPadIndex <= LENGTHFIELDLEN) {
    byteInput[newMsgLen - lengthPadIndex] = (u8)ctx->block_counter;
    lengthPadIndex++;
    ctx->block_counter >>= 8;
  }
  /***********************START OF MESSAGE REPLICATION?TRANSFORMATION JUST FOR TESTING*/

  int msgLenWithPadding = completeBlockCounter * ctx->statesize;
  const int NO_OF_PARALLEL_INPUTS = WORD_SIZE;

  uchar* transformedInput = malloc(msgLenWithPadding * NO_OF_PARALLEL_INPUTS);
  memset(transformedInput, 0, msgLenWithPadding * NO_OF_PARALLEL_INPUTS);
  
  for (int transformIndex = 0; transformIndex < NO_OF_PARALLEL_INPUTS; transformIndex++) {
    // first block of all the parallel inputs are placed first and then the second blocks for all the parallel inputs and so on
    for (int blockIndex = 0; blockIndex < completeBlockCounter; blockIndex++) {
      // ctx->statesize should be equal to 64 or msgLenWithPadding/ ctx->block_counter
      memcpy(transformedInput + ((transformIndex * msgLenWithPadding) + (ctx->statesize * blockIndex)), input + (ctx->statesize * blockIndex) ,  ctx->statesize);
    }
  }


 Transform512Combined(transformedOutput, transformedInput, msgLenWithPadding * NO_OF_PARALLEL_INPUTS);

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

  word_t *combinedTransformedOutput = malloc(context.statesize * WORD_SIZE);// [64* BLOCK_SIZE];
  memset(combinedTransformedOutput, 0, context.statesize * WORD_SIZE);

  u32* combinedTransformedOutput32 = combinedTransformedOutput; // temp cast
  /* allocate memory for state and data buffer */
  combinedTransformedOutput32[2*context.columns-1] = U32BIG((u32)context.hashbitlen);

  /* process message */
  if ((ret = Update(&context, data, databitlen, combinedTransformedOutput)) != SUCCESS)
    return ret;

  /* finalise */
  ret = Final(&context, combinedTransformedOutput, hashval);


  free(combinedTransformedOutput); 

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

    const char* message = "my message";
    size_t size = strlen(message);

    unsigned char* data = (unsigned char*)malloc(size + (SIZE512 * 2));
    memset(data, 0, size + (SIZE512 * 2));
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
