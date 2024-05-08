
#include <string.h>
#include "bs.h"

#if (defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__) ||\
        defined(__amd64__) || defined(__amd32__)|| defined(__amd16__)
#define bs2le(x) (x)
#define bs2be(x) (x)
#elif (defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__) ||\
            (defined(__sparc__))
#define bs2le(x) __builtin_bswap_wordsize(x)
#define bs2be(x) __builtin_bswap_wordsize(x)
#else
#error "endianness not supported"
#endif

    #include <stdio.h>
#include "hash.h"
#include <inttypes.h> // for PRId64 macro
#include "tablesSubBytes.h"
void sbox_bitsliced(word_t *in);
//unbitsliced subbytes
void subBytes(uint8_t* x)
{
	int i;
	for(i=0;i<64;i++)
	{
		x[i] = S[x[i]];
	}
}


void printArray(word_t* array) {
  int i;
  for (i = 0; i < 8; i++) {
    printf("%016llx", array[i]); // Print as 64-bit hexadecimal
    if (i < 7) {
      printf(", ");
    }
  }
}

void bs_addroundkey(word_t * B, word_t rk)
{
    int i;
    for (i = 0; i < BLOCK_SIZE; i++)
        B[i] ^= rk;
}

void bs_apply_sbox(word_t * input)
{
    int i;
    for(i=0; i < BLOCK_SIZE; i+=8)
    {
       bs_sbox(input+i);
     // sbox_bitsliced(input+i);
    }
}

void bs_apply_sbox_rev(word_t * input)
{
    int i;
    for(i=0; i < BLOCK_SIZE; i+=8)
    {
        bs_sbox_rev(input+i);
    }
}

/*July 2011*/
/*Straight-line program for AES s box*/

/*Input is U[0], U[1],...,U[7]*/
/*Output is S[0], S[1],...,S[7]*/
// http://cs-www.cs.yale.edu/homes/peralta/CircuitStuff/CMT.html
void bs_sbox_rev(word_t U[8])
{
    word_t W[8];
    word_t
        T1,T2,T3,T4,T5,T6,T8,
        T9,T10,T13,T14,T15,T16,
        T17,T18,T19,T20,T22,T23,T24,
        T25, T26, T27;

    word_t
        M1,M2,M3,M4,M5,M6,M7,M8,
        M9,M10,M11,M12,M13,M14,M15,
        M16,M17,M18,M19,M20,M21,M22,
        M23,M24,M25,M26,M27,M28,M29,
        M30,M31,M32,M33,M34,M35,M36,
        M37,M38,M39,M40,M41,M42,M43,
        M44,M45,M46,M47,M48,M49,M50,
        M51,M52,M53,M54,M55,M56,M57,
        M58,M59,M60,M61,M62,M63;

    word_t
        P0,P1,P2,P3,P4,P5,P6,P7,P8,
        P9,P10,P11,P12,P13,P14,
        P15,P16,P17,P18,P19,P20,
        P21,P22,P23,P24,P25,P26,
        P27,P28,P29;

    word_t Y5,
        R5, R13, R17, R18, R19;


    T23 = U[7] ^ U[4];
    T22 = ~(U[6] ^ U[4]);
    T2 = ~(U[7] ^ U[6]);
    T1 = U[4] ^ U[3];
    T24 = ~(U[3] ^ U[0]);
    R5 = U[1] ^ U[0];
    T8 = ~(U[6] ^ T23);
    T19 = T22 ^ R5;
    T9 = ~(U[0] ^ T1);
    T10 = T2 ^ T24;
    T13 = T2 ^ R5;
    T3 = T1 ^ R5;
    T25 = ~(U[5] ^ T1);
    R13 = U[6] ^ U[1];
    T17 = ~(U[5] ^ T19);
    T20 = T24 ^ R13;
    T4 = U[3] ^ T8;
    R17 = ~(U[5] ^ U[2]);
    R18 = ~(U[2] ^ U[1]);
    R19 = ~(U[5] ^ U[3]);
    Y5 = U[7] ^ R17;
    T6 = T22 ^ R17;
    T16 = R13 ^ R19;
    T27 = T1 ^ R18;
    T15 = T10 ^ T27;
    T14 = T10 ^ R18;
    T26 = T3 ^ T16;
    M1 = T13 & T6;
    M2 = T23 & T8;
    M3 = T14 ^ M1;
    M4 = T19 & Y5;
    M5 = M4 ^ M1;
    M6 = T3 & T16;
    M7 = T22 & T9;
    M8 = T26 ^ M6;
    M9 = T20 & T17;
    M10 = M9 ^ M6;
    M11 = T1 & T15;
    M12 = T4 & T27;
    M13 = M12 ^ M11;
    M14 = T2 & T10;
    M15 = M14 ^ M11;
    M16 = M3 ^ M2;
    M17 = M5 ^ T24;
    M18 = M8 ^ M7;
    M19 = M10 ^ M15;
    M20 = M16 ^ M13;
    M21 = M17 ^ M15;
    M22 = M18 ^ M13;
    M23 = M19 ^ T25;
    M24 = M22 ^ M23;
    M25 = M22 & M20;
    M26 = M21 ^ M25;
    M27 = M20 ^ M21;
    M28 = M23 ^ M25;
    M29 = M28 & M27;
    M30 = M26 & M24;
    M31 = M20 & M23;
    M32 = M27 & M31;
    M33 = M27 ^ M25;
    M34 = M21 & M22;
    M35 = M24 & M34;
    M36 = M24 ^ M25;
    M37 = M21 ^ M29;
    M38 = M32 ^ M33;
    M39 = M23 ^ M30;
    M40 = M35 ^ M36;
    M41 = M38 ^ M40;
    M42 = M37 ^ M39;
    M43 = M37 ^ M38;
    M44 = M39 ^ M40;
    M45 = M42 ^ M41;
    M46 = M44 & T6;
    M47 = M40 & T8;
    M48 = M39 & Y5;
    M49 = M43 & T16;
    M50 = M38 & T9;
    M51 = M37 & T17;
    M52 = M42 & T15;
    M53 = M45 & T27;
    M54 = M41 & T10;
    M55 = M44 & T13;
    M56 = M40 & T23;
    M57 = M39 & T19;
    M58 = M43 & T3;
    M59 = M38 & T22;
    M60 = M37 & T20;
    M61 = M42 & T1;
    M62 = M45 & T4;
    M63 = M41 & T2;
    P0 = M52 ^ M61;
    P1 = M58 ^ M59;
    P2 = M54 ^ M62;
    P3 = M47 ^ M50;
    P4 = M48 ^ M56;
    P5 = M46 ^ M51;
    P6 = M49 ^ M60;
    P7 = P0 ^ P1;
    P8 = M50 ^ M53;
    P9 = M55 ^ M63;
    P10 = M57 ^ P4;
    P11 = P0 ^ P3;
    P12 = M46 ^ M48;
    P13 = M49 ^ M51;
    P14 = M49 ^ M62;
    P15 = M54 ^ M59;
    P16 = M57 ^ M61;
    P17 = M58 ^ P2;
    P18 = M63 ^ P5;
    P19 = P2 ^ P3;
    P20 = P4 ^ P6;
    P22 = P2 ^ P7;
    P23 = P7 ^ P8;
    P24 = P5 ^ P7;
    P25 = P6 ^ P10;
    P26 = P9 ^ P11;
    P27 = P10 ^ P18;
    P28 = P11 ^ P25;
    P29 = P15 ^ P20;
    W[7] = P13 ^ P22;
    W[6] = P26 ^ P29;
    W[5] = P17 ^ P28;
    W[4] = P12 ^ P22;
    W[3] = P23 ^ P27;
    W[2] = P19 ^ P24;
    W[1] = P14 ^ P23;
    W[0] = P9 ^ P16;

    memmove(U,W,sizeof(W));
}


#define XOR(c,a,b) (c = (a)^(b))
#define AND(c,a,b) (c = (a)&(b))
void sbox_bitsliced(word_t *in) {

word_t S[8];

word_t U0,U1,U2,U3,U4,U5,U6,U7;
word_t T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14,T15,T16,T17,T18,T19,T20,T21,T22,T23,T24,T25,T26,T27;
word_t M1,M2,M3,M4,M5,M6,M7,M8,M9,M10,M11,M12,M13,M14,M15,M16,M17,M18,M19,M20,M21,M22,M23,M24,M25,M26,M27,M28,M29,M30,M31,M32,M33,M34,M35,M36,M37,M38,M39,M40,M41,M42,M43,M44,M45,M46,M47,M48,M49,M50,M51,M52,M53,M54,M55,M56,M57,M58,M59,M60,M61,M62,M63;
word_t L0,L1,L2,L3,L4,L5,L6,L7,L8,L9,L10,L11,L12,L13,L14,L15,L16,L17,L18,L19,L20,L21,L22,L23,L24,L25,L26,L27,L28,L29;
word_t S0,S1,S2,S3,S4,S5,S6,S7;
//word_t out[8];

U0=in[0];
U1=in[1];
U2=in[2];
U3=in[3];
U4=in[4];
U5=in[5];
U6=in[6];
U7=in[7];
XOR(T1,U0,U3);
XOR(T2,U0,U5);
XOR(T3,U0,U6);
XOR(T4,U3,U5);
XOR(T5,U4,U6);
XOR(T6,T1,T5);
XOR(T7,U1,U2);
XOR(T8,U7,T6);
XOR(T9,U7,T7);
XOR(T10,T6,T7);
XOR(T11,U1,U5);
XOR(T12,U2,U5);
XOR(T13,T3,T4);
XOR(T14,T6,T11);
XOR(T15,T5,T11);
XOR(T16,T5,T12);
XOR(T17,T9,T16);
XOR(T18,U3,U7);
XOR(T19,T7,T18);
XOR(T20,T1,T19);
XOR(T21,U6,U7);
XOR(T22,T7,T21);
XOR(T23,T2,T22);
XOR(T24,T2,T10);
XOR(T25,T20,T17);
XOR(T26,T3,T16);
XOR(T27,T1,T12);
AND(M1,T13,T6);
AND(M2,T23,T8);
XOR(M3,T14,M1);
AND(M4,T19,U7);
XOR(M5,M4,M1);
AND(M6,T3,T16);
AND(M7,T22,T9);
XOR(M8,T26,M6);
AND(M9,T20,T17);
XOR(M10,M9,M6);
AND(M11,T1,T15);
AND(M12,T4,T27);
XOR(M13,M12,M11);
AND(M14,T2,T10);
XOR(M15,M14,M11);
XOR(M16,M3,M2);
XOR(M17,M5,T24);
XOR(M18,M8,M7);
XOR(M19,M10,M15);
XOR(M20,M16,M13);
XOR(M21,M17,M15);
XOR(M22,M18,M13);
XOR(M23,M19,T25);
XOR(M24,M22,M23);
AND(M25,M22,M20);
XOR(M26,M21,M25);
XOR(M27,M20,M21);
XOR(M28,M23,M25);
AND(M29,M28,M27);
AND(M30,M26,M24);
AND(M31,M20,M23);
AND(M32,M27,M31);
XOR(M33,M27,M25);
AND(M34,M21,M22);
AND(M35,M24,M34);
XOR(M36,M24,M25);
XOR(M37,M21,M29);
XOR(M38,M32,M33);
XOR(M39,M23,M30);
XOR(M40,M35,M36);
XOR(M41,M38,M40);
XOR(M42,M37,M39);
XOR(M43,M37,M38);
XOR(M44,M39,M40);
XOR(M45,M42,M41);
AND(M46,M44,T6);
AND(M47,M40,T8);
AND(M48,M39,U7);
AND(M49,M43,T16);
AND(M50,M38,T9);
AND(M51,M37,T17);
AND(M52,M42,T15);
AND(M53,M45,T27);
AND(M54,M41,T10);
AND(M55,M44,T13);
AND(M56,M40,T23);
AND(M57,M39,T19);
AND(M58,M43,T3);
AND(M59,M38,T22);
AND(M60,M37,T20);
AND(M61,M42,T1);
AND(M62,M45,T4);
AND(M63,M41,T2);
XOR(L0,M61,M62);
XOR(L1,M50,M56);
XOR(L2,M46,M48);
XOR(L3,M47,M55);
XOR(L4,M54,M58);
XOR(L5,M49,M61);
XOR(L6,M62,L5);
XOR(L7,M46,L3);
XOR(L8,M51,M59);
XOR(L9,M52,M53);
XOR(L10,M53,L4);
XOR(L11,M60,L2);
XOR(L12,M48,M51);
XOR(L13,M50,L0);
XOR(L14,M52,M61);
XOR(L15,M55,L1);
XOR(L16,M56,L0);
XOR(L17,M57,L1);
XOR(L18,M58,L8);
XOR(L19,M63,L4);
XOR(L20,L0,L1);
XOR(L21,L1,L7);
XOR(L22,L3,L12);
XOR(L23,L18,L2);
XOR(L24,L15,L9);
XOR(L25,L6,L10);
XOR(L26,L7,L9);
XOR(L27,L8,L10);
XOR(L28,L11,L14);
XOR(L29,L11,L17);
XOR(S0,L6,L24);
XOR(S1,L16,L26);
XOR(S2,L19,L28);
XOR(S3,L6,L21);
XOR(S4,L20,L22);
XOR(S5,L25,L29);
XOR(S6,L13,L27);
XOR(S7,L6,L23);
in[0]=S0;
in[1]=~S1;
in[2]=~S2;
in[3]=S3;
in[4]=S4;
in[5]=S5;
in[6]=~S6;
in[7]=~S7;
}


