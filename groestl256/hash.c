#include <stdio.h>
#include <stdlib.h>
#include "hash.h"
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
int bs_transform_512(word_t *bs_state, const uchar *msg, int msglen) {

   uint64_t *msg_64;
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
            bs_transpose(input_space, 1);
            bs_cipher(bs_state, input_space);
            offset += BS_BLOCK_SIZE;
            size_left -= BS_BLOCK_SIZE;
            break;
        }
        else
        {
         //   memset(input_space,0,BS_BLOCK_SIZE);
            memcpy(input_space, msg +  offset, BS_BLOCK_SIZE);
            bs_transpose(input_space, 1);
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
  // ctx->block_counter = 0;

  return SUCCESS;
}

void copy_result_hashes(uchar* transformed_hashed_output, uchar* result_hashes) {
  int i = 0, j = 0;
  int result_hash_size_bytes = BLOCK_SIZE_BYTES/2;//BLOCK_SIZE_BYTES/2 since it takes 64 bytes(256 bits) to store the result hash

  for (i = 0; i < BS_BLOCK_SIZE ; i+= BLOCK_SIZE_BYTES, j+= result_hash_size_bytes ) { 
    // printf("\n Result hash for %d\n", i/WORDS_PER_BLOCK);
    memcpy(result_hashes + j, transformed_hashed_output + i + result_hash_size_bytes, result_hash_size_bytes); //  + result_hash_size_bytes because the result hash is stored in the last 256 bits of the 512 bits
    // printHexArray(&array[i], 64);// 
  }
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

typedef struct groestl_block_info {

  // uchar* input;
  // word_t msglen;
  // word_t total_data_length_bytes;
  word_t original_msg_size_bytes;
  word_t original_num_blocks; // how many blocks kdoes it take to store the initial message without the potential extra padded block.
  word_t final_num_blocks; // make sure we write 64 bytes as the block counter. As per groestl docs 8 bytes is reserved for the block counter
  word_t  final_msg_size_bytes;// after padding is added according to groestl 256
  // word_t* transformed_output;
}Groestl_block_info;

// given a chunck size we need to calculate the number of groestl blocks that can be formed from the data
Groestl_block_info calculate_blocks_for_chunk(word_t chunk_size_bytes, hashState* ctx) {
  Groestl_block_info block_info;
  block_info.original_msg_size_bytes = chunk_size_bytes;

  word_t running_msg_len_bytes = chunk_size_bytes; // this will get modified depnding upon the padding.
  // uchar* byteInput = input;

  block_info.final_num_blocks = (running_msg_len_bytes / ctx->statesize) + 1; // final_num_blocks might change below if an extra padded block is added
  
  block_info.original_num_blocks = block_info.final_num_blocks;// this is the number of blocks that the original message takes without the extra padded.
  // byteInput[running_msg_len_bytes] = 0x80; // very important to remember.
  running_msg_len_bytes++; // for the 0x80 being added at the end of the original message. Important to append 0x80 to the message wherever the modification is done

  const int remainder = (running_msg_len_bytes)%ctx->statesize;
  int remainder_index = remainder;
  /* store remaining data in buffer */
  if (remainder_index > ctx->statesize - LENGTHFIELDLEN) {
    // extra buffer
    running_msg_len_bytes += ctx->statesize - remainder_index;
    // newMsgLen = newMsgLen + (ctx->statesize - remainder);
    remainder_index = 0;
    block_info.final_num_blocks++;
  }

  running_msg_len_bytes += (ctx->statesize-LENGTHFIELDLEN) - remainder_index;

  running_msg_len_bytes += LENGTHFIELDLEN;
  block_info.final_msg_size_bytes = running_msg_len_bytes;

  // int lengthPad = LENGTHFIELDLEN;
  // int lengthPadIndex = 1;

  // const int completeBlockCounter =  ctx->block_counter; // ctx->block_counter gets modified below so storing in a  temp var here
  // while (lengthPadIndex <= LENGTHFIELDLEN) {
  //   byteInput[newMsgLen - lengthPadIndex] = (u8)ctx->block_counter;
  //   lengthPadIndex++;
  //   ctx->block_counter >>= 8;
  // }

  if (block_info.final_num_blocks > (block_info.original_num_blocks + 1)) {
    // this means that the extra padded block was not added
    printf("ERROR: too many padded blocks\n");
    abort();
  }

  return block_info;
}

// given a chunck size we need to calculate the number of groestl blocks that can be formed from the data
// block_info is already calculated and passed as an argument. Assuming it has correct calculated values
void modify_last_blocks(uchar* last_block_bytes, word_t current_index , Groestl_block_info block_info) {
  // Groestl_block_info block_info;
  // block_info.original_msg_size_bytes = chunk_size_bytes;

  // word_t running_msg_len_bytes = chunk_size_bytes; // this will get modified depnding upon the padding.
  // uchar* byteInput = input;
  int remainder_index = block_info.original_msg_size_bytes % BLOCK_SIZE_BYTES;

  if (current_index == (block_info.original_num_blocks - 1) ) {
    // this means that the extra padded block was not added
    last_block_bytes[remainder_index] = 0x80; // very important to remember. signifies end of the original message
  }



  if (current_index == (block_info.final_num_blocks - 1)){
    // this check will only execute for the last block, either the original last block or the extra padded block
    // adding the block counter at the end of the block
    int length_pad = LENGTHFIELDLEN;
    int length_pad_index = 1;
    // int final_msg_size_remainder_index = block_info.final_msg_size_bytes % BLOCK_SIZE_BYTES;
    word_t block_counter =  block_info.final_num_blocks; // block_info.final_num_blocks gets modified below so storing in a  temp var here

    while (length_pad_index <= LENGTHFIELDLEN) {
      last_block_bytes[BLOCK_SIZE_BYTES - length_pad_index] = (uchar)block_counter;
      length_pad_index++;
      block_counter >>= 8;
    }
  }
}


/// for integration with binius rust input
/* hash bit sequence */
// 
HashReturn hash_binius_input(int hash_bit_len,
                            const uchar* original_data,  // comlete data for all the parallel inputs, data coming from the caller, should never be freed by this program or modified
                            const word_t complete_data_length_bytes,
                            word_t chunk_length_bytes_original,
                            uchar* result_hashes) {

  HashReturn ret;
  hashState context;

  // create a new buffer for every top level instance which will have the input with padding which the groestl 256 expects
  // this new buffer will also block sliced (not bit sliced yet). That is the first blocks from all the chuncks will be placed together and then the second blocks from all the chunks and so on so forth

  if (complete_data_length_bytes % chunk_length_bytes_original != 0) {
    printf("ERROR: Data length is not a multiple of chunk length\n");
    return FAIL;
  }

  /* initialise */
  if ((ret = Init(&context, hash_bit_len)) != SUCCESS)
    return ret;


  Groestl_block_info block_info = calculate_blocks_for_chunk(chunk_length_bytes_original, &context);
  // might have to spawn multiple threads here.
  // for now just one thread
  int  total_chunks = complete_data_length_bytes / chunk_length_bytes_original; // total chunks correspond to total groestl 256 hashes but not bitsliced instances.
  int  top_level_instances = total_chunks / WORD_SIZE ;  // this corresponds to bitsliced instances. Each instance will compute WORD_SIZE parallel groestl hashes
  int chunks_in_bs_block_to_process = total_chunks < WORD_SIZE ? total_chunks : WORD_SIZE; // many not be equal to WORD_SIZE in the last instance

  const word_t remaining_data_length_bytes_bitsliced_window = (total_chunks % WORD_SIZE)? (total_chunks % WORD_SIZE) * chunk_length_bytes_original : 0; // TODO handle this case, may be just copy what we have to a new buffer and then call the function again
  // const word_t partial_empty_top_level_instance = top_level_instances;//the partial one will be the last one
  if (remaining_data_length_bytes_bitsliced_window != 0) {
    // append an extra top level bitsliced instance to process
    top_level_instances++;
  }

  uchar* new_instance_buffer = NULL;
  uchar* instance_copy_from_buffer = NULL;
  word_t new_buffer_block_index = 0;

  for (int instance = 0 ; instance < top_level_instances; instance++){
    new_instance_buffer = malloc(block_info.final_msg_size_bytes * WORD_SIZE);
    memset(new_instance_buffer, 0, block_info.final_msg_size_bytes * WORD_SIZE);
    instance_copy_from_buffer = original_data + chunks_in_bs_block_to_process * instance * chunk_length_bytes_original;
    new_buffer_block_index = 0;
    for (int block_in_a_chunk_index = 0; block_in_a_chunk_index < block_info.final_num_blocks; block_in_a_chunk_index++) {
      for (int chunk_index = 0; chunk_index < chunks_in_bs_block_to_process; chunk_index++, new_buffer_block_index++) {
          if (block_in_a_chunk_index >= (block_info.original_num_blocks - 1)) { // -1 because 0 will be the index for lets say num blocks = 1
            // the block index where the custom groestl padding is added which includes 0x80 at the end of the message and the block counter at the end and a potential extra block
            if (block_in_a_chunk_index == (block_info.original_num_blocks - 1)){
              word_t copy_size_from_last_block_in_chunk =  chunk_length_bytes_original % BLOCK_SIZE_BYTES; // 0 bytes is for the extra padded block, which does not need to be copied from the source.
              memcpy(new_instance_buffer + (new_buffer_block_index * BLOCK_SIZE_BYTES) , instance_copy_from_buffer +  (block_in_a_chunk_index * BLOCK_SIZE_BYTES) + (chunk_index * chunk_length_bytes_original) , copy_size_from_last_block_in_chunk);
              // memcpy(new_instance_buffer + (new_buffer_block_index * BLOCK_SIZE_BYTES) , instance_copy_from_buffer + (chunk_index * block_in_a_chunk_index * BLOCK_SIZE_BYTES) , copy_size_from_last_block_in_chunk);
            }
            modify_last_blocks(new_instance_buffer + (new_buffer_block_index * BLOCK_SIZE_BYTES), block_in_a_chunk_index, block_info);
          }
          else{
              memcpy(new_instance_buffer + (new_buffer_block_index * BLOCK_SIZE_BYTES) , instance_copy_from_buffer +  (block_in_a_chunk_index * BLOCK_SIZE_BYTES) + (chunk_index * chunk_length_bytes_original) , BLOCK_SIZE_BYTES);
          }
      }
    }

    // all the blocks are copied to the new buffer. Now we need to bitslice the buffer and then call the groestl function
    word_t bs_output_size = context.statesize * WORD_SIZE;
    word_t *bs_transformed_output = malloc(bs_output_size);// [64* BLOCK_SIZE];
    memset(bs_transformed_output, 0, bs_output_size);

    uint32_t* bs_transformed_output32 = bs_transformed_output; // temp cast, TODO CLEAN THIS UP
    // set context.hashbitlen in all of the blocks
    for (int block = 0; block < WORD_SIZE; block++) {
      bs_transformed_output32[2*context.columns-1] = U32BIG((uint32_t)context.hashbitlen);
      bs_transformed_output32 += context.statesize/sizeof(uint32_t);
    }

    bs_transform_512(bs_transformed_output, new_instance_buffer, block_info.final_msg_size_bytes * WORD_SIZE);
    int number_of_results_in_bs_output = WORD_SIZE;
    int size_of_result_hash = BLOCK_SIZE_BYTES/2;

    copy_result_hashes(bs_transformed_output, result_hashes + (instance * number_of_results_in_bs_output* size_of_result_hash));
    // printAllResultsHashes(bs_transformed_output, block_info.final_num_blocks);

    // if ((ret = update_binius_input(&context, new_instance_buffer, chunk_length_bytes_original, chunk_length_bytes_original * WORD_SIZE, bs_transformed_output)) != SUCCESS)
    //   return ret;

    // /* finalise */
    // ret = Final(&context, bs_transformed_output, hash_val);
    free(bs_transformed_output); 
    free(new_instance_buffer);
  }

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

int groestl_bs_hash(unsigned char *result_hashes, const unsigned char *in, word_t total_length_bytes, word_t chunk_length_bytes ) {
  HashReturn ret;
  if ((ret = hash_binius_input(256, in, total_length_bytes, chunk_length_bytes, result_hashes)) != SUCCESS) {
    return -1;
  }
  return 0;
}

void printHexArray(unsigned char *array, uint size) {
    int i;
    for(i=0 ; i< size; i++)
    	printf("%02x", array[i]);
    printf("\n");
}

////////////////////////////////////////RUST INTEGRATION///////////////////////////

// void process_packed_array(PackedPrimitiveType *array, size_t total_length, size_t chunk_size)  {
void binius_groestl_bs_hash(ScaledPackedField *result, PackedPrimitiveType *array, size_t total_length, size_t chunk_size) {
  // printf("incoming size id: %d", total_length);        
  // for (size_t i = 0; i < total_length; i++) {
  //     // Example processing
  //     // Accessing high and low parts of a 128-bit integer
  //     uint64_t high_part = array[i].value.high;
  //     uint64_t low_part = array[i].value.low;
  //     if (i < 10){
  //       printf("High: %" PRIu64 ", Low: %" PRIu64 "\n", high_part, low_part);        
  //     }
  // }
  groestl_bs_hash(result, array, total_length, chunk_size);
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
