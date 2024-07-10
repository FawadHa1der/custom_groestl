
#include <string.h>
#include <pthread.h>
#include <stdint.h>
#include <inttypes.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include "hash.h"


unsigned char* concatenate_copies(unsigned char* input, size_t length, int num_copies) {
  size_t total_length = length * num_copies;
  unsigned char* buffer = (unsigned char*)malloc(total_length);
  if (buffer == NULL) {
    printf("Error allocating memory.\n");
    return NULL;
  }
  for (int i = 0; i < num_copies; i++) {
    memcpy(buffer + (i * length), input, length);
  }
  return buffer;
}


void print_result_hashes(char* result_hashes, unsigned int copies_n_hashes, unsigned int hash_size_bytes) {
//  for (int i = 0; i < 10; i++) {
  for (int i = 0; i < copies_n_hashes; i++) {
    printf("Hash %d (Hex): ", i);
    for (int j = 0; j < hash_size_bytes; j++) {
      printf("%02x", (unsigned char)result_hashes[i * hash_size_bytes + j]);
    }
    printf("\n");
    printf("Hash %d (Decimal): ", i);
    for (int j = 0; j < hash_size_bytes; j++) {
      printf("%d ", (unsigned char)result_hashes[i * hash_size_bytes + j]);
    }
    printf("\n");
  }
}


int main(int argc, char **argv) {

    clock_t start, end;
    double cpu_time_used;

    // for testing 256 bytes of data
    const char* message = "my message gdfjhghjkfdhgjklfdshgjklfdhgjkfdshkfjsdhgjfdlshgjkfdsghfjdklhgjfkdlghfjdkslhgfdjksgsdfhj    dsdscxcd3232322ccmy message gdfjhghjkfdhgjklfdshgjklfdhgjkfdshkfjsdhgjfdlshgjkfdsghfjdklhgjfkdlghfjdkslhgfdjksgsdfhj    dsdscxcd3232322ccd3232322cc2322cc";


    // for testing an edge case to added an extra padded block when the size is 120 bytes
    // const char* message = "my message gdfjhghjkfdhgjklfdshgjklfdhgjkfdshkfjsdhgjfdlshgjkfdsghfjdklhgjfkdlghfjdkslhgfdjksgsdfhj    dsdscxcd3232322cc";

    // for one block
    // const uchar* message = "my message";

    size_t single_item_size_bytes = strlen(message);

    printf("single_item_size_bytes: %zu\n", single_item_size_bytes);

    unsigned int copies_n_hashes = 8192;
    unsigned int hash_size_bytes = BLOCK_SIZE_BYTES/2;
    word_t complete_data_list_length_bytes = single_item_size_bytes * copies_n_hashes;
    word_t result_hashes_length_bytes = hash_size_bytes * copies_n_hashes;
    uchar* data_list = concatenate_copies(message, single_item_size_bytes, copies_n_hashes);
    uchar* result_hashes = (char*)malloc(hash_size_bytes * copies_n_hashes);
    memset(result_hashes, 0, hash_size_bytes * copies_n_hashes);
    start = clock();
    groestl_bs_hash(result_hashes, data_list, complete_data_list_length_bytes, single_item_size_bytes);
    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    print_result_hashes(result_hashes, copies_n_hashes, hash_size_bytes);

    printf("Time spent: %f seconds\n", cpu_time_used);

}
