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
#include <time.h>

void bs_OutputTransformation512(word_t *outputTransformation);
void printHexArray(unsigned char *array, uint size);


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
    bs_transpose(bs_state);

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
         //   memset(input_space,0,BS_BLOCK_SIZE);
            memcpy(input_space, msg +  offset, BS_BLOCK_SIZE);
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

    word_t bs_p_round_constant[BLOCK_SIZE];
    // word_t bs_m64_m[BLOCK_SIZE];
    word_t bs_m64_hm[BLOCK_SIZE];

    for (int word_index = 0; word_index < BLOCK_SIZE; word_index ++) {
        // bs_m64_m[word_index] = input[word_index];
        bs_m64_hm[word_index] = state[word_index] ;
    }

    for (word_t round = 0; round < 10; round++)
    {
        memset(bs_p_round_constant, 0, sizeof (bs_p_round_constant));
        bs_generate_roundc_matrix_p_minimal(bs_p_round_constant, round);
        // XOR with round constants
        for (int word_index = 0; word_index < BLOCK_SIZE; word_index ++) {
            bs_m64_hm[word_index] ^= bs_p_round_constant[word_index]; // for P
        }

        // P 
        bs_apply_sbox(bs_m64_hm);
        bs_shiftrows_p(bs_m64_hm);
        bs_mixbytes(bs_m64_hm);
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
HashReturn update_binius_input(hashState* ctx,
		                          const BitSequence* input,
		                          DataLength individual_chunk_bit_length, 
                              word_t total_data_length , word_t* transformedOutput) {
  int index = 0;
  const int msglen = (int)(individual_chunk_bit_length/8);
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
  // printf("\n early early Input: \n");
  // printArray(input);

  int msgLenWithPadding = completeBlockCounter * ctx->statesize;
  const int NO_OF_PARALLEL_INPUTS = WORD_SIZE;

  uchar* transformedInput = malloc(msgLenWithPadding * NO_OF_PARALLEL_INPUTS);
  memset(transformedInput, 0, msgLenWithPadding * NO_OF_PARALLEL_INPUTS);

  u8* combinedTransformedOutput8 =transformedOutput;
  
  for (int transformIndex = 0; transformIndex < NO_OF_PARALLEL_INPUTS; transformIndex++) {
    // first block of all the parallel inputs are placed first and then the second blocks for all the parallel inputs and so on
    for (int blockIndex = 0; blockIndex < completeBlockCounter; blockIndex++) {
      // ctx->statesize should be equal to 64 or msgLenWithPadding/ ctx->block_counter
      memcpy(transformedInput + ((transformIndex * msgLenWithPadding) + (ctx->statesize * blockIndex)), input + (ctx->statesize * blockIndex) ,  ctx->statesize);
    }
  }
  // printf("\n print all input to see if its copied correctly: \n");
  // printAllResultsHashes(transformedInput);

  // printf("\n Input after replication: \n");
  // printArray(transformedInput);

  Transform512Combined(transformedOutput, transformedInput, msgLenWithPadding * NO_OF_PARALLEL_INPUTS);
  printAllResultsHashes(transformedOutput, completeBlockCounter);

/*****************************/
  // prev call below
  // Transform(transformedOutput, input, newMsgLen);
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
  // printf("\n early early Input: \n");
  // printArray(input);

  int msgLenWithPadding = completeBlockCounter * ctx->statesize;
  const int NO_OF_PARALLEL_INPUTS = WORD_SIZE;

  uchar* transformedInput = malloc(msgLenWithPadding * NO_OF_PARALLEL_INPUTS);
  memset(transformedInput, 0, msgLenWithPadding * NO_OF_PARALLEL_INPUTS);

  u8* combinedTransformedOutput8 =transformedOutput;
  
  for (int transformIndex = 0; transformIndex < NO_OF_PARALLEL_INPUTS; transformIndex++) {
    // first block of all the parallel inputs are placed first and then the second blocks for all the parallel inputs and so on
    for (int blockIndex = 0; blockIndex < completeBlockCounter; blockIndex++) {
      // ctx->statesize should be equal to 64 or msgLenWithPadding/ ctx->block_counter
      memcpy(transformedInput + ((transformIndex * msgLenWithPadding) + (ctx->statesize * blockIndex)), input + (ctx->statesize * blockIndex) ,  ctx->statesize);
    }
  }
  // printf("\n print all input to see if its copied correctly: \n");
  // printAllResultsHashes(transformedInput);

  // printf("\n Input after replication: \n");
  // printArray(transformedInput);

  Transform512Combined(transformedOutput, transformedInput, msgLenWithPadding * NO_OF_PARALLEL_INPUTS);
  printAllResultsHashes(transformedOutput, completeBlockCounter);

/*****************************/
  // prev call below
  // Transform(transformedOutput, input, newMsgLen);
  return SUCCESS;
}

void printAllResultsHashes(word_t* array, int block_counter) {
  int i;
  for (i = 0; i < BLOCK_SIZE ; i+= 8 * block_counter) {
    printf("\n Result hash for %d\n", i/8);
    printHexArray(&array[i], 64);// i-4 because last 256 bits contain the answer
  }
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


/// for integration with binius rust input
/* hash bit sequence */
HashReturn Hash_binius_input(int hashbitlen,
		const BitSequence* data, 
		DataLength databitlen,
		BitSequence* hashval,
    word_t* result_hashes) {

  HashReturn ret;
  hashState context;

  /* initialise */
  if ((ret = Init(&context, hashbitlen)) != SUCCESS)
    return ret;

  word_t *combinedTransformedOutput = malloc(context.statesize * WORD_SIZE);// [64* BLOCK_SIZE];
  memset(combinedTransformedOutput, 0, context.statesize * WORD_SIZE);

  u32* combinedTransformedOutput32 = combinedTransformedOutput; // temp cast
  /* allocate memory for state and data buffer */

  // set context.hashbitlen in all of the blocks that are copied
  for (int block = 0; block < WORD_SIZE; block++) {
    combinedTransformedOutput32[2*context.columns-1] = U32BIG((u32)context.hashbitlen);
    combinedTransformedOutput32 += context.statesize/sizeof(u32);
  }
 // combinedTransformedOutput32[2*context.columns-1] = U32BIG((u32)context.hashbitlen);

  /* process message */
  if ((ret = Update(&context, data, databitlen, combinedTransformedOutput)) != SUCCESS)
    return ret;

  /* finalise */
  ret = Final(&context, combinedTransformedOutput, hashval);


  free(combinedTransformedOutput); 

  return ret;
}

/////

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

  // set context.hashbitlen in all of the blocks that are copied
  for (int block = 0; block < WORD_SIZE; block++) {
    combinedTransformedOutput32[2*context.columns-1] = U32BIG((u32)context.hashbitlen);
    combinedTransformedOutput32 += context.statesize/sizeof(u32);
  }
 // combinedTransformedOutput32[2*context.columns-1] = U32BIG((u32)context.hashbitlen);

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

////////////////////////////////////////RUST INTEGRATION///////////////////////////

void process_packed_array(PackedPrimitiveType *array, size_t total_length, size_t chunk_size)  {
  printf("incoming size id: %d", total_length);        
  for (size_t i = 0; i < total_length; i++) {
      // Example processing
      // Accessing high and low parts of a 128-bit integer
      uint64_t high_part = array[i].value.high;
      uint64_t low_part = array[i].value.low;
      if (i < 10){
        printf("High: %" PRIu64 ", Low: %" PRIu64 "\n", high_part, low_part);        
      }
  }
}

void populate_scaled_packed_fields(ScaledPackedField *array, size_t length) {
    for (size_t i = 0; i < length; i++) {
        for (size_t j = 0; j < 2; j++) {  // Assuming N=2
            array[i].elements[j].value.high = i + j;  // Example logic
            array[i].elements[j].value.low = i + j + 100;  // Example logic
        }
    }
}

////////////////////////////////////////END OF RUST INTEGRATION///////////////////////////
#else
#error "MMX instructions must be enabled"
#endif /* __MMX__ */
