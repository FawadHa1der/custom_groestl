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
    bs_transpose(bs_state, 1);

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
  bs_transpose_rev(bs_state,1);
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
		                          const uchar* input,
		                          word_t individual_chunk_byte_length, 
                              word_t total_data_length , 
                              word_t* transformed_output) {
  // int index = 0;
  const word_t msglen = individual_chunk_byte_length;
  word_t newMsgLen = msglen;
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

  u8* combinedtransformed_output8 =transformed_output;
  
  // for (int transformIndex = 0; transformIndex < NO_OF_PARALLEL_INPUTS; transformIndex++) {
  //   // first block of all the parallel inputs are placed first and then the second blocks for all the parallel inputs and so on
  //   for (int blockIndex = 0; blockIndex < completeBlockCounter; blockIndex++) {
  //     // ctx->statesize should be equal to 64 or msgLenWithPadding/ ctx->block_counter
  //     memcpy(transformedInput + ((transformIndex * msgLenWithPadding) + (ctx->statesize * blockIndex)), input + (ctx->statesize * blockIndex) ,  ctx->statesize);
  //   }
  // }
  // printf("\n print all input to see if its copied correctly: \n");
  // printAllResultsHashes(transformedInput);

  // printf("\n Input after replication: \n");
  // printArray(transformedInput);

  Transform512Combined(transformed_output, transformedInput, msgLenWithPadding * NO_OF_PARALLEL_INPUTS);
  printAllResultsHashes(transformed_output, completeBlockCounter);

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


// if the chunk_size is less than block_size then we need to create a new buffer so that we can all the chunks of BLOCK_SIZE which are zeroed for missing data.
// output_buffer should be pre allocated. and every data slice should be of size BLOCK_SIZE
void copy_input_buffer_with_min_block_size(uchar* input_buffer, int32_t complete_data_length_bytes, int32_t chunk_length_bytes, uchar* output_buffer){
  int32_t total_chunks = complete_data_length_bytes / chunk_length_bytes;
  const int32_t block_size_bytes = BLOCK_SIZE/8;

  if (chunk_length_bytes < BLOCK_SIZE) {
    // uchar* new_buffer = malloc(total_data_length_in_bytes);
    memset(output_buffer, 0, complete_data_length_bytes);

    for (int i = 0; i < total_chunks; i++) {
      memcpy(output_buffer + (i * block_size_bytes), input_buffer + (i * chunk_length_bytes), chunk_length_bytes);
    }
  }
}

/// for integration with binius rust input
/* hash bit sequence */
// 
HashReturn Hash_binius_input(int hash_bit_len,
                            const BitSequence* data,  // comlete data for all the parallel inputs, data coming from the caller, should never be freed by this program or modified
                            const word_t complete_data_length_bytes,
                            word_t chunk_length_bytes,
                            BitSequence* hash_val,
                            word_t* result_hashes) {

  HashReturn ret;
  hashState context;

  if (complete_data_length_bytes % chunk_length_bytes != 0) {
    printf("ERROR: Data length is not a multiple of chunk length\n");
    return FAIL;
  }

  // if chunk_length_bytes < BLOCK_SIZE then we need to create a new buffer so that we can have all the chunks of BLOCK_SIZE which are zeroed for missing data.
  // output_buffer should be pre allocated. and every data slice should be of size BLOCK_SIZE_BYTES

  uchar* new_complete_data_buffer = NULL; // be very careful with this buffer. It should be freed after use. may be try to figureotu a cleaner way to do this
  if (chunk_length_bytes < BLOCK_SIZE_BYTES) {
    uchar* new_complete_data_buffer = malloc(complete_data_length_bytes);
    copy_input_buffer_with_min_block_size(data, complete_data_length_bytes, chunk_length_bytes, new_complete_data_buffer);
    data = new_complete_data_buffer;
    chunk_length_bytes = BLOCK_SIZE_BYTES;
  }
 
  // might have to spawn multiple threads here.
  // for now just one thread
  word_t  total_chunks = complete_data_length_bytes / chunk_length_bytes; // total chunks correspond to total groestl 256 hashes but not bitsliced instances.
  word_t  top_level_instances =  total_chunks / WORD_SIZE ;  // this corresponds to bitsliced instances. Each instance will compute WORDS_SIZE parallel groestl hashes

  const word_t remaining_data_length_bytes_bitsliced_window = (total_chunks % WORD_SIZE)? (total_chunks % WORD_SIZE) * chunk_length_bytes : 0; // TODO handle this case, may be just copy what we have to a new buffer and then call the function again
  const word_t partial_empty_top_level_instance = top_level_instances;//the partial one will be the last one
  if (remaining_data_length_bytes_bitsliced_window != 0) {
    // append an extra top level bitsliced instance to process
    top_level_instances++;
    return FAIL;
  }

  for (int instance = 0 ; instance < top_level_instances; instance++){
    /* initialise */
    if ((ret = Init(&context, hash_bit_len)) != SUCCESS)
      return ret;

    uchar* instance_data = data + (instance * WORD_SIZE * chunk_length_bytes);
    word_t instance_data_length_bytes = WORD_SIZE * chunk_length_bytes;

    word_t *bs_transformed_output = malloc(context.statesize * WORD_SIZE);// [64* BLOCK_SIZE];
    memset(bs_transformed_output, 0, context.statesize * WORD_SIZE);

    u32* bs_transformed_output32 = bs_transformed_output; // temp cast
    // set context.hashbitlen in all of the blocks
    for (int block = 0; block < WORD_SIZE; block++) {
      bs_transformed_output32[2*context.columns-1] = U32BIG((u32)context.hashbitlen);
      bs_transformed_output32 += context.statesize/sizeof(u32);
    }

    uchar* instance_data_when_partial = malloc(instance_data_length_bytes);
    memset(instance_data_when_partial, 0, instance_data_length_bytes);

    if (remaining_data_length_bytes_bitsliced_window && (instance == partial_empty_top_level_instance)) {
      // this is the last instance and it has less than WORD_SIZE chunks
      // we need to copy the data to a new buffer and then call the function again
      memcpy(instance_data_when_partial, instance_data, remaining_data_length_bytes_bitsliced_window);
    }

    /* process message */
    if ((ret = update_binius_input(&context, instance_data, chunk_length_bytes, chunk_length_bytes * WORD_SIZE, bs_transformed_output)) != SUCCESS)
      return ret;

    if (instance_data_when_partial) {
      free(instance_data_when_partial);
      instance_data_when_partial = NULL;      
    }

    /* finalise */
    ret = Final(&context, bs_transformed_output, hash_val);
    free(bs_transformed_output); 
  }

  if (new_complete_data_buffer != NULL) { // only in certain cases we need to free this buffer
    free(new_complete_data_buffer);
    new_complete_data_buffer = NULL;
  }


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