void bs_sbox(word_t *U)
{
    word_t S[8];
    word_t
        T1,T2,T3,T4,T5,T6,T7,T8,
        T9,T10,T11,T12,T13,T14,T15,T16,
        T17,T18,T19,T20,T21,T22,T23,T24,
        T25, T26, T27;

    word_t
        M1,M2,M3,M4,M5,M6,M7,M8,
        M9,M10,M11,M12,M13,M14,M15,
        M16,M17,M18,M19,M20,M21,M22,
        M23,M24,M25,M26,M27,M28,M29,
        M30,M31,M32,M33,M34,M35,M36,
        M37,M38,M39,M40,M41,M42,M43,
        M44,M45,M46,M47,M48,M49,M50,
        M51,M52,M53,M54,M55,M56,M57,
        M58,M59,M60,M61,M62,M63;

    word_t
        L0,L1,L2,L3,L4,L5,L6,L7,L8,
        L9,L10,L11,L12,L13,L14,
        L15,L16,L17,L18,L19,L20,
        L21,L22,L23,L24,L25,L26,
        L27,L28,L29;

    T1 = U[7] ^ U[4];
    T2 = U[7] ^ U[2];
    T3 = U[7] ^ U[1];
    T4 = U[4] ^ U[2];
    T5 = U[3] ^ U[1];
    T6 = T1 ^ T5;
    T7 = U[6] ^ U[5];
    T8 = U[0] ^ T6;
    T9 = U[0] ^ T7;
    T10 = T6 ^ T7;
    T11 = U[6] ^ U[2];
    T12 = U[5] ^ U[2];
    T13 = T3 ^ T4;
    T14 = T6 ^ T11;
    T15 = T5 ^ T11;
    T16 = T5 ^ T12;
    T17 = T9 ^ T16;
    T18 = U[4] ^ U[0];
    T19 = T7 ^ T18;
    T20 = T1 ^ T19;
    T21 = U[1] ^ U[0];
    T22 = T7 ^ T21;
    T23 = T2 ^ T22;
    T24 = T2 ^ T10;
    T25 = T20 ^ T17;
    T26 = T3 ^ T16;
    T27 = T1 ^ T12;
    M1 = T13 & T6;
    M2 = T23 & T8;
    M3 = T14 ^ M1;
    M4 = T19 & U[0];
    M5 = M4 ^ M1;
    M6 = T3 & T16;
    M7 = T22 & T9;
    M8 = T26 ^ M6;
    M9 = T20 & T17;
    M10 = M9 ^ M6;
    M11 = T1 & T15;
    M12 = T4 & T27;
    M13 = M12 ^ M11;
    M14 = T2 & T10;
    M15 = M14 ^ M11;
    M16 = M3 ^ M2;
    M17 = M5 ^ T24;
    M18 = M8 ^ M7;
    M19 = M10 ^ M15;
    M20 = M16 ^ M13;
    M21 = M17 ^ M15;
    M22 = M18 ^ M13;
    M23 = M19 ^ T25;
    M24 = M22 ^ M23;
    M25 = M22 & M20;
    M26 = M21 ^ M25;
    M27 = M20 ^ M21;
    M28 = M23 ^ M25;
    M29 = M28 & M27;
    M30 = M26 & M24;
    M31 = M20 & M23;
    M32 = M27 & M31;
    M33 = M27 ^ M25;
    M34 = M21 & M22;
    M35 = M24 & M34;
    M36 = M24 ^ M25;
    M37 = M21 ^ M29;
    M38 = M32 ^ M33;
    M39 = M23 ^ M30;
    M40 = M35 ^ M36;
    M41 = M38 ^ M40;
    M42 = M37 ^ M39;
    M43 = M37 ^ M38;
    M44 = M39 ^ M40;
    M45 = M42 ^ M41;
    M46 = M44 & T6;
    M47 = M40 & T8;
    M48 = M39 & U[0];
    M49 = M43 & T16;
    M50 = M38 & T9;
    M51 = M37 & T17;
    M52 = M42 & T15;
    M53 = M45 & T27;
    M54 = M41 & T10;
    M55 = M44 & T13;
    M56 = M40 & T23;
    M57 = M39 & T19;
    M58 = M43 & T3;
    M59 = M38 & T22;
    M60 = M37 & T20;
    M61 = M42 & T1;
    M62 = M45 & T4;
    M63 = M41 & T2;
    L0 = M61 ^ M62;
    L1 = M50 ^ M56;
    L2 = M46 ^ M48;
    L3 = M47 ^ M55;
    L4 = M54 ^ M58;
    L5 = M49 ^ M61;
    L6 = M62 ^ L5;
    L7 = M46 ^ L3;
    L8 = M51 ^ M59;
    L9 = M52 ^ M53;
    L10 = M53 ^ L4;
    L11 = M60 ^ L2;
    L12 = M48 ^ M51;
    L13 = M50 ^ L0;
    L14 = M52 ^ M61;
    L15 = M55 ^ L1;
    L16 = M56 ^ L0;
    L17 = M57 ^ L1;
    L18 = M58 ^ L8;
    L19 = M63 ^ L4;
    L20 = L0 ^ L1;
    L21 = L1 ^ L7;
    L22 = L3 ^ L12;
    L23 = L18 ^ L2;
    L24 = L15 ^ L9;
    L25 = L6 ^ L10;
    L26 = L7 ^ L9;
    L27 = L8 ^ L10;
    L28 = L11 ^ L14;
    L29 = L11 ^ L17;
    U[7] = L6 ^ L24;
    U[6] = ~(L16 ^ L26);
    U[5] = ~(L19 ^ L28);
    U[4] = L6 ^ L21;
    U[3] = L20 ^ L22;
    U[2] = L25 ^ L29;
     U[1] = ~(L13 ^ L27);//  <-- original
//    U[1] = (L13 ^ L27);
    U[0] = ~(L6 ^ L23);

    // memmove(U,S,sizeof(S));
}

void bs_transpose(word_t * blocks, word_t width_to_adjacent_block)
{
    word_t transpose[BLOCK_SIZE];
    memset(transpose, 0, sizeof(transpose));
    bs_transpose_dst(transpose,blocks);
    int sizeof_transpose = sizeof(transpose);

    // memmove(blocks,transpose,sizeof(transpose));

    // TODO : cleanup

    // word_t transpose_rev[BLOCK_SIZE];
    // // note to do rev transpose and make sure we get the same result
    // int sizeof_transpose_rev = sizeof(transpose_rev);

    // memset(transpose_rev, 0, sizeof(transpose_rev));
    // memcpy(transpose_rev, transpose, sizeof(transpose_rev));
    // if (sizeof_transpose != sizeof_transpose_rev){
    //     printf("\nERROOOOOOOOOOOOOOOOOR\n");
    //     printf("sizeof_transpose != sizeof_transpose_rev\n");
    // }
    
    // bs_transpose_rev(transpose_rev);

    // for (int i = 0; i < BLOCK_SIZE; i++){
    //     if (blocks[i] != transpose_rev[i]){
    //         printf("\nERROOOOOOOOOOOOOOOOOR\n");
    //         printf("transpose[%d] != transpose_rev[%d]\n", i, i);
    //     }
    // }

   memmove(blocks,transpose,sizeof(transpose));
//   memcpy(blocks,transpose,sizeof(transpose));

}


// since all the input is sequential we need to find the next block from the adjacent data block in the sequetial input. 
// for example if every data point is onnly one block deep. then width_to_adjacent_block = 1. if every data point is 2 blocks deep then width_to_adjacent_block = 2.
void bs_transpose_dst(word_t * transpose, word_t * blocks, word_t width_to_adjacent_block)
{
    word_t i,k;
    word_t w;
    for(k=0; k < WORD_SIZE; k++)
    {
        word_t bitpos = ONE << k;
        for (i=0; i < WORDS_PER_BLOCK; i++)
        {
            w = bs2le(blocks[k * WORDS_PER_BLOCK * width_to_adjacent_block + i]);
            word_t offset = i << MUL_SHIFT;

#ifndef UNROLL_TRANSPOSE
            word_t j;
            for(j=0; j < WORD_SIZE; j++)
            {
                // TODO make const time
                transpose[offset + j] |= (w & (ONE << j)) ? bitpos : 0;
            }
#else

            transpose[(offset)+ 0 ] |= (w & (ONE << 0 )) ? (bitpos) : 0;
            transpose[(offset)+ 1 ] |= (w & (ONE << 1 )) ? (bitpos) : 0;
            transpose[(offset)+ 2 ] |= (w & (ONE << 2 )) ? (bitpos) : 0;
            transpose[(offset)+ 3 ] |= (w & (ONE << 3 )) ? (bitpos) : 0;
            transpose[(offset)+ 4 ] |= (w & (ONE << 4 )) ? (bitpos) : 0;
            transpose[(offset)+ 5 ] |= (w & (ONE << 5 )) ? (bitpos) : 0;
            transpose[(offset)+ 6 ] |= (w & (ONE << 6 )) ? (bitpos) : 0;
            transpose[(offset)+ 7 ] |= (w & (ONE << 7 )) ? (bitpos) : 0;
#if WORD_SIZE > 8
            transpose[(offset)+ 8 ] |= (w & (ONE << 8 )) ? (bitpos) : 0;
            transpose[(offset)+ 9 ] |= (w & (ONE << 9 )) ? (bitpos) : 0;
            transpose[(offset)+ 10] |= (w & (ONE << 10)) ? (bitpos) : 0;
            transpose[(offset)+ 11] |= (w & (ONE << 11)) ? (bitpos) : 0;
            transpose[(offset)+ 12] |= (w & (ONE << 12)) ? (bitpos) : 0;
            transpose[(offset)+ 13] |= (w & (ONE << 13)) ? (bitpos) : 0;
            transpose[(offset)+ 14] |= (w & (ONE << 14)) ? (bitpos) : 0;
            transpose[(offset)+ 15] |= (w & (ONE << 15)) ? (bitpos) : 0;
#endif
#if WORD_SIZE > 16
            transpose[(offset)+ 16] |= (w & (ONE << 16)) ? (bitpos) : 0;
            transpose[(offset)+ 17] |= (w & (ONE << 17)) ? (bitpos) : 0;
            transpose[(offset)+ 18] |= (w & (ONE << 18)) ? (bitpos) : 0;
            transpose[(offset)+ 19] |= (w & (ONE << 19)) ? (bitpos) : 0;
            transpose[(offset)+ 20] |= (w & (ONE << 20)) ? (bitpos) : 0;
            transpose[(offset)+ 21] |= (w & (ONE << 21)) ? (bitpos) : 0;
            transpose[(offset)+ 22] |= (w & (ONE << 22)) ? (bitpos) : 0;
            transpose[(offset)+ 23] |= (w & (ONE << 23)) ? (bitpos) : 0;
            transpose[(offset)+ 24] |= (w & (ONE << 24)) ? (bitpos) : 0;
            transpose[(offset)+ 25] |= (w & (ONE << 25)) ? (bitpos) : 0;
            transpose[(offset)+ 26] |= (w & (ONE << 26)) ? (bitpos) : 0;
            transpose[(offset)+ 27] |= (w & (ONE << 27)) ? (bitpos) : 0;
            transpose[(offset)+ 28] |= (w & (ONE << 28)) ? (bitpos) : 0;
            transpose[(offset)+ 29] |= (w & (ONE << 29)) ? (bitpos) : 0;
            transpose[(offset)+ 30] |= (w & (ONE << 30)) ? (bitpos) : 0;
            transpose[(offset)+ 31] |= (w & (ONE << 31)) ? (bitpos) : 0;
#endif
#if WORD_SIZE > 32
            transpose[(offset)+ 32] |= (w & (ONE << 32)) ? (bitpos) : 0;
            transpose[(offset)+ 33] |= (w & (ONE << 33)) ? (bitpos) : 0;
            transpose[(offset)+ 34] |= (w & (ONE << 34)) ? (bitpos) : 0;
            transpose[(offset)+ 35] |= (w & (ONE << 35)) ? (bitpos) : 0;
            transpose[(offset)+ 36] |= (w & (ONE << 36)) ? (bitpos) : 0;
            transpose[(offset)+ 37] |= (w & (ONE << 37)) ? (bitpos) : 0;
            transpose[(offset)+ 38] |= (w & (ONE << 38)) ? (bitpos) : 0;
            transpose[(offset)+ 39] |= (w & (ONE << 39)) ? (bitpos) : 0;
            transpose[(offset)+ 40] |= (w & (ONE << 40)) ? (bitpos) : 0;
            transpose[(offset)+ 41] |= (w & (ONE << 41)) ? (bitpos) : 0;
            transpose[(offset)+ 42] |= (w & (ONE << 42)) ? (bitpos) : 0;
            transpose[(offset)+ 43] |= (w & (ONE << 43)) ? (bitpos) : 0;
            transpose[(offset)+ 44] |= (w & (ONE << 44)) ? (bitpos) : 0;
            transpose[(offset)+ 45] |= (w & (ONE << 45)) ? (bitpos) : 0;
            transpose[(offset)+ 46] |= (w & (ONE << 46)) ? (bitpos) : 0;
            transpose[(offset)+ 47] |= (w & (ONE << 47)) ? (bitpos) : 0;
            transpose[(offset)+ 48] |= (w & (ONE << 48)) ? (bitpos) : 0;
            transpose[(offset)+ 49] |= (w & (ONE << 49)) ? (bitpos) : 0;
            transpose[(offset)+ 50] |= (w & (ONE << 50)) ? (bitpos) : 0;
            transpose[(offset)+ 51] |= (w & (ONE << 51)) ? (bitpos) : 0;
            transpose[(offset)+ 52] |= (w & (ONE << 52)) ? (bitpos) : 0;
            transpose[(offset)+ 53] |= (w & (ONE << 53)) ? (bitpos) : 0;
            transpose[(offset)+ 54] |= (w & (ONE << 54)) ? (bitpos) : 0;
            transpose[(offset)+ 55] |= (w & (ONE << 55)) ? (bitpos) : 0;
            transpose[(offset)+ 56] |= (w & (ONE << 56)) ? (bitpos) : 0;
            transpose[(offset)+ 57] |= (w & (ONE << 57)) ? (bitpos) : 0;
            transpose[(offset)+ 58] |= (w & (ONE << 58)) ? (bitpos) : 0;
            transpose[(offset)+ 59] |= (w & (ONE << 59)) ? (bitpos) : 0;
            transpose[(offset)+ 60] |= (w & (ONE << 60)) ? (bitpos) : 0;
            transpose[(offset)+ 61] |= (w & (ONE << 61)) ? (bitpos) : 0;
            transpose[(offset)+ 62] |= (w & (ONE << 62)) ? (bitpos) : 0;
            transpose[(offset)+ 63] |= (w & (ONE << 63)) ? (bitpos) : 0;
#endif
#endif
                // constant time:
                //transpose[(i<<MUL_SHIFT)+ j] |= (((int64_t)((w & (ONE << j)) << (WORD_SIZE-1-j)))>>(WORD_SIZE-1)) & (ONE<<k);
        }
    }
}

