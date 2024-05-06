#include <stdio.h>
#include <stdlib.h>

void printHexArray(unsigned char *array, unsigned int size) {
    int i;
    for(i = 0; i < size; i += 16) {
        printf("Decimal: ");
        for(int j = i; j < i + 16 && j < size; j++) {
            printf("%d ", array[j]);
        }
        printf("\nHex: ");
        for(int j = i; j < i + 16 && j < size; j++) {
            printf("%02x ", array[j]);
        }
        printf("\n");
    }
}

float doubler(float x) {
    printf("doubler called with %f\n", x);
    return x * 2;
}


// void printHexArray(unsigned char *array, unsigned int size) {
//     int i;
//     for(i = 0; i < size; i += 16) {
//         printf("Decimal: ");
//         for(int j = i; j < i + 16 && j < size; j++) {
//             printf("%d ", array[j]);
//         }
//         printf("\nHex: ");
//         for(int j = i; j < i + 16 && j < size; j++) {
//             printf("%02x ", array[j]);
//         }
//         printf("\n");
//     }
// }

int crypto_hash(unsigned char *out, const unsigned char *in, unsigned long long inlen){
    // just print out the input
    printHexArray(in, inlen);
}