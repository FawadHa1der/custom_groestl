
#include <string.h>
#include <pthread.h>
#include <stdint.h>
#include <inttypes.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include "hash.h"

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


    clock_t start, end;
    double cpu_time_used;

    start = clock();

    const char* message = "my message gdfjhghjkfdhgjklfdshgjklfdhgjkfdshkfjsdhgjfdlshgjkfdsghfjdklhgjfkdlghfjdkslhgfdjksgsdfhj    dsdscxcd3232322cc";
    size_t size = strlen(message);

    unsigned char* data = (unsigned char*)malloc(size + (SIZE512 * 2));
    memset(data, 0, size + (SIZE512 * 2));
    memcpy(data, message, size);
    crypto_hash(ct, data, size);

    // printf("Data: %s\n", hostData);
    // printf("Size: %zu\n", dataSize);
    // crypto_hash(ct, hostData, dataSize);
    printf("\nHash:\n ");
    printHexArray(ct, 32);
    printf("done done\n");

    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;

    printf("Time spent: %f seconds\n", cpu_time_used);

}