// width_to_adjacent_block should be the same it was transposed with
void bs_transpose_rev(word_t * blocks, word_t width_to_adjacent_block)
{
    word_t i,k;
    word_t w;
    word_t transpose[BLOCK_SIZE];
    memset(transpose, 0, sizeof(transpose));
    for(k=0; k < BLOCK_SIZE; k++)
    {
        w = blocks[k];
        word_t bitpos = bs2be(ONE << (k % WORD_SIZE));
        word_t offset = k / WORD_SIZE;
#ifndef UNROLL_TRANSPOSE
        word_t j;
        for(j=0; j < WORD_SIZE; j++)
        {
            word_t bit = (w & (ONE << j)) ? (ONE << (k % WORD_SIZE)) : 0;
            transpose[j * WORDS_PER_BLOCK * width_to_adjacent_block + (offset)] |= bit;
        }
#else
        transpose[0  * WORDS_PER_BLOCK + (offset )] |= (w & (ONE << 0 )) ? bitpos : 0;
        transpose[1  * WORDS_PER_BLOCK + (offset )] |= (w & (ONE << 1 )) ? bitpos : 0;
        transpose[2  * WORDS_PER_BLOCK + (offset )] |= (w & (ONE << 2 )) ? bitpos : 0;
        transpose[3  * WORDS_PER_BLOCK + (offset )] |= (w & (ONE << 3 )) ? bitpos : 0;
        transpose[4  * WORDS_PER_BLOCK + (offset )] |= (w & (ONE << 4 )) ? bitpos : 0;
        transpose[5  * WORDS_PER_BLOCK + (offset )] |= (w & (ONE << 5 )) ? bitpos : 0;
        transpose[6  * WORDS_PER_BLOCK + (offset )] |= (w & (ONE << 6 )) ? bitpos : 0;
        transpose[7  * WORDS_PER_BLOCK + (offset )] |= (w & (ONE << 7 )) ? bitpos : 0;
#if WORD_SIZE > 8
        transpose[8  * WORDS_PER_BLOCK + (offset )] |= (w & (ONE << 8 )) ? bitpos : 0;
        transpose[9  * WORDS_PER_BLOCK + (offset )] |= (w & (ONE << 9 )) ? bitpos : 0;
        transpose[10 * WORDS_PER_BLOCK + (offset )] |= (w & (ONE << 10)) ? bitpos : 0;
        transpose[11 * WORDS_PER_BLOCK + (offset )] |= (w & (ONE << 11)) ? bitpos : 0;
        transpose[12 * WORDS_PER_BLOCK + (offset )] |= (w & (ONE << 12)) ? bitpos : 0;
        transpose[13 * WORDS_PER_BLOCK + (offset )] |= (w & (ONE << 13)) ? bitpos : 0;
        transpose[14 * WORDS_PER_BLOCK + (offset )] |= (w & (ONE << 14)) ? bitpos : 0;
        transpose[15 * WORDS_PER_BLOCK + (offset )] |= (w & (ONE << 15)) ? bitpos : 0;
#endif
#if WORD_SIZE > 16
        transpose[16 * WORDS_PER_BLOCK + (offset )] |= (w & (ONE << 16)) ? bitpos : 0;
        transpose[17 * WORDS_PER_BLOCK + (offset )] |= (w & (ONE << 17)) ? bitpos : 0;
        transpose[18 * WORDS_PER_BLOCK + (offset )] |= (w & (ONE << 18)) ? bitpos : 0;
        transpose[19 * WORDS_PER_BLOCK + (offset )] |= (w & (ONE << 19)) ? bitpos : 0;
        transpose[20 * WORDS_PER_BLOCK + (offset )] |= (w & (ONE << 20)) ? bitpos : 0;
        transpose[21 * WORDS_PER_BLOCK + (offset )] |= (w & (ONE << 21)) ? bitpos : 0;
        transpose[22 * WORDS_PER_BLOCK + (offset )] |= (w & (ONE << 22)) ? bitpos : 0;
        transpose[23 * WORDS_PER_BLOCK + (offset )] |= (w & (ONE << 23)) ? bitpos : 0;
        transpose[24 * WORDS_PER_BLOCK + (offset )] |= (w & (ONE << 24)) ? bitpos : 0;
        transpose[25 * WORDS_PER_BLOCK + (offset )] |= (w & (ONE << 25)) ? bitpos : 0;
        transpose[26 * WORDS_PER_BLOCK + (offset )] |= (w & (ONE << 26)) ? bitpos : 0;
        transpose[27 * WORDS_PER_BLOCK + (offset )] |= (w & (ONE << 27)) ? bitpos : 0;
        transpose[28 * WORDS_PER_BLOCK + (offset )] |= (w & (ONE << 28)) ? bitpos : 0;
        transpose[29 * WORDS_PER_BLOCK + (offset )] |= (w & (ONE << 29)) ? bitpos : 0;
        transpose[30 * WORDS_PER_BLOCK + (offset )] |= (w & (ONE << 30)) ? bitpos : 0;
        transpose[31 * WORDS_PER_BLOCK + (offset )] |= (w & (ONE << 31)) ? bitpos : 0;
#endif
#if WORD_SIZE > 32
        transpose[32 * WORDS_PER_BLOCK + (offset )] |= (w & (ONE << 32)) ? bitpos : 0;
        transpose[33 * WORDS_PER_BLOCK + (offset )] |= (w & (ONE << 33)) ? bitpos : 0;
        transpose[34 * WORDS_PER_BLOCK + (offset )] |= (w & (ONE << 34)) ? bitpos : 0;
        transpose[35 * WORDS_PER_BLOCK + (offset )] |= (w & (ONE << 35)) ? bitpos : 0;
        transpose[36 * WORDS_PER_BLOCK + (offset )] |= (w & (ONE << 36)) ? bitpos : 0;
        transpose[37 * WORDS_PER_BLOCK + (offset )] |= (w & (ONE << 37)) ? bitpos : 0;
        transpose[38 * WORDS_PER_BLOCK + (offset )] |= (w & (ONE << 38)) ? bitpos : 0;
        transpose[39 * WORDS_PER_BLOCK + (offset )] |= (w & (ONE << 39)) ? bitpos : 0;
        transpose[40 * WORDS_PER_BLOCK + (offset )] |= (w & (ONE << 40)) ? bitpos : 0;
        transpose[41 * WORDS_PER_BLOCK + (offset )] |= (w & (ONE << 41)) ? bitpos : 0;
        transpose[42 * WORDS_PER_BLOCK + (offset )] |= (w & (ONE << 42)) ? bitpos : 0;
        transpose[43 * WORDS_PER_BLOCK + (offset )] |= (w & (ONE << 43)) ? bitpos : 0;
        transpose[44 * WORDS_PER_BLOCK + (offset )] |= (w & (ONE << 44)) ? bitpos : 0;
        transpose[45 * WORDS_PER_BLOCK + (offset )] |= (w & (ONE << 45)) ? bitpos : 0;
        transpose[46 * WORDS_PER_BLOCK + (offset )] |= (w & (ONE << 46)) ? bitpos : 0;
        transpose[47 * WORDS_PER_BLOCK + (offset )] |= (w & (ONE << 47)) ? bitpos : 0;
        transpose[48 * WORDS_PER_BLOCK + (offset )] |= (w & (ONE << 48)) ? bitpos : 0;
        transpose[49 * WORDS_PER_BLOCK + (offset )] |= (w & (ONE << 49)) ? bitpos : 0;
        transpose[50 * WORDS_PER_BLOCK + (offset )] |= (w & (ONE << 50)) ? bitpos : 0;
        transpose[51 * WORDS_PER_BLOCK + (offset )] |= (w & (ONE << 51)) ? bitpos : 0;
        transpose[52 * WORDS_PER_BLOCK + (offset )] |= (w & (ONE << 52)) ? bitpos : 0;
        transpose[53 * WORDS_PER_BLOCK + (offset )] |= (w & (ONE << 53)) ? bitpos : 0;
        transpose[54 * WORDS_PER_BLOCK + (offset )] |= (w & (ONE << 54)) ? bitpos : 0;
        transpose[55 * WORDS_PER_BLOCK + (offset )] |= (w & (ONE << 55)) ? bitpos : 0;
        transpose[56 * WORDS_PER_BLOCK + (offset )] |= (w & (ONE << 56)) ? bitpos : 0;
        transpose[57 * WORDS_PER_BLOCK + (offset )] |= (w & (ONE << 57)) ? bitpos : 0;
        transpose[58 * WORDS_PER_BLOCK + (offset )] |= (w & (ONE << 58)) ? bitpos : 0;
        transpose[59 * WORDS_PER_BLOCK + (offset )] |= (w & (ONE << 59)) ? bitpos : 0;
        transpose[60 * WORDS_PER_BLOCK + (offset )] |= (w & (ONE << 60)) ? bitpos : 0;
        transpose[61 * WORDS_PER_BLOCK + (offset )] |= (w & (ONE << 61)) ? bitpos : 0;
        transpose[62 * WORDS_PER_BLOCK + (offset )] |= (w & (ONE << 62)) ? bitpos : 0;
        transpose[63 * WORDS_PER_BLOCK + (offset )] |= (w & (ONE << 63)) ? bitpos : 0;
#endif
#endif
    }
//    memmove(blocks,transpose,sizeof(transpose));
    memcpy(blocks,transpose,sizeof(transpose));
}


#define R0          0
#define R1          8
#define R2          16
#define R3          24

#define B0         0
#define B1         64
#define B2         128
#define B3         192
#define B4         256
#define B5         320
#define B6         384
#define B7         448


#define R0_shift        (BLOCK_SIZE/4)*0
#define R1_shift        (BLOCK_SIZE/4)*1
#define R2_shift        (BLOCK_SIZE/4)*2
#define R3_shift        (BLOCK_SIZE/4)*3
#define B_MOD           (BLOCK_SIZE)

/// @brief FOR Q ONLY
/// @param B 
void bs_shiftrows_q(word_t * B)
{
    // think of Br* as columns
    word_t Bp_space[BLOCK_SIZE];
    word_t * Bp = Bp_space;

    word_t * Br0;
    word_t * Br1;    
    word_t * Br2;
    word_t * Br3;
    word_t * Br4;
    word_t * Br5;
    word_t * Br6;
    word_t * Br7;

    uint32_t offsetr0 = 64;
    uint32_t offsetr1 = 192;
    uint32_t offsetr2 = 320;
    uint32_t offsetr3 = 448;
    uint32_t offsetr4 = 0;
    uint32_t offsetr5 = 128;
    uint32_t offsetr6 = 256;
    uint32_t offsetr7 = 384;

    Br0 = B + offsetr0;
    Br1 = B + offsetr1;
    Br2 = B + offsetr2;
    Br3 = B + offsetr3;
    Br4 = B + offsetr4;
    Br5 = B + offsetr5;
    Br6 = B + offsetr6;
    Br7 = B + offsetr7;

    int i;
    for(i=0; i<8; i++)
    {
        if (i == 0){
            Br0 = B + 64;
            Br1 = B + 128;
            Br2 = B + 192;
            Br3 = B + 256;
            Br4 = B + 320;
            Br5 = B + 384;
            Br6 = B + 448;
            Br7 = B + 0;
        }
        else if (i == 1){
            Br0 = B + 200;
            Br1 = B + 264;
            Br2 = B + 328;
            Br3 = B + 392;
            Br4 = B + 456;
            Br5 = B + 8;
            Br6 = B + 72;
            Br7 = B + 136;
        }
        else if (i == 2){
            Br0 = B + 336;
            Br1 = B + 400;
            Br2 = B + 464;
            Br3 = B + 16;
            Br4 = B + 80;
            Br5 = B + 144;
            Br6 = B + 208;
            Br7 = B + 272;
        }
        else if (i == 3){
            Br0 = B + 472;
            Br1 = B + 24;
            Br2 = B + 88;
            Br3 = B + 152;
            Br4 = B + 216;
            Br5 = B + 280;
            Br6 = B + 344;
            Br7 = B + 408;
        }
        else if (i == 4){
            Br0 = B + 32;
            Br1 = B + 96;
            Br2 = B + 160;
            Br3 = B + 224;
            Br4 = B + 288;
            Br5 = B + 352;
            Br6 = B + 416;
            Br7 = B + 480;
        }
        else if (i == 5){
            Br0 = B + 168;
            Br1 = B + 232;
            Br2 = B + 296;
            Br3 = B + 360;
            Br4 = B + 424;
            Br5 = B + 488;
            Br6 = B + 40;
            Br7 = B + 104;
        }
        else if (i == 6){
            Br0 = B + 304;
            Br1 = B + 368;
            Br2 = B + 432;
            Br3 = B + 496;
            Br4 = B + 48;
            Br5 = B + 112;
            Br6 = B + 176;
            Br7 = B + 240;
        }
        else if (i == 7){
            Br0 = B + 440;
            Br1 = B + 504;
            Br2 = B + 56;
            Br3 = B + 120;
            Br4 = B + 184;
            Br5 = B + 248;
            Br6 = B + 312;
            Br7 = B + 376;
        }
        Bp[B0 + 0] = Br0[0];
        Bp[B0 + 1] = Br0[1];
        Bp[B0 + 2] = Br0[2];
        Bp[B0 + 3] = Br0[3];
        Bp[B0 + 4] = Br0[4];
        Bp[B0 + 5] = Br0[5];
        Bp[B0 + 6] = Br0[6];
        Bp[B0 + 7] = Br0[7];
        
        Bp[B1 + 0] = Br1[0];
        Bp[B1 + 1] = Br1[1];
        Bp[B1 + 2] = Br1[2];
        Bp[B1 + 3] = Br1[3];
        Bp[B1 + 4] = Br1[4];
        Bp[B1 + 5] = Br1[5];
        Bp[B1 + 6] = Br1[6];
        Bp[B1 + 7] = Br1[7];
        
        Bp[B2 + 0] = Br2[0];
        Bp[B2 + 1] = Br2[1];
        Bp[B2 + 2] = Br2[2];
        Bp[B2 + 3] = Br2[3];
        Bp[B2 + 4] = Br2[4];
        Bp[B2 + 5] = Br2[5];
        Bp[B2 + 6] = Br2[6];
        Bp[B2 + 7] = Br2[7];

        Bp[B3 + 0] = Br3[0];
        Bp[B3 + 1] = Br3[1];
        Bp[B3 + 2] = Br3[2];
        Bp[B3 + 3] = Br3[3];
        Bp[B3 + 4] = Br3[4];
        Bp[B3 + 5] = Br3[5];
        Bp[B3 + 6] = Br3[6];
        Bp[B3 + 7] = Br3[7];

        Bp[B4 + 0] = Br4[0];
        Bp[B4 + 1] = Br4[1];
        Bp[B4 + 2] = Br4[2];
        Bp[B4 + 3] = Br4[3];
        Bp[B4 + 4] = Br4[4];
        Bp[B4 + 5] = Br4[5];
        Bp[B4 + 6] = Br4[6];
        Bp[B4 + 7] = Br4[7];

        Bp[B5 + 0] = Br5[0];
        Bp[B5 + 1] = Br5[1];
        Bp[B5 + 2] = Br5[2];
        Bp[B5 + 3] = Br5[3];
        Bp[B5 + 4] = Br5[4];
        Bp[B5 + 5] = Br5[5];
        Bp[B5 + 6] = Br5[6];
        Bp[B5 + 7] = Br5[7];

        Bp[B6 + 0] = Br6[0];
        Bp[B6 + 1] = Br6[1];
        Bp[B6 + 2] = Br6[2];
        Bp[B6 + 3] = Br6[3];
        Bp[B6 + 4] = Br6[4];
        Bp[B6 + 5] = Br6[5];
        Bp[B6 + 6] = Br6[6];
        Bp[B6 + 7] = Br6[7];

        Bp[B7 + 0] = Br7[0];
        Bp[B7 + 1] = Br7[1];
        Bp[B7 + 2] = Br7[2];
        Bp[B7 + 3] = Br7[3];
        Bp[B7 + 4] = Br7[4];
        Bp[B7 + 5] = Br7[5];
        Bp[B7 + 6] = Br7[6];
        Bp[B7 + 7] = Br7[7];

        // 64 cus thats the number of elements in the matrix. 
        offsetr0 = (offsetr0 + BLOCK_SIZE/64 + BLOCK_SIZE/8) & 0x1FF;
        offsetr1 = (offsetr1 + BLOCK_SIZE/64 + BLOCK_SIZE/8) & 0x1FF;
        offsetr2 = (offsetr2 + BLOCK_SIZE/64 + BLOCK_SIZE/8) & 0x1FF;
        offsetr3 = (offsetr3 + BLOCK_SIZE/64 + BLOCK_SIZE/8) & 0x1FF;
        offsetr4 = (offsetr4 + BLOCK_SIZE/64 + BLOCK_SIZE/8) & 0x1FF;
        offsetr5 = (offsetr5 + BLOCK_SIZE/64 + BLOCK_SIZE/8) & 0x1FF;
        offsetr6 = (offsetr6 + BLOCK_SIZE/64 + BLOCK_SIZE/8) & 0x1FF;
        offsetr7 = (offsetr7 + BLOCK_SIZE/64 + BLOCK_SIZE/8) & 0x1FF;

        Br0 = B + offsetr0;
        Br1 = B + offsetr1;
        Br2 = B + offsetr2;
        Br3 = B + offsetr3;
        Br4 = B + offsetr4;
        Br5 = B + offsetr5;
        Br6 = B + offsetr6;
        Br7 = B + offsetr7;

        Bp += 8;
    }
    memmove(B,Bp_space,sizeof(Bp_space));
}

/// @brief FOR P PERMUTATION ONLY
/// @param B 
void bs_shiftrows_p(word_t * B)
{
    // think of Br* as columns
    word_t Bp_space[BLOCK_SIZE];
    word_t * Bp = Bp_space;
    word_t * Br0 = B + 0;
    word_t * Br1 = B + 64;
    word_t * Br2 = B + 128;
    word_t * Br3 = B + 192;
    word_t * Br4 = B + 256;
    word_t * Br5 = B + 320;
    word_t * Br6 = B + 384;
    word_t * Br7 = B + 448;

    uint32_t offsetr0 = 0;
    uint32_t offsetr1 = 64;
    uint32_t offsetr2 = 128;
    uint32_t offsetr3 = 192;
    uint32_t offsetr4 = 256;
    uint32_t offsetr5 = 320;
    uint32_t offsetr6 = 384;
    uint32_t offsetr7 = 448;


    Br0 = B + offsetr0;
    Br1 = B + offsetr1;
    Br2 = B + offsetr2;
    Br3 = B + offsetr3;
    Br4 = B + offsetr4;
    Br5 = B + offsetr5;
    Br6 = B + offsetr6;
    Br7 = B + offsetr7;



    int i;
    for(i=0; i<8; i++)
    {
        Bp[B0 + 0] = Br0[0];
        Bp[B0 + 1] = Br0[1];
        Bp[B0 + 2] = Br0[2];
        Bp[B0 + 3] = Br0[3];
        Bp[B0 + 4] = Br0[4];
        Bp[B0 + 5] = Br0[5];
        Bp[B0 + 6] = Br0[6];
        Bp[B0 + 7] = Br0[7];
        
        Bp[B1 + 0] = Br1[0];
        Bp[B1 + 1] = Br1[1];
        Bp[B1 + 2] = Br1[2];
        Bp[B1 + 3] = Br1[3];
        Bp[B1 + 4] = Br1[4];
        Bp[B1 + 5] = Br1[5];
        Bp[B1 + 6] = Br1[6];
        Bp[B1 + 7] = Br1[7];
        
        Bp[B2 + 0] = Br2[0];
        Bp[B2 + 1] = Br2[1];
        Bp[B2 + 2] = Br2[2];
        Bp[B2 + 3] = Br2[3];
        Bp[B2 + 4] = Br2[4];
        Bp[B2 + 5] = Br2[5];
        Bp[B2 + 6] = Br2[6];
        Bp[B2 + 7] = Br2[7];

        Bp[B3 + 0] = Br3[0];
        Bp[B3 + 1] = Br3[1];
        Bp[B3 + 2] = Br3[2];
        Bp[B3 + 3] = Br3[3];
        Bp[B3 + 4] = Br3[4];
        Bp[B3 + 5] = Br3[5];
        Bp[B3 + 6] = Br3[6];
        Bp[B3 + 7] = Br3[7];

        Bp[B4 + 0] = Br4[0];
        Bp[B4 + 1] = Br4[1];
        Bp[B4 + 2] = Br4[2];
        Bp[B4 + 3] = Br4[3];
        Bp[B4 + 4] = Br4[4];
        Bp[B4 + 5] = Br4[5];
        Bp[B4 + 6] = Br4[6];
        Bp[B4 + 7] = Br4[7];

        Bp[B5 + 0] = Br5[0];
        Bp[B5 + 1] = Br5[1];
        Bp[B5 + 2] = Br5[2];
        Bp[B5 + 3] = Br5[3];
        Bp[B5 + 4] = Br5[4];
        Bp[B5 + 5] = Br5[5];
        Bp[B5 + 6] = Br5[6];
        Bp[B5 + 7] = Br5[7];

        Bp[B6 + 0] = Br6[0];
        Bp[B6 + 1] = Br6[1];
        Bp[B6 + 2] = Br6[2];
        Bp[B6 + 3] = Br6[3];
        Bp[B6 + 4] = Br6[4];
        Bp[B6 + 5] = Br6[5];
        Bp[B6 + 6] = Br6[6];
        Bp[B6 + 7] = Br6[7];

        Bp[B7 + 0] = Br7[0];
        Bp[B7 + 1] = Br7[1];
        Bp[B7 + 2] = Br7[2];
        Bp[B7 + 3] = Br7[3];
        Bp[B7 + 4] = Br7[4];
        Bp[B7 + 5] = Br7[5];
        Bp[B7 + 6] = Br7[6];
        Bp[B7 + 7] = Br7[7];

        // 64 cus thats the number of elements in the matrix. 
        offsetr0 = (offsetr0 + BLOCK_SIZE/64 + BLOCK_SIZE/8) & 0x1FF;
        offsetr1 = (offsetr1 + BLOCK_SIZE/64 + BLOCK_SIZE/8) & 0x1FF;
        offsetr2 = (offsetr2 + BLOCK_SIZE/64 + BLOCK_SIZE/8) & 0x1FF;
        offsetr3 = (offsetr3 + BLOCK_SIZE/64 + BLOCK_SIZE/8) & 0x1FF;
        offsetr4 = (offsetr4 + BLOCK_SIZE/64 + BLOCK_SIZE/8) & 0x1FF;
        offsetr5 = (offsetr5 + BLOCK_SIZE/64 + BLOCK_SIZE/8) & 0x1FF;
        offsetr6 = (offsetr6 + BLOCK_SIZE/64 + BLOCK_SIZE/8) & 0x1FF;
        offsetr7 = (offsetr7 + BLOCK_SIZE/64 + BLOCK_SIZE/8) & 0x1FF;

        Br0 = B + offsetr0;
        Br1 = B + offsetr1;
        Br2 = B + offsetr2;
        Br3 = B + offsetr3;
        Br4 = B + offsetr4;
        Br5 = B + offsetr5;
        Br6 = B + offsetr6;
        Br7 = B + offsetr7;

        Bp += 8;
    }
    memmove(B,Bp_space,sizeof(Bp_space));
}


void bs_shiftrows_rev_p(word_t * B)
{
    word_t Bp_space[BLOCK_SIZE];
    word_t * Bp = Bp_space;
    word_t * Br0 = Bp + 0;
    word_t * Br1 = Bp + 32;
    word_t * Br2 = Bp + 64;
    word_t * Br3 = Bp + 96;
    uint8_t offsetr0 = 0;
    uint8_t offsetr1 = 32;
    uint8_t offsetr2 = 64;
    uint8_t offsetr3 = 96;


    int i;
    for(i=0; i<4; i++)
    {
        Br0[0] = B[B0 + 0];
        Br0[1] = B[B0 + 1];
        Br0[2] = B[B0 + 2];
        Br0[3] = B[B0 + 3];
        Br0[4] = B[B0 + 4];
        Br0[5] = B[B0 + 5];
        Br0[6] = B[B0 + 6];
        Br0[7] = B[B0 + 7];
        Br1[0] = B[B1 + 0];
        Br1[1] = B[B1 + 1];
        Br1[2] = B[B1 + 2];
        Br1[3] = B[B1 + 3];
        Br1[4] = B[B1 + 4];
        Br1[5] = B[B1 + 5];
        Br1[6] = B[B1 + 6];
        Br1[7] = B[B1 + 7];
        Br2[0] = B[B2 + 0];
        Br2[1] = B[B2 + 1];
        Br2[2] = B[B2 + 2];
        Br2[3] = B[B2 + 3];
        Br2[4] = B[B2 + 4];
        Br2[5] = B[B2 + 5];
        Br2[6] = B[B2 + 6];
        Br2[7] = B[B2 + 7];
        Br3[0] = B[B3 + 0];
        Br3[1] = B[B3 + 1];
        Br3[2] = B[B3 + 2];
        Br3[3] = B[B3 + 3];
        Br3[4] = B[B3 + 4];
        Br3[5] = B[B3 + 5];
        Br3[6] = B[B3 + 6];
        Br3[7] = B[B3 + 7];

        offsetr0 = (offsetr0 + BLOCK_SIZE/16 + BLOCK_SIZE/4) & 0x7f;
        offsetr1 = (offsetr1 + BLOCK_SIZE/16 + BLOCK_SIZE/4) & 0x7f;
        offsetr2 = (offsetr2 + BLOCK_SIZE/16 + BLOCK_SIZE/4) & 0x7f;
        offsetr3 = (offsetr3 + BLOCK_SIZE/16 + BLOCK_SIZE/4) & 0x7f;

        Br0 = Bp + offsetr0;
        Br1 = Bp + offsetr1;
        Br2 = Bp + offsetr2;
        Br3 = Bp + offsetr3;

        B += 8;
    }
    memmove(B - 8 * 4,Bp_space,sizeof(Bp_space));
}


#define A0  0
#define A1  8
#define A2  16
#define A3  24
#define A4  32
#define A5  40
#define A6  48
#define A7  56

// Does shift rows and mix columns in same step
void bs_shiftmix(word_t * B)
{
    word_t Bp_space[BLOCK_SIZE];
    word_t * Bp = Bp_space;

    word_t * Br0 = B + 0;
    word_t * Br1 = B + 32;
    word_t * Br2 = B + 64;
    word_t * Br3 = B + 96;

    uint8_t offsetr0 = 0;
    uint8_t offsetr1 = 32;
    uint8_t offsetr2 = 64;
    uint8_t offsetr3 = 96;

        Br0 = B + offsetr0;
        Br1 = B + offsetr1;
        Br2 = B + offsetr2;
        Br3 = B + offsetr3;


    int i;
    for (i = 0; i < 4; i++)
    {
        // B0
        //            2*A0        2*A1              A1           A2           A3
        word_t of =Br0[R0+7]^ Br1[R1+7];
        Bp[A0+0] =                         Br1[R1+0] ^ Br2[R2+0] ^ Br3[R3+0] ^ of;
        Bp[A0+1] = Br0[R0+0] ^ Br1[R1+0] ^ Br1[R1+1] ^ Br2[R2+1] ^ Br3[R3+1] ^ of;
        Bp[A0+2] = Br0[R0+1] ^ Br1[R1+1] ^ Br1[R1+2] ^ Br2[R2+2] ^ Br3[R3+2];
        Bp[A0+3] = Br0[R0+2] ^ Br1[R1+2] ^ Br1[R1+3] ^ Br2[R2+3] ^ Br3[R3+3] ^ of;
        Bp[A0+4] = Br0[R0+3] ^ Br1[R1+3] ^ Br1[R1+4] ^ Br2[R2+4] ^ Br3[R3+4] ^ of;
        Bp[A0+5] = Br0[R0+4] ^ Br1[R1+4] ^ Br1[R1+5] ^ Br2[R2+5] ^ Br3[R3+5];
        Bp[A0+6] = Br0[R0+5] ^ Br1[R1+5] ^ Br1[R1+6] ^ Br2[R2+6] ^ Br3[R3+6];
        Bp[A0+7] = Br0[R0+6] ^ Br1[R1+6] ^ Br1[R1+7] ^ Br2[R2+7] ^ Br3[R3+7];

        //            A0            2*A1        2*A2        A2       A3
        of = Br1[R1+7] ^ Br2[R2+7];
        Bp[A1+0] = Br0[R0+0]                         ^ Br2[R2+0] ^ Br3[R3+0] ^ of;
        Bp[A1+1] = Br0[R0+1] ^ Br1[R1+0] ^ Br2[R2+0] ^ Br2[R2+1] ^ Br3[R3+1] ^ of;
        Bp[A1+2] = Br0[R0+2] ^ Br1[R1+1] ^ Br2[R2+1] ^ Br2[R2+2] ^ Br3[R3+2];
        Bp[A1+3] = Br0[R0+3] ^ Br1[R1+2] ^ Br2[R2+2] ^ Br2[R2+3] ^ Br3[R3+3] ^ of;
        Bp[A1+4] = Br0[R0+4] ^ Br1[R1+3] ^ Br2[R2+3] ^ Br2[R2+4] ^ Br3[R3+4] ^ of;
        Bp[A1+5] = Br0[R0+5] ^ Br1[R1+4] ^ Br2[R2+4] ^ Br2[R2+5] ^ Br3[R3+5];
        Bp[A1+6] = Br0[R0+6] ^ Br1[R1+5] ^ Br2[R2+5] ^ Br2[R2+6] ^ Br3[R3+6];
        Bp[A1+7] = Br0[R0+7] ^ Br1[R1+6] ^ Br2[R2+6] ^ Br2[R2+7] ^ Br3[R3+7];

        //            A0             A1      2*A2        2*A3         A3
        of = Br2[R2+7] ^ Br3[R3+7];
        Bp[A2+0] = Br0[R0+0] ^ Br1[R1+0]                         ^ Br3[R3+0] ^ of;
        Bp[A2+1] = Br0[R0+1] ^ Br1[R1+1] ^ Br2[R2+0] ^ Br3[R3+0] ^ Br3[R3+1] ^ of;
        Bp[A2+2] = Br0[R0+2] ^ Br1[R1+2] ^ Br2[R2+1] ^ Br3[R3+1] ^ Br3[R3+2];
        Bp[A2+3] = Br0[R0+3] ^ Br1[R1+3] ^ Br2[R2+2] ^ Br3[R3+2] ^ Br3[R3+3] ^ of;
        Bp[A2+4] = Br0[R0+4] ^ Br1[R1+4] ^ Br2[R2+3] ^ Br3[R3+3] ^ Br3[R3+4] ^ of;
        Bp[A2+5] = Br0[R0+5] ^ Br1[R1+5] ^ Br2[R2+4] ^ Br3[R3+4] ^ Br3[R3+5];
        Bp[A2+6] = Br0[R0+6] ^ Br1[R1+6] ^ Br2[R2+5] ^ Br3[R3+5] ^ Br3[R3+6];
        Bp[A2+7] = Br0[R0+7] ^ Br1[R1+7] ^ Br2[R2+6] ^ Br3[R3+6] ^ Br3[R3+7];

        //             A0          2*A0           A1       A2      2*A3
        of = Br0[R0+7] ^ Br3[R3+7];
        Bp[A3+0] = Br0[R0+0] ^             Br1[R1+0] ^ Br2[R2+0]             ^ of;
        Bp[A3+1] = Br0[R0+1] ^ Br0[R0+0] ^ Br1[R1+1] ^ Br2[R2+1] ^ Br3[R3+0] ^ of;
        Bp[A3+2] = Br0[R0+2] ^ Br0[R0+1] ^ Br1[R1+2] ^ Br2[R2+2] ^ Br3[R3+1];
        Bp[A3+3] = Br0[R0+3] ^ Br0[R0+2] ^ Br1[R1+3] ^ Br2[R2+3] ^ Br3[R3+2] ^ of;
        Bp[A3+4] = Br0[R0+4] ^ Br0[R0+3] ^ Br1[R1+4] ^ Br2[R2+4] ^ Br3[R3+3] ^ of;
        Bp[A3+5] = Br0[R0+5] ^ Br0[R0+4] ^ Br1[R1+5] ^ Br2[R2+5] ^ Br3[R3+4];
        Bp[A3+6] = Br0[R0+6] ^ Br0[R0+5] ^ Br1[R1+6] ^ Br2[R2+6] ^ Br3[R3+5];
        Bp[A3+7] = Br0[R0+7] ^ Br0[R0+6] ^ Br1[R1+7] ^ Br2[R2+7] ^ Br3[R3+6];

        Bp += BLOCK_SIZE/4;

        offsetr0 = (offsetr0 + BLOCK_SIZE/4) & 0x7f;
        offsetr1 = (offsetr1 + BLOCK_SIZE/4) & 0x7f;
        offsetr2 = (offsetr2 + BLOCK_SIZE/4) & 0x7f;
        offsetr3 = (offsetr3 + BLOCK_SIZE/4) & 0x7f;

        Br0 = B + offsetr0;
        Br1 = B + offsetr1;
        Br2 = B + offsetr2;
        Br3 = B + offsetr3;
    }

    memmove(B,Bp_space,sizeof(Bp_space));
}

void bs_gf_multiply(word_t * B, word_t * A, int C)
{
    // B is word_t[8] as input. 
    // A is word_t[8] for the result
    // C is the constant multiplier


    // lets create a local copy of B
    word_t B_space[8];
    B_space[0] = B[0];
    B_space[1] = B[1];
    B_space[2] = B[2];
    B_space[3] = B[3];
    B_space[4] = B[4];
    B_space[5] = B[5];
    B_space[6] = B[6];
    B_space[7] = B[7];

    int i;
    for(i=0; i<8; i++)
    {
        A[i] = 0;
    }
    int b_index = 0;
    int j = 0;
    for(i=0; i<8; i++)
    {
        if(C & 1)
        {
            // for (j = i, b_index = 0; j < 8; j++, b_index++)
            // {
            //    A[j] ^= B[b_index];
            // }
            for (j = 0; j < 8; j++)
            {
                A[j] ^= B_space[j];
            }
        }
        else if (C == 0){
            break;
        }
        C >>= 1;

        // shift B_space to the left by 1
        word_t MSB = B_space[7];
        for (j = 7; j > 0; j--)
        {
            B_space[j] = B_space[j -1 ];
        }
        B_space[0] = 0;
        
        // if MSB is 1, then we need to XOR with 0x1b
        B_space[0] ^= MSB;
        B_space[1] ^= MSB;
        B_space[3] ^= MSB;
        B_space[4] ^= MSB;

        //B += WORD_SIZE;
    }
}


// A has the result
void bs_gf_add(word_t * A, word_t * B)
{
    int i; 
    for(i=0; i<8; i++)
    {
        A[i] ^= B[i];
    }
}
void print_word_t_var(word_t var[8]) {
    printf("\n");
    for(int i = 0; i < 8; i++) {
        printf("%lu ", var[i]);
    }
    printf("\n");
}

void bs_mixbytes(word_t * B)
{
    word_t Bp_space[BLOCK_SIZE];
    memset(Bp_space, 0, sizeof(Bp_space));
    word_t * Bp = Bp_space;
    word_t tmpProducts[8];

    word_t oldTmpProducts[8];
    // to understand this, see
    // https://en.wikipedia.org/wiki/Rijndael_mix_columns
    
    int i = 0;
    for (; i < 8; i++)
    {
    // NEW_S00 = 2S00 + 2S01 + 3S02 + 4SO3 + 5S04 + 3S05 + 5S06 + 7S07
    // NEW_S01 = 7S00 + 2S01 + 2S02 + 3SO3 + 4S04 + 5S05 + 3S06 + 5S07
    // NEW_S02 = 5S00 + 7S01 + 2S02 + 2SO3 + 3S04 + 4S05 + 5S06 + 3S07
    // NEW_S03 = 3S00 + 5S01 + 7S02 + 2SO3 + 2S04 + 3S05 + 4S06 + 5S07
    // NEW_S04 = 5S00 + 3S01 + 5S02 + 7SO3 + 2S04 + 2S05 + 3S06 + 4S07
    // NEW_S05 = 4S00 + 5S01 + 3S02 + 5SO3 + 7S04 + 2S05 + 2S06 + 3S07
    // NEW_S06 = 3S00 + 4S01 + 5S02 + 3SO3 + 5S04 + 7S05 + 2S06 + 2S07
    // NEW_S07 = 2S00 + 3S01 + 4S02 + 5SO3 + 3S04 + 5S05 + 7S06 + 2S07

    //  of = A0 ^ A1;
    //  A0 = A0 ^ (0x1b & ((signed char)of>>7));

    //// 2 * A0
    //  A0 = A0 ^ (A0 << 1)

    //// + 3 * A1
    //  A0 = A0 ^ (A1)
    //  A0 = A0 ^ (A1<<1)

    //// + A2 + A3
    //  A0 = A0 ^ (A2)
    //  A0 = A0 ^ (A3)
    //          A0.7    A1.7

    // NEW_S00 = 2S00 + 2S01 + 3S02 + 4SO3 + 5S04 + 3S05 + 5S06 + 7S07

    // word_t of = B[A0+7] ^ B[A1+7] ^ B[A2+7];
    // 2S00

    bs_gf_multiply(&B[A0], tmpProducts, 2);
    bs_gf_add(&Bp[A0], tmpProducts);

    // 2S01
    bs_gf_multiply(&B[A1], tmpProducts, 2);
    bs_gf_add(&Bp[A0], tmpProducts);


    // 3S02
    bs_gf_multiply(&B[A2], tmpProducts, 3);
    bs_gf_add(&Bp[A0], tmpProducts);

    // 4S03
    bs_gf_multiply(&B[A3], tmpProducts, 4);
    bs_gf_add(&Bp[A0], tmpProducts);

    // 5S04
    bs_gf_multiply(&B[A4], tmpProducts, 5);
    bs_gf_add(&Bp[A0], tmpProducts);

    // 3S05
    bs_gf_multiply(&B[A5], tmpProducts, 3);
    bs_gf_add(&Bp[A0], tmpProducts);

    // 5S06
    bs_gf_multiply(&B[A6], tmpProducts, 5);
    bs_gf_add(&Bp[A0], tmpProducts);

    // 7S07
    bs_gf_multiply(&B[A7], tmpProducts, 7);
    bs_gf_add(&Bp[A0], tmpProducts);

    // NEW_S01 = 7S00 + 2S01 + 2S02 + 3S03 + 4S04 + 5S05 + 3S06 + 5S07

    // 7S00
    bs_gf_multiply(&B[A0], tmpProducts, 7);
    bs_gf_add(&Bp[A1], tmpProducts);

    // 2S01
    bs_gf_multiply(&B[A1], tmpProducts, 2);
    bs_gf_add(&Bp[A1], tmpProducts);

    // 2S02
    bs_gf_multiply(&B[A2], tmpProducts, 2);
    bs_gf_add(&Bp[A1], tmpProducts);

    // 3S03
    bs_gf_multiply(&B[A3], tmpProducts, 3);
    bs_gf_add(&Bp[A1], tmpProducts);

    // 4S04
    bs_gf_multiply(&B[A4], tmpProducts, 4);
    bs_gf_add(&Bp[A1], tmpProducts);

    // 5S05
    bs_gf_multiply(&B[A5], tmpProducts, 5);
    bs_gf_add(&Bp[A1], tmpProducts);

    // 3S06
    bs_gf_multiply(&B[A6], tmpProducts, 3);
    bs_gf_add(&Bp[A1], tmpProducts);

    // 5S07
    bs_gf_multiply(&B[A7], tmpProducts, 5);
    bs_gf_add(&Bp[A1], tmpProducts);

    // NEW_S02 = 5S00 + 7S01 + 2S02 + 2S03 + 3S04 + 4S05 + 5S06 + 3S07

    // 5S00
    bs_gf_multiply(&B[A0], tmpProducts, 5);
    bs_gf_add(&Bp[A2], tmpProducts);

    // 7S01
    bs_gf_multiply(&B[A1], tmpProducts, 7);
    bs_gf_add(&Bp[A2], tmpProducts);

    // 2S02
    bs_gf_multiply(&B[A2], tmpProducts, 2);
    bs_gf_add(&Bp[A2], tmpProducts);

    // 2S03
    bs_gf_multiply(&B[A3], tmpProducts, 2);
    bs_gf_add(&Bp[A2], tmpProducts);

    // 3S04
    bs_gf_multiply(&B[A4], tmpProducts, 3);
    bs_gf_add(&Bp[A2], tmpProducts);

    // 4S05
    bs_gf_multiply(&B[A5], tmpProducts, 4);
    bs_gf_add(&Bp[A2], tmpProducts);

    // 5S06
    bs_gf_multiply(&B[A6], tmpProducts, 5);
    bs_gf_add(&Bp[A2], tmpProducts);

    // 3S07
    bs_gf_multiply(&B[A7], tmpProducts, 3);
    bs_gf_add(&Bp[A2], tmpProducts);

    // NEW_S03 = 3S00 + 5S01 + 7S02 + 2S03 + 2S04 + 3S05 + 4S06 + 5S07

    // 3S00
    bs_gf_multiply(&B[A0], tmpProducts, 3);
    bs_gf_add(&Bp[A3], tmpProducts);

    // 5S01
    bs_gf_multiply(&B[A1], tmpProducts, 5);
    bs_gf_add(&Bp[A3], tmpProducts);

    // 7S02
    bs_gf_multiply(&B[A2], tmpProducts, 7);
    bs_gf_add(&Bp[A3], tmpProducts);

    // 2S03
    bs_gf_multiply(&B[A3], tmpProducts, 2);
    bs_gf_add(&Bp[A3], tmpProducts);

    // 2S04
    bs_gf_multiply(&B[A4], tmpProducts, 2);
    bs_gf_add(&Bp[A3], tmpProducts);

    // 3S05
    bs_gf_multiply(&B[A5], tmpProducts, 3);
    bs_gf_add(&Bp[A3], tmpProducts);

    // 4S06
    bs_gf_multiply(&B[A6], tmpProducts, 4);
    bs_gf_add(&Bp[A3], tmpProducts);

    // 5S07
    bs_gf_multiply(&B[A7], tmpProducts, 5);
    bs_gf_add(&Bp[A3], tmpProducts);

    // NEW_S04 = 5S00 + 3S01 + 5S02 + 7S03 + 2S04 + 2S05 + 3S06 + 4S07

    // 5S00
    bs_gf_multiply(&B[A0], tmpProducts, 5);
    bs_gf_add(&Bp[A4], tmpProducts);

    // 3S01
    bs_gf_multiply(&B[A1], tmpProducts, 3);
    bs_gf_add(&Bp[A4], tmpProducts);

    // 5S02
    bs_gf_multiply(&B[A2], tmpProducts, 5);
    bs_gf_add(&Bp[A4], tmpProducts);

    // 7S03
    bs_gf_multiply(&B[A3], tmpProducts, 7);
    bs_gf_add(&Bp[A4], tmpProducts);

    // 2S04
    bs_gf_multiply(&B[A4], tmpProducts, 2);
    bs_gf_add(&Bp[A4], tmpProducts);

    // 2S05
    bs_gf_multiply(&B[A5], tmpProducts, 2);
    bs_gf_add(&Bp[A4], tmpProducts);

    // 3S06
    bs_gf_multiply(&B[A6], tmpProducts, 3);
    bs_gf_add(&Bp[A4], tmpProducts);

    // 4S07
    bs_gf_multiply(&B[A7], tmpProducts, 4);
    bs_gf_add(&Bp[A4], tmpProducts);

    // NEW_S05 = 4S00 + 5S01 + 3S02 + 5S03 + 7S04 + 2S05 + 2S06 + 3S07

    // 4S00
    bs_gf_multiply(&B[A0], tmpProducts, 4);
    bs_gf_add(&Bp[A5], tmpProducts);

    // 5S01
    bs_gf_multiply(&B[A1], tmpProducts, 5);
    bs_gf_add(&Bp[A5], tmpProducts);

    // 3S02
    bs_gf_multiply(&B[A2], tmpProducts, 3);
    bs_gf_add(&Bp[A5], tmpProducts);

    // 5S03
    bs_gf_multiply(&B[A3], tmpProducts, 5);
    bs_gf_add(&Bp[A5], tmpProducts);

    // 7S04
    bs_gf_multiply(&B[A4], tmpProducts, 7);
    bs_gf_add(&Bp[A5], tmpProducts);

    // 2S05
    bs_gf_multiply(&B[A5], tmpProducts, 2);
    bs_gf_add(&Bp[A5], tmpProducts);

    // 2S06
    bs_gf_multiply(&B[A6], tmpProducts, 2);
    bs_gf_add(&Bp[A5], tmpProducts);

    // 3S07
    bs_gf_multiply(&B[A7], tmpProducts, 3);
    bs_gf_add(&Bp[A5], tmpProducts);

    // NEW_S06 = 3S00 + 4S01 + 5S02 + 3S03 + 5S04 + 7S05 + 2S06 + 2S07

    // 3S00
    bs_gf_multiply(&B[A0], tmpProducts, 3);
    bs_gf_add(&Bp[A6], tmpProducts);

    // 4S01
    bs_gf_multiply(&B[A1], tmpProducts, 4);
    bs_gf_add(&Bp[A6], tmpProducts);

    // 5S02
    bs_gf_multiply(&B[A2], tmpProducts, 5);
    bs_gf_add(&Bp[A6], tmpProducts);

    // 3S03
    bs_gf_multiply(&B[A3], tmpProducts, 3);
    bs_gf_add(&Bp[A6], tmpProducts);

    // 5S04
    bs_gf_multiply(&B[A4], tmpProducts, 5);
    bs_gf_add(&Bp[A6], tmpProducts);

    // 7S05
    bs_gf_multiply(&B[A5], tmpProducts, 7);
    bs_gf_add(&Bp[A6], tmpProducts);

    // 2S06
    bs_gf_multiply(&B[A6], tmpProducts, 2);
    bs_gf_add(&Bp[A6], tmpProducts);

    // 2S07
    bs_gf_multiply(&B[A7], tmpProducts, 2);
    bs_gf_add(&Bp[A6], tmpProducts);

    // NEW_S07 = 2S00 + 3S01 + 4S02 + 5S03 + 3S04 + 5S05 + 7S06 + 2S07

    // 2S00
    bs_gf_multiply(&B[A0], tmpProducts, 2);
    bs_gf_add(&Bp[A7], tmpProducts);

    // 3S01
    bs_gf_multiply(&B[A1], tmpProducts, 3);
    bs_gf_add(&Bp[A7], tmpProducts);

    // 4S02
    bs_gf_multiply(&B[A2], tmpProducts, 4);
    bs_gf_add(&Bp[A7], tmpProducts);

    // 5S03
    bs_gf_multiply(&B[A3], tmpProducts, 5);
    bs_gf_add(&Bp[A7], tmpProducts);

    // 3S04
    bs_gf_multiply(&B[A4], tmpProducts, 3);
    bs_gf_add(&Bp[A7], tmpProducts);

    // 5S05
    bs_gf_multiply(&B[A5], tmpProducts, 5);
    bs_gf_add(&Bp[A7], tmpProducts);

    // 7S06
    bs_gf_multiply(&B[A6], tmpProducts, 7);
    bs_gf_add(&Bp[A7], tmpProducts);

    // 2S07
    bs_gf_multiply(&B[A7], tmpProducts, 2);
    bs_gf_add(&Bp[A7], tmpProducts);

        //
    Bp += BLOCK_SIZE/8;
    B  += BLOCK_SIZE/8;




    // //          2*S0     2*S1       2*S2      S2          // S1      S2           S3
    // Bp[A0+0] =                               B[A2+0]         B[A1+0] ^ B[A2+0] ^ B[A3+0] ^ of;
    // Bp[A0+1] = B[A0+0] ^ B[A1+0] ^ B[A2+0] ^ B[A2+0]       ^ B[A1+1] ^ B[A2+1] ^ B[A3+1] ^ of;
    // Bp[A0+2] = B[A0+1] ^ B[A1+1] ^ B[A2+0] ^ B[A2+0]       ^ B[A1+2] ^ B[A2+2] ^ B[A3+2];
    // Bp[A0+3] = B[A0+2] ^ B[A1+2] ^ B[A2+0] ^ B[A2+0]       ^ B[A1+3] ^ B[A2+3] ^ B[A3+3] ^ of;
    // Bp[A0+4] = B[A0+3] ^ B[A1+3] ^ B[A2+0] ^ B[A2+0]       ^ B[A1+4] ^ B[A2+4] ^ B[A3+4] ^ of;
    // Bp[A0+5] = B[A0+4] ^ B[A1+4] ^ B[A2+0] ^ B[A2+0]       ^ B[A1+5] ^ B[A2+5] ^ B[A3+5];
    // Bp[A0+6] = B[A0+5] ^ B[A1+5] ^ B[A2+0] ^ B[A2+0]       ^ B[A1+6] ^ B[A2+6] ^ B[A3+6];
    // Bp[A0+7] = B[A0+6] ^ B[A1+6] ^ B[A2+0] ^ B[A2+0]       ^ B[A1+7] ^ B[A2+7] ^ B[A3+7];



    // //  of = A1 ^ A2
    // //  A1 = A1 ^ (0x1b & ((signed char)of>>7));

    // //// A0
    // //  A1 = A1 ^ (A0)

    // //// + 2 * A1
    // //  A1 = A1 ^ (A1 << 1)

    // //// + 3 * A2
    // //  A1 = A1 ^ (A2)
    // //  A1 = A1 ^ (A2<<1)

    // //// + A3
    // //  A1 = A1 ^ (A3)

    // of = B[A1+7] ^ B[A2+7];

    // //          A0      2*A1        2*A2      A2        A3
    // Bp[A1+0] = B[A0+0]                     ^ B[A2+0] ^ B[A3+0] ^ of;
    // Bp[A1+1] = B[A0+1] ^ B[A1+0] ^ B[A2+0] ^ B[A2+1] ^ B[A3+1] ^ of;
    // Bp[A1+2] = B[A0+2] ^ B[A1+1] ^ B[A2+1] ^ B[A2+2] ^ B[A3+2];
    // Bp[A1+3] = B[A0+3] ^ B[A1+2] ^ B[A2+2] ^ B[A2+3] ^ B[A3+3] ^ of;
    // Bp[A1+4] = B[A0+4] ^ B[A1+3] ^ B[A2+3] ^ B[A2+4] ^ B[A3+4] ^ of;
    // Bp[A1+5] = B[A0+5] ^ B[A1+4] ^ B[A2+4] ^ B[A2+5] ^ B[A3+5];
    // Bp[A1+6] = B[A0+6] ^ B[A1+5] ^ B[A2+5] ^ B[A2+6] ^ B[A3+6];
    // Bp[A1+7] = B[A0+7] ^ B[A1+6] ^ B[A2+6] ^ B[A2+7] ^ B[A3+7];
    

    // //  of = A2 ^ A3
    // //  A2 = A2 ^ (0x1b & ((signed char)of>>7));

    // //// A0 + A1
    // //  A2 = A2 ^ (A0)
    // //  A2 = A2 ^ (A1)

    // //// + 2 * A2
    // //  A2 = A2 ^ (A2 << 1)

    // //// + 3 * A3
    // //  A2 = A2 ^ (A3)
    // //  A2 = A2 ^ (A3<<1)


    // of = B[A2+7] ^ B[A3+7];

    // //          A0      A1          2*A2       2*A3         A3
    // Bp[A2+0] = B[A0+0] ^ B[A1+0]                     ^ B[A3+0] ^ of;
    // Bp[A2+1] = B[A0+1] ^ B[A1+1] ^ B[A2+0] ^ B[A3+0] ^ B[A3+1] ^ of;
    // Bp[A2+2] = B[A0+2] ^ B[A1+2] ^ B[A2+1] ^ B[A3+1] ^ B[A3+2];
    // Bp[A2+3] = B[A0+3] ^ B[A1+3] ^ B[A2+2] ^ B[A3+2] ^ B[A3+3] ^ of;
    // Bp[A2+4] = B[A0+4] ^ B[A1+4] ^ B[A2+3] ^ B[A3+3] ^ B[A3+4] ^ of;
    // Bp[A2+5] = B[A0+5] ^ B[A1+5] ^ B[A2+4] ^ B[A3+4] ^ B[A3+5];
    // Bp[A2+6] = B[A0+6] ^ B[A1+6] ^ B[A2+5] ^ B[A3+5] ^ B[A3+6];
    // Bp[A2+7] = B[A0+7] ^ B[A1+7] ^ B[A2+6] ^ B[A3+6] ^ B[A3+7];
    

    // //  A3 = A0 ^ A3
    // //  A3 = A3 ^ (0x1b & ((signed char)of>>7));

    // //// 3 * A0
    // //  A3 = A3 ^ (A0)
    // //  A3 = A3 ^ (A0 << 1)

    // //// + A1 + A2
    // //  A3 = A3 ^ A1
    // //  A3 = A3 ^ A2

    // //// + 2 * A3
    // //  A3 = A3 ^ (A3<<1)

    // of = B[A0+7] ^ B[A3+7];

    // //        2*A0       A0         A1         A2       2*A3
    // Bp[A3+0] = B[A0+0] ^           B[A1+0] ^ B[A2+0]           ^ of;
    // Bp[A3+1] = B[A0+1] ^ B[A0+0] ^ B[A1+1] ^ B[A2+1] ^ B[A3+0] ^ of;
    // Bp[A3+2] = B[A0+2] ^ B[A0+1] ^ B[A1+2] ^ B[A2+2] ^ B[A3+1];
    // Bp[A3+3] = B[A0+3] ^ B[A0+2] ^ B[A1+3] ^ B[A2+3] ^ B[A3+2] ^ of;
    // Bp[A3+4] = B[A0+4] ^ B[A0+3] ^ B[A1+4] ^ B[A2+4] ^ B[A3+3] ^ of;
    // Bp[A3+5] = B[A0+5] ^ B[A0+4] ^ B[A1+5] ^ B[A2+5] ^ B[A3+4];
    // Bp[A3+6] = B[A0+6] ^ B[A0+5] ^ B[A1+6] ^ B[A2+6] ^ B[A3+5];
    // Bp[A3+7] = B[A0+7] ^ B[A0+6] ^ B[A1+7] ^ B[A2+7] ^ B[A3+6];
    

    // //
    // Bp += BLOCK_SIZE/4;
    // B  += BLOCK_SIZE/4;
    }


    memmove(B - BLOCK_SIZE,Bp - BLOCK_SIZE,sizeof(Bp_space));
}

void bs_mixcolumns_rev(word_t * B)
{
    // to understand this, see
    // https://en.wikipedia.org/wiki/Rijndael_mix_columns
    // TODO combine with shiftrows for performance on decryption
    word_t Bp_space[BLOCK_SIZE];
    word_t * Bp = Bp_space;

 
    int i = 0;
    for (; i < BLOCK_SIZE / 4; i += BLOCK_SIZE / 16)
    {

    //// state[i][0] = A0*0x0e + A1*0x0b + A2*0x0d + A3*0x09
    // overflow:
                    /* A0 * 0b1110             */   /* A1 * 0b1011     */   /* A2 * 0b1101   */   /* A3 * 0b1001 */
    word_t of0 = (  (B[A0+7] ^ B[A0+6] ^ B[A0+5]) ^ (B[A1 + 7] ^ B[A1+5]) ^ (B[A2+6] ^ B[A2+5]) ^ ( B[A3+5] ));       // 2 bit
    word_t of1 = (            (B[A0+7] ^ B[A0+6]) ^ (            B[A1+6]) ^ (B[A2+7] ^ B[A2+6]) ^ ( B[A3+6] ));       // 3 bit
    word_t of2 = (                      (B[A0+7]) ^ (            B[A1+7]) ^ (          B[A2+7]) ^ ( B[A3+7] ));       // 4 bit

    // inverse:
    //          1110      1011    1101      1001
    // A0 = A0 * 14 + A1 * 11 + A2 * 13 + A3 * 9
    // A0 = A0 * (2+4+8) + A1 * (1+2+8) + A2 * (1+4+8) + A3 * (1+8)

    //         (2*A0   +  4*A0   + 8*A0 )  + (8*A1   +  2*A1   +   A1  ) + ( A2    +   4*A2  +  8*A2 ) + (  A3   + 8*A3)
    Bp[A0+0] =                                                   B[A1+0] ^ B[A2+0]                     ^ B[A3+0]           ^ of0;
    Bp[A0+1] = B[A0+0]                               ^ B[A1+0] ^ B[A1+1] ^ B[A2+1]                     ^ B[A3+1]           ^ of0 ^ of1;
    Bp[A0+2] = B[A0+1] ^ B[A0+0]                     ^ B[A1+1] ^ B[A1+2] ^ B[A2+2] ^ B[A2+0]           ^ B[A3+2]                 ^ of1 ^ of2;
    Bp[A0+3] = B[A0+2] ^ B[A0+1] ^ B[A0+0] ^ B[A1+0] ^ B[A1+2] ^ B[A1+3] ^ B[A2+3] ^ B[A2+1] ^ B[A2+0] ^ B[A3+3] ^ B[A3+0] ^ of0       ^ of2;
    Bp[A0+4] = B[A0+3] ^ B[A0+2] ^ B[A0+1] ^ B[A1+1] ^ B[A1+3] ^ B[A1+4] ^ B[A2+4] ^ B[A2+2] ^ B[A2+1] ^ B[A3+4] ^ B[A3+1] ^ of0 ^ of1;
    Bp[A0+5] = B[A0+4] ^ B[A0+3] ^ B[A0+2] ^ B[A1+2] ^ B[A1+4] ^ B[A1+5] ^ B[A2+5] ^ B[A2+3] ^ B[A2+2] ^ B[A3+5] ^ B[A3+2]       ^ of1 ^ of2;
    Bp[A0+6] = B[A0+5] ^ B[A0+4] ^ B[A0+3] ^ B[A1+3] ^ B[A1+5] ^ B[A1+6] ^ B[A2+6] ^ B[A2+4] ^ B[A2+3] ^ B[A3+6] ^ B[A3+3]             ^ of2;
    Bp[A0+7] = B[A0+6] ^ B[A0+5] ^ B[A0+4] ^ B[A1+4] ^ B[A1+6] ^ B[A1+7] ^ B[A2+7] ^ B[A2+5] ^ B[A2+4] ^ B[A3+7] ^ B[A3+4];



    //// state[i][1] = A0*0x09 + A1*0xe + A2*0x0b + A3*0x0d
    // overflow:
          /* A0 * 0b1001 */   /* A1 * 0b1110             */ /* A2 * 0b101  1     */   /* A3 * 0b1101 */
    of0 = (  (B[A0+5]) ^      (B[A1+7] ^ B[A1+6] ^ B[A1+5]) ^ (B[A2 + 7] ^ B[A2+5]) ^ (B[A3+6] ^ B[A3+5]));       // 2 bit
    of1 = (  (B[A0+6]) ^                (B[A1+7] ^ B[A1+6]) ^ (            B[A2+6]) ^ (B[A3+7] ^ B[A3+6]));       // 3 bit
    of2 = (  (B[A0+7]) ^                          (B[A1+7]) ^ (            B[A2+7]) ^ (          B[A3+7]));       // 4 bit

    // inverse:
    //          1001      1110    1011      1101
    // A1 = A0 * 9 + A1 * 14 + A2 * 11 + A3 * 13
    // A1 = A0 * (1+8) + A1 * (2+4+8) + A2 * (1+2+8) + A3 * (1+4+8)

    //         (1*A0   +  8*A0 ) +(2*A1    +  4*A1   +  8*A1 ) + (1*A2   +  2*A2   +  8*A2 ) + (1*A3   +   4*A3  +  8*A3)
    Bp[A1+0] = B[A0+0] ^                                         B[A2+0] ^                     B[A3+0]                     ^ of0;
    Bp[A1+1] = B[A0+1] ^           B[A1+0]                     ^ B[A2+1] ^ B[A2+0] ^           B[A3+1]                     ^ of0 ^ of1;
    Bp[A1+2] = B[A0+2] ^           B[A1+1] ^ B[A1+0]           ^ B[A2+2] ^ B[A2+1] ^           B[A3+2] ^ B[A3+0]                 ^ of1 ^ of2;
    Bp[A1+3] = B[A0+3] ^ B[A0+0] ^ B[A1+2] ^ B[A1+1] ^ B[A1+0] ^ B[A2+3] ^ B[A2+2] ^ B[A2+0] ^ B[A3+3] ^ B[A3+1] ^ B[A3+0] ^ of0       ^ of2;
    Bp[A1+4] = B[A0+4] ^ B[A0+1] ^ B[A1+3] ^ B[A1+2] ^ B[A1+1] ^ B[A2+4] ^ B[A2+3] ^ B[A2+1] ^ B[A3+4] ^ B[A3+2] ^ B[A3+1] ^ of0 ^ of1;
    Bp[A1+5] = B[A0+5] ^ B[A0+2] ^ B[A1+4] ^ B[A1+3] ^ B[A1+2] ^ B[A2+5] ^ B[A2+4] ^ B[A2+2] ^ B[A3+5] ^ B[A3+3] ^ B[A3+2]       ^ of1 ^ of2;
    Bp[A1+6] = B[A0+6] ^ B[A0+3] ^ B[A1+5] ^ B[A1+4] ^ B[A1+3] ^ B[A2+6] ^ B[A2+5] ^ B[A2+3] ^ B[A3+6] ^ B[A3+4] ^ B[A3+3]             ^ of2;
    Bp[A1+7] = B[A0+7] ^ B[A0+4] ^ B[A1+6] ^ B[A1+5] ^ B[A1+4] ^ B[A2+7] ^ B[A2+6] ^ B[A2+4] ^ B[A3+7] ^ B[A3+5] ^ B[A3+4];


    //// state[i][2] = A0*0x0d + A1*0x09 + A2*0x0e + A3*0x0b
    // overflow:
          /* A1 * 0b1001 */  /* A2 * 0b1110              */ /* A3 * 0b1011      */   /* A0 * 0b1101 */
    of0 = (  (B[A1+5]) ^      (B[A2+7] ^ B[A2+6] ^ B[A2+5]) ^ (B[A3 + 7] ^ B[A3+5]) ^ (B[A0+6] ^ B[A0+5]));       // 2 bit
    of1 = (  (B[A1+6]) ^                (B[A2+7] ^ B[A2+6]) ^ (            B[A3+6]) ^ (B[A0+7] ^ B[A0+6]));       // 3 bit
    of2 = (  (B[A1+7]) ^                          (B[A2+7]) ^ (            B[A3+7]) ^ (          B[A0+7]));       // 4 bit

    // inverse:
    //          1001      1110    1011      1101
    // A2 = A1 * 9 + A2 * 14 + A3 * 11 + A0 * 13
    // A2 = A1 * (1+8) + A2 * (2+4+8) + A3 * (1+2+8) + A0 * (1+4+8)

    //         (1*A1   +  8*A1) + ( 2*A2   +  4*A2   +  8*A2)  + (1*A3      2*A2   +  8*A2) +  (1*A0   +   4*A0  +  8*A0)
    Bp[A2+0] = B[A1+0] ^                                         B[A3+0] ^                     B[A0+0]                     ^ of0;
    Bp[A2+1] = B[A1+1] ^           B[A2+0]                     ^ B[A3+1] ^ B[A3+0] ^           B[A0+1]                     ^ of0 ^ of1;
    Bp[A2+2] = B[A1+2] ^           B[A2+1] ^ B[A2+0]           ^ B[A3+2] ^ B[A3+1] ^           B[A0+2] ^ B[A0+0]                 ^ of1 ^ of2;
    Bp[A2+3] = B[A1+3] ^ B[A1+0] ^ B[A2+2] ^ B[A2+1] ^ B[A2+0] ^ B[A3+3] ^ B[A3+2] ^ B[A3+0] ^ B[A0+3] ^ B[A0+1] ^ B[A0+0] ^ of0       ^ of2;
    Bp[A2+4] = B[A1+4] ^ B[A1+1] ^ B[A2+3] ^ B[A2+2] ^ B[A2+1] ^ B[A3+4] ^ B[A3+3] ^ B[A3+1] ^ B[A0+4] ^ B[A0+2] ^ B[A0+1] ^ of0 ^ of1;
    Bp[A2+5] = B[A1+5] ^ B[A1+2] ^ B[A2+4] ^ B[A2+3] ^ B[A2+2] ^ B[A3+5] ^ B[A3+4] ^ B[A3+2] ^ B[A0+5] ^ B[A0+3] ^ B[A0+2]       ^ of1 ^ of2;
    Bp[A2+6] = B[A1+6] ^ B[A1+3] ^ B[A2+5] ^ B[A2+4] ^ B[A2+3] ^ B[A3+6] ^ B[A3+5] ^ B[A3+3] ^ B[A0+6] ^ B[A0+4] ^ B[A0+3]             ^ of2;
    Bp[A2+7] = B[A1+7] ^ B[A1+4] ^ B[A2+6] ^ B[A2+5] ^ B[A2+4] ^ B[A3+7] ^ B[A3+6] ^ B[A3+4] ^ B[A0+7] ^ B[A0+5] ^ B[A0+4];



    //// state[i][3] = A0*0x0b + A1*0x0d + A2*0x09 + A3*0x0e
    // overflow:
           /* A2 * 0b1001 */  /* A3 * 0b1110             */ /* A0 * 0b1011     */     /* A1 * 0b1101 */
    of0 = (  (B[A2+5]) ^      (B[A3+7] ^ B[A3+6] ^ B[A3+5]) ^ (B[A0 + 7] ^ B[A0+5]) ^ (B[A1+6] ^ B[A1+5]));       // 2 bit
    of1 = (  (B[A2+6]) ^                (B[A3+7] ^ B[A3+6]) ^ (            B[A0+6]) ^ (B[A1+7] ^ B[A1+6]));       // 3 bit
    of2 = (  (B[A2+7]) ^                          (B[A3+7]) ^ (            B[A0+7]) ^ (          B[A1+7]));       // 4 bit

    // inverse:
    //          1001      1110    1011      1101
    // A2 = A2 * 9 + A3 * 14 + A0 * 11 + A1 * 13
    // A2 = A2 * (1+8) + A3 * (2+4+8) + A0 * (1+2+8) + A1 * (1+4+8)

    //         (1*A2   +  8*A2) + ( 2*A3   +  4*A3   +  8*A3)  + (1*A0      2*A0   +  8*A0) +  (1*A1   +   4*A1  +  8*A1)
    Bp[A3+0] = B[A2+0] ^                                         B[A0+0] ^                     B[A1+0]                     ^ of0;
    Bp[A3+1] = B[A2+1] ^           B[A3+0]                     ^ B[A0+1] ^ B[A0+0] ^           B[A1+1]                     ^ of0 ^ of1;
    Bp[A3+2] = B[A2+2] ^           B[A3+1] ^ B[A3+0]           ^ B[A0+2] ^ B[A0+1] ^           B[A1+2] ^ B[A1+0]                 ^ of1 ^ of2;
    Bp[A3+3] = B[A2+3] ^ B[A2+0] ^ B[A3+2] ^ B[A3+1] ^ B[A3+0] ^ B[A0+3] ^ B[A0+2] ^ B[A0+0] ^ B[A1+3] ^ B[A1+1] ^ B[A1+0] ^ of0       ^ of2;
    Bp[A3+4] = B[A2+4] ^ B[A2+1] ^ B[A3+3] ^ B[A3+2] ^ B[A3+1] ^ B[A0+4] ^ B[A0+3] ^ B[A0+1] ^ B[A1+4] ^ B[A1+2] ^ B[A1+1] ^ of0 ^ of1;
    Bp[A3+5] = B[A2+5] ^ B[A2+2] ^ B[A3+4] ^ B[A3+3] ^ B[A3+2] ^ B[A0+5] ^ B[A0+4] ^ B[A0+2] ^ B[A1+5] ^ B[A1+3] ^ B[A1+2]       ^ of1 ^ of2;
    Bp[A3+6] = B[A2+6] ^ B[A2+3] ^ B[A3+5] ^ B[A3+4] ^ B[A3+3] ^ B[A0+6] ^ B[A0+5] ^ B[A0+3] ^ B[A1+6] ^ B[A1+4] ^ B[A1+3]             ^ of2;
    Bp[A3+7] = B[A2+7] ^ B[A2+4] ^ B[A3+6] ^ B[A3+5] ^ B[A3+4] ^ B[A0+7] ^ B[A0+6] ^ B[A0+4] ^ B[A1+7] ^ B[A1+5] ^ B[A1+4];

    Bp += BLOCK_SIZE/4;
    B  += BLOCK_SIZE/4;
    }

    memmove(B - BLOCK_SIZE, Bp - BLOCK_SIZE,sizeof(Bp_space));

}

void bs_expand_key(word_t (* rk)[BLOCK_SIZE], uint8_t * _key)
{
    // // TODO integrate this better
    // uint8_t key[KEY_SCHEDULE_SIZE];
    // memmove(key,_key,BLOCK_SIZE/8);
    // expand_key(key);

    // int i, j = 0, k, l;
    // for (i = 0; i < KEY_SCHEDULE_SIZE; i += (BLOCK_SIZE/8))
    // {
    //     memmove(rk[j], key + i, BLOCK_SIZE / 8);

    //     for (k = WORDS_PER_BLOCK; k < 128; k += WORDS_PER_BLOCK)
    //     {
    //         for (l = 0; l < WORDS_PER_BLOCK; l++)
    //         {
    //             rk[j][k + l] = rk[j][l];
    //         }
    //     }
    //     bs_transpose(rk[j]);
    //     j++;
    // }
}

void print_word_in_hex_and_binary(word_t word) {

    printf("Hex: %" PRIx64 "\n", word);
    for (int i = 63; i >= 0; i--) {
        printf("%llu", (word >> i) & 1);
    }
    printf("\n");
}


//normal unbitsliced
unsigned char GMul(unsigned char a, unsigned char b) { // Galois Field (256) Multiplication of two Bytes
    unsigned char p = 0;

    for (int counter = 0; counter < 8; counter++) {
        if ((b & 1) != 0) {
            p ^= a;
        }

        unsigned char hi_bit_set = (a & 0x80) != 0;
        a <<= 1;
        if (hi_bit_set) {
            a ^= 0x1B; /* x^8 = x^4 + x^3 + x + 1 */
        }
        b >>= 1;
    }

    printf("a: %d, b: %d, p: %d\n", a, b, p);

    return p;
}



#define P_CONSTANT_FIRST_ROW 0x7060504030201000ULL
#define Q_CONSTANT_LAST_ROW 0x8f9fafbfcfdfefffULL

#define ROTL64(a,n) ((((a)<<(n))|((a)>>(64-(n))))&(0xffffffffffffffffULL))
#define U64BIG(a) \
  ((ROTL64(a, 8) & (0x000000FF000000FFULL)) | \
   (ROTL64(a,24) & (0x0000FF000000FF00ULL)) | \
   (ROTL64(a,40) & (0x00FF000000FF0000ULL)) | \
   (ROTL64(a,56) & (0xFF000000FF000000ULL)))


// for BS_BLOCKSIZE, this is non bitsliced
void generate_roundc_matrix ( word_t * p_round_constant, word_t* q_round_constant, word_t round){

    // word_t round = 0; // goes from 0 - 9
    word_t round_constant = round;

        for (word_t i = 0; i < BLOCK_SIZE; i+= WORDS_PER_BLOCK)
        {
            // assuming 8 for now
       
            // p_round_constant[i + 0] = 0x0000000000000000ull ^ U64BIG(round_constant) ;
            // p_round_constant[i + 1] = 0x1000000000000000ull ^ U64BIG(round_constant) ;
            // p_round_constant[i + 2] = 0x2000000000000000ull ^ U64BIG(round_constant) ;
            // p_round_constant[i + 3] = 0x3000000000000000ull ^ U64BIG(round_constant) ;
            // p_round_constant[i + 4] = 0x4000000000000000ull^ U64BIG(round_constant) ;
            // p_round_constant[i + 5] = 0x5000000000000000ull ^ U64BIG(round_constant) ;
            // p_round_constant[i + 6] = 0x6000000000000000ull^ U64BIG(round_constant) ;
            // p_round_constant[i + 7] = 0x7000000000000000ull^ U64BIG(round_constant) ;

            p_round_constant[i + 0] = 0x0000000000000000ull ^ round_constant ;
            p_round_constant[i + 1] = 0x0000000000000010ull ^ round_constant ;
            p_round_constant[i + 2] = 0x0000000000000020ull ^ round_constant ;
            p_round_constant[i + 3] = 0x0000000000000030ull ^ round_constant ;
            p_round_constant[i + 4] = 0x0000000000000040ull^ round_constant ;
            p_round_constant[i + 5] = 0x0000000000000050ull ^ round_constant ;
            p_round_constant[i + 6] = 0x0000000000000060ull^ round_constant ;
            p_round_constant[i + 7] = 0x0000000000000070ull^ round_constant ;


            q_round_constant[i + 0] = 0xffffffffffffffffull^ U64BIG(round_constant) ;
            q_round_constant[i + 1] = 0xefffffffffffffffull^ U64BIG(round_constant);
            q_round_constant[i + 2] = 0xdfffffffffffffffull^ U64BIG(round_constant);
            q_round_constant[i + 3] = 0xcfffffffffffffffull^ U64BIG(round_constant);
            q_round_constant[i + 4] = 0xbfffffffffffffffull^ U64BIG(round_constant);
            q_round_constant[i + 5] = 0xafffffffffffffffull^ U64BIG(round_constant) ;
            q_round_constant[i + 6] = 0x9fffffffffffffffull^ U64BIG(round_constant);
            q_round_constant[i + 7] = 0x8fffffffffffffffull^ U64BIG(round_constant);
        }
}

void bs_generate_roundc_matrix_p_minimal ( word_t * p_round_constant, word_t round){

    uint8_t bit_index = 0; 
    const word_t P_ROUND_INITIAL_CONSTANT = 0x7060504030201000ULL;
    const int COLUMN_SIZE_BITS = 64; // 64 bits per groestl docs

    word_t P_ROUND_CONSTANT = 0;

    P_ROUND_CONSTANT = P_ROUND_INITIAL_CONSTANT ;

    // bit sliced round constants
    // one isthe simpler way to just transpose the round constant matrix and the other is to do it manually in a complicated way in an effort to optimize it.
    // bs_generate_roundc_matrix(bs_p_round_constant, bs_q_round_constant, round);
    for (int byte_index = 0; byte_index < 8; byte_index++) { // byte index into the P_ROUND_INITIAL_CONSTANT
        u8 byte_constant =  (P_ROUND_CONSTANT >> (8 * byte_index)) & 0xFF;// we take a byte out of the 64 bit constant. 
        byte_constant = byte_constant ^ round;
        // now we convert the bits in the byte constant to indices in the bit sliced array
        for (bit_index = 0; bit_index < 8; bit_index++){
            if (byte_constant & (1 << bit_index)){
                // printf("round: %d, bit_index: %d\n", round, bit_index);
                int index_into_bs_p_round_constant = byte_index * COLUMN_SIZE_BITS + bit_index;
                p_round_constant[index_into_bs_p_round_constant] = 0xffffffffffffffffULL;
            }
        }
    }

    // word_t round = 0; // goes from 0 - 9
}

// an attempt to generate the round constant matrix in a more efficient/performant way
void bs_generate_roundc_matrix_q_minimal ( word_t * q_round_constant, word_t round){

    uint8_t bit_index = 0; 
    const word_t Q_ROUND_INITIAL_CONSTANT = 0x8f9fafbfcfdfefffULL;
    const int COLUMN_SIZE_BITS = 64; // 64 bits per groestl docs

    word_t Q_ROUND_CONSTANT = 0;
    Q_ROUND_CONSTANT = Q_ROUND_INITIAL_CONSTANT;

    for (int byte_index = 0; byte_index < 8; byte_index++) { // byte index into the ROUND_INITIAL_CONSTANT
        u8 byte_constant =  (Q_ROUND_CONSTANT >> (8 * byte_index)) & 0xFF;// we take a byte out of the 64 bit constant. 
        byte_constant = byte_constant ^ round;
        // now we convert the bits in the byte constant to indices in the bit sliced array
        for (bit_index = 0; bit_index < 8; bit_index++){
            if ((byte_constant & (1 << bit_index)) == 0){
                // printf("round: %d, bit_index: %d\n", round, bit_index);
                // since the Q round constants 0x8f9fafbfcfdfefffULL are XORed to the MSBs of the 64 bit columns we add 56(i know weird number). See groestl docs, page 9.
                int index_into_bs_q_round_constant = byte_index * COLUMN_SIZE_BITS + bit_index + 56;
                // bs_q_round_constant_minimal[index_into_bs_q_round_constant] = 0xffffffffffffffffULL;
                q_round_constant[index_into_bs_q_round_constant] = 0x0000000000000ULL;
            }
        }
    }
    // word_t round = 0; // goes from 0 - 9
}



void bs_generate_roundc_matrix ( word_t * p_round_constant, word_t* q_round_constant, word_t round){

    word_t bs_q_round_constant[BLOCK_SIZE];
    word_t bs_p_round_constant[BLOCK_SIZE];

    // word_t round = 0; // goes from 0 - 9
    word_t round_constant = round;


        for (word_t i = 0; i < BLOCK_SIZE; i+= WORDS_PER_BLOCK)
        {

            p_round_constant[i + 0] = 0x0000000000000000ull ^ round_constant ;
            p_round_constant[i + 1] = 0x0000000000000010ull ^ round_constant ;
            p_round_constant[i + 2] = 0x0000000000000020ull ^ round_constant ;
            p_round_constant[i + 3] = 0x0000000000000030ull ^ round_constant ;
            p_round_constant[i + 4] = 0x0000000000000040ull^ round_constant ;
            p_round_constant[i + 5] = 0x0000000000000050ull ^ round_constant ;
            p_round_constant[i + 6] = 0x0000000000000060ull^ round_constant ;
            p_round_constant[i + 7] = 0x0000000000000070ull^ round_constant ;

            q_round_constant[i + 0] = 0xffffffffffffffffull^ U64BIG(round_constant) ;
            q_round_constant[i + 1] = 0xefffffffffffffffull^ U64BIG(round_constant);
            q_round_constant[i + 2] = 0xdfffffffffffffffull^ U64BIG(round_constant);
            q_round_constant[i + 3] = 0xcfffffffffffffffull^ U64BIG(round_constant);
            q_round_constant[i + 4] = 0xbfffffffffffffffull^ U64BIG(round_constant);
            q_round_constant[i + 5] = 0xafffffffffffffffull^ U64BIG(round_constant) ;
            q_round_constant[i + 6] = 0x9fffffffffffffffull^ U64BIG(round_constant);
            q_round_constant[i + 7] = 0x8fffffffffffffffull^ U64BIG(round_constant);

            // // assuming 8 for now
            // p_round_constant[i + 0] = P_CONSTANT_FIRST_ROW ^ round_constant;
            // p_round_constant[i + 1] = 0x0000000000000000ULL;
            // p_round_constant[i + 2] = 0x0000000000000000ULL;
            // p_round_constant[i + 3] = 0x0000000000000000ULL;
            // p_round_constant[i + 4] = 0x0000000000000000ULL;
            // p_round_constant[i + 5] = 0x0000000000000000ULL;
            // p_round_constant[i + 6] = 0x0000000000000000ULL;
            // p_round_constant[i + 7] = 0x0000000000000000ULL;

            // //for the alternative method 
            // bs_p_round_constant[i + 0] = 0x0000000000000000ULL;
            // bs_p_round_constant[i + 1] = 0x0000000000000000ULL;
            // bs_p_round_constant[i + 2] = 0x0000000000000000ULL;
            // bs_p_round_constant[i + 3] = 0x0000000000000000ULL;
            // bs_p_round_constant[i + 4] = 0x0000000000000000ULL;
            // bs_p_round_constant[i + 5] = 0x0000000000000000ULL;
            // bs_p_round_constant[i + 6] = 0x0000000000000000ULL;
            // bs_p_round_constant[i + 7] = 0x0000000000000000ULL;


            // q_round_constant[i + 0] = 0xffffffffffffffffULL;
            // q_round_constant[i + 1] = 0xffffffffffffffffULL;
            // q_round_constant[i + 2] = 0xffffffffffffffffULL;
            // q_round_constant[i + 3] = 0xffffffffffffffffULL;
            // q_round_constant[i + 4] = 0xffffffffffffffffULL;
            // q_round_constant[i + 5] = 0xffffffffffffffffULL;
            // q_round_constant[i + 6] = 0xffffffffffffffffULL;
            // q_round_constant[i + 7] = round_constant ^ Q_CONSTANT_LAST_ROW;

            // //for the alternative method 
            // bs_q_round_constant[i + 0] = 0xffffffffffffffffULL;
            // bs_q_round_constant[i + 1] = 0xffffffffffffffffULL;
            // bs_q_round_constant[i + 2] = 0xffffffffffffffffULL;
            // bs_q_round_constant[i + 3] = 0xffffffffffffffffULL;
            // bs_q_round_constant[i + 4] = 0xffffffffffffffffULL;
            // bs_q_round_constant[i + 5] = 0xffffffffffffffffULL;
            // bs_q_round_constant[i + 6] = 0xffffffffffffffffULL;
            // bs_q_round_constant[i + 7] = 0xffffffffffffffffULL;

        }
        
        bs_transpose(p_round_constant);
        bs_transpose(q_round_constant);
        return; // TODO remove this and check the code below


        // print the transposed round constant matrix
        printf ("P Round constant matrix for round %d\n", round);
        // printArray(p_round_constant);
        for (word_t i = 0; i < BLOCK_SIZE; i++) {
            if (p_round_constant[i] != 0) {
                printf("Line number: %d in p_round_constant \n", i);
                print_word_in_hex_and_binary(p_round_constant[i]);
            }

            // if (p_round_constant[i] != bs_p_round_constant_and_round) {
            // }
            // word_t bitpos = 1ULL << i;
            // if (bitpos & bs_p_round_constant_and_round) {
            //     // printf("Line number: %d in p_round_constant \n", i);
            //     // print_word_in_hex_and_binary(p_round_constant[i]);
            //     bs_p_round_constant[i] = 0xffffffffffffffffULL;
            // }
        }

        // printf ("Q Round constant matrix for round %d\n", round);

        // for (word_t i = 0; i < WORD_SIZE; i++) {
        //     if (q_round_constant[i] != 0xffffffffffffffffULL) {
        //         printf("Line number: %d in transposed q_round_constant \n", i);
        //         print_word_in_hex_and_binary(q_round_constant[i]);
        //     }
        // }



         //////////////////////// PPPPPPPPPPPPP/////////////////////////
       // for P the modified bits are in the first 64 bytes and for q they are in the last 64 bytes 
        word_t bs_p_round_constant_and_round = P_CONSTANT_FIRST_ROW ^ round_constant;
        for (word_t i = 0; i < 64; i++) {
            // if (p_round_constant[i] != bs_p_round_constant_and_round) {
            //     printf("Line number: %d in p_round_constant \n", i);
            //     print_word_in_hex_and_binary(p_round_constant[i]);
            // }
            word_t bitpos = 1ULL << i;
            if (bitpos & bs_p_round_constant_and_round) {
                // printf("Line number: %d in p_round_constant \n", i);
                // print_word_in_hex_and_binary(p_round_constant[i]);
                bs_p_round_constant[i] = 0xffffffffffffffffULL;
            }
        }

////////////////////////////////////QQQQQQQQQQ////////////////////////////
        // for q they are in the last 64 bytes 
        word_t bs_q_round_constant_and_round = Q_CONSTANT_LAST_ROW ^ round_constant;

        for (word_t i = BLOCK_SIZE - 64, j = 0; i < BLOCK_SIZE; i++, j++) {
            // if (p_round_constant[i] != bs_p_round_constant_and_round) {
            //     printf("Line number: %d in p_round_constant \n", i);
            //     print_word_in_hex_and_binary(p_round_constant[i]);
            // }
            word_t bitpos = 1ULL << j;
            if ((bitpos & bs_q_round_constant_and_round) == 0) {
                // printf("Line number: %d in p_round_constant \n", i);
                // print_word_in_hex_and_binary(p_round_constant[i]);
                bs_q_round_constant[i] = 0x0000000000000000ULL;
            }
        }

        // p_round_constant and bs_p_round_constant should be the same
        for (word_t i = 0; i < BLOCK_SIZE; i++) {
            if (p_round_constant[i] != bs_p_round_constant[i]) {
                printf("Line number: %d in p_round_constant \n", i);
                print_word_in_hex_and_binary(p_round_constant[i]);

                printf("Line number: %d in bs_p_round_constant \n", i);
                print_word_in_hex_and_binary(bs_p_round_constant[i]);
            }
        }

        // q_round_constant and bs_q_round_constant should be the same
        for (word_t i = 0; i < BLOCK_SIZE; i++) {
            if (q_round_constant[i] != bs_q_round_constant[i]) {
                printf("Line number: %d in q_round_constant \n", i);
                print_word_in_hex_and_binary(q_round_constant[i]);

                printf("Line number: %d in bs_q_round_constant \n", i);
                print_word_in_hex_and_binary(bs_q_round_constant[i]);
            }
        }

         

        // for (int i = 0; i < BLOCK_SIZE; i++) {
        //     if (p_round_constant[i] != 0x0000000000000000ULL) {

        //         printf("Line number: %d in p_round_constant \n", i);
        //         print_word_in_hex_and_binary(p_round_constant[i]);
        //     }
        // }

        // for (int i = 0; i < BLOCK_SIZE; i++) {
        //     if (q_round_constant[i] != 0xffffffffffffffffULL) {
        //         printf("Line number: %d in q_round_constant\n", i);
        //         print_word_in_hex_and_binary(q_round_constant[i]);
        //     }
        // }
}

void bs_cipher(word_t state[BLOCK_SIZE], word_t input[BLOCK_SIZE])
{
    word_t round = 0;
    word_t bs_p_round_constant[BLOCK_SIZE];
    word_t bs_q_round_constant[BLOCK_SIZE];



    // word_t p_round_constant[BLOCK_SIZE];
    // word_t q_round_constant[BLOCK_SIZE];
    // memset(p_round_constant, 0, sizeof(p_round_constant));
    // memset(q_round_constant, 0, sizeof(q_round_constant));

    word_t bs_m64_m[BLOCK_SIZE];
    word_t bs_m64_hm[BLOCK_SIZE];
    memset(bs_m64_m, 0, sizeof(bs_m64_m));
    memset(bs_m64_hm, 0, sizeof(bs_m64_hm));

    int word_index =0;

    for (word_index = 0; word_index < BLOCK_SIZE; word_index ++) {
        bs_m64_m[word_index] = input[word_index];
        bs_m64_hm[word_index] = state[word_index] ^ bs_m64_m[word_index];
    }
    // printf("\ninput P before xor with round constant\n");
    // printArray(bs_m64_hm);

    // printf("\ninput Q before xor with round constant\n");
    // printArray(bs_m64_m);

    word_t bs_tmp_copy[BLOCK_SIZE];
    memset(bs_tmp_copy, 0, sizeof(bs_tmp_copy));

    // printf("\ninput \n");
    // printArray(input);

    // technically P/Q can be parralelized from here but there does not seem to be much performance gain. Check branch bs_multithreaded
    for (round = 0; round < 10; round++)
    {

        // for (word_index = 0; word_index < BLOCK_SIZE; word_index ++) {
        //     if (bs_p_round_constant[word_index] != bs_p_round_constant_minimal[word_index]){
        //         printf("bs_p_round_constant[word_index] != bs_p_round_constant_minimal[word_index]\n");
        //         print_word_in_hex_and_binary(bs_p_round_constant[word_index]);
        //     }
        // }
        memset(bs_p_round_constant, 0, sizeof(bs_p_round_constant));
        memset(bs_q_round_constant, 0xff, sizeof(bs_q_round_constant)); // setting to 0xff since we are XORing with 0xff in Q. reset start of every round.

        bs_generate_roundc_matrix_p_minimal(bs_p_round_constant, round);
        bs_generate_roundc_matrix_q_minimal(bs_q_round_constant, round);

        // for (word_index = 0; word_index < BLOCK_SIZE; word_index ++) {
        //     if (bs_q_round_constant[word_index] != bs_q_round_constant_minimal[word_index]){
        //         printf("bs_q_round_constant[word_index] != bs_q_round_constant_minimal[word_index]\n");
        //         print_word_in_hex_and_binary(bs_q_round_constant[word_index]);
        //     }
        // }


        // non bit sliced
        // generate_roundc_matrix(bs_p_round_constant, bs_q_round_constant, round);

        // XOR with round constants
        for (word_index = 0; word_index < BLOCK_SIZE; word_index ++) {
            bs_m64_m[word_index] ^= bs_q_round_constant[word_index]; // for Q
           bs_m64_hm[word_index] ^= bs_p_round_constant[word_index]; // for P
        }

        // P 
        bs_apply_sbox(bs_m64_hm);
        bs_shiftrows_p(bs_m64_hm);
        bs_mixbytes(bs_m64_hm);

        // Q
        bs_apply_sbox(bs_m64_m);
        bs_shiftrows_q(bs_m64_m);
        bs_mixbytes(bs_m64_m);

    }

    // printf("\nQ bs_m64_m before XOR with state  \n");
    // printArray(bs_m64_m);

    for (word_index = 0; word_index < BLOCK_SIZE; word_index ++) {
        // tmp_xor[word_index] = bs_m64_hm[word_index] ^ bs_m64_m[word_index];
        // state[word_index] = state[word_index] ^ tmp_xor[word_index];
        state[word_index] = state[word_index] ^ bs_m64_m [word_index];
        state[word_index] = state [word_index] ^ bs_m64_hm[word_index];
    }
    // printf("\ntmp_xor\n");
    // printArray(tmp_xor);

    // printf("\nstate after the bs_m64_m xor\n");
    // printArray(state);

    // printf("\nstate after the xor\n");
    // printArray(state);

}
