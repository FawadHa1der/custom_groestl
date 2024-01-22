/*
 * CUDA-fied by Fawad Haider January 2024
 *
 */

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
#define __MMX__ 81
#define CUDAFIED 80
#if defined(__MMX__)
#include <stdio.h>
#include <stdlib.h>
#include "hash.h"
#include "tables.h"
#include <string.h>
#include <pthread.h>
#include <iostream>
#include <time.h>
#include <sys/time.h>

#define checkCudaErrors(val) check((val), #val, __FILE__, __LINE__)

void check(cudaError_t err, const char *const func, const char *const file,
           const int line)
{
  if (err != cudaSuccess)
  {
    std::cerr << "CUDA Runtime Error at: " << file << ":" << line
              << std::endl;
    std::cerr << cudaGetErrorString(err) << " " << func << std::endl;
    // We don't exit when we encounter CUDA errors in this example.
    // std::exit(EXIT_FAILURE);
  }
}

#define getLastCudaError() checkLast(__FILE__, __LINE__)

void checkLast(const char *const file, const int line)
{
  cudaError_t const err{cudaGetLastError()};
  if (err != cudaSuccess)
  {
    std::cerr << "CUDA Runtime Error at: " << file << ":" << line
              << std::endl;
    std::cerr << cudaGetErrorString(err) << std::endl;
    // We don't exit when we encounter CUDA errors in this example.
    // std::exit(EXIT_FAILURE);
  }
}

__device__ void ROUNDP512(u64 *m_in, u64 *m, int r, u64 *T_shared, int shared_message_index_64)
{

  u64 *T_m64 = (u64 *)T_shared;
  u64 zero;

  int local_message_block_index_32 = shared_message_index_64 % COLS512;
  int start_message_index = shared_message_index_64 - local_message_block_index_32; // need to find the start offset for the current message block

  u32 *x = (u32 *)&m_in[start_message_index];
  zero = 0;

  switch (local_message_block_index_32)
  {
  case 5:
    m[shared_message_index_64] = T_m64[3 * 256 + EXT_BYTE(x[0], 3)] ^ zero;
    break;
  case 3:
    m[shared_message_index_64] = T_m64[5 * 256 + EXT_BYTE(x[1], 1)] ^ zero;
    break;
  case 1:
    m[shared_message_index_64] = T_m64[7 * 256 + EXT_BYTE(x[1], 3)] ^ zero;
    break;
  case 7:
    m[shared_message_index_64] = T_m64[1 * 256 + EXT_BYTE(x[0], 1)] ^ zero;
    break;
  case 6:
    m[shared_message_index_64] = T_m64[2 * 256 + EXT_BYTE(x[0], 2)] ^ zero;
    break;
  case 4:
    m[shared_message_index_64] = T_m64[4 * 256 + EXT_BYTE(x[1], 0)] ^ zero;
    break;
  case 2:
    m[shared_message_index_64] = T_m64[6 * 256 + EXT_BYTE(x[1], 2)] ^ zero;
    break;
  case 0:
    m[shared_message_index_64] = T_m64[0 * 256 + ((r) ^ EXT_BYTE(x[0], 0))] ^ zero;
    break;
  }
  //  __syncthreads();
  switch (local_message_block_index_32)
  {
  case 5:
    m[shared_message_index_64] = T_m64[2 * 256 + EXT_BYTE(x[14], 2)] ^ m[shared_message_index_64];
    break;
  case 3:
    m[shared_message_index_64] = T_m64[4 * 256 + EXT_BYTE(x[15], 0)] ^ m[shared_message_index_64];
    break;
  case 1:
    m[shared_message_index_64] = T_m64[6 * 256 + EXT_BYTE(x[15], 2)] ^ m[shared_message_index_64];
    break;
  case 7:
    m[shared_message_index_64] = T_m64[0 * 256 + ((r) ^ 0x70 ^ EXT_BYTE(x[14], 0))] ^ m[shared_message_index_64];
    break;
  case 6:
    m[shared_message_index_64] = T_m64[1 * 256 + EXT_BYTE(x[14], 1)] ^ m[shared_message_index_64];
    break;
  case 4:
    m[shared_message_index_64] = T_m64[3 * 256 + EXT_BYTE(x[14], 3)] ^ m[shared_message_index_64];
    break;
  case 2:
    m[shared_message_index_64] = T_m64[5 * 256 + EXT_BYTE(x[15], 1)] ^ m[shared_message_index_64];
    break;
  case 0:
    m[shared_message_index_64] = T_m64[7 * 256 + EXT_BYTE(x[15], 3)] ^ m[shared_message_index_64];
    break;
  }

  // __syncthreads();
  switch (local_message_block_index_32)
  {
  case 5:
    m[shared_message_index_64] = T_m64[1 * 256 + EXT_BYTE(x[12], 1)] ^ m[shared_message_index_64];
    break;
  case 3:
    m[shared_message_index_64] = T_m64[3 * 256 + EXT_BYTE(x[12], 3)] ^ m[shared_message_index_64];
    break;
  case 1:
    m[shared_message_index_64] = T_m64[5 * 256 + EXT_BYTE(x[13], 1)] ^ m[shared_message_index_64];
    break;
  case 7:
    m[shared_message_index_64] = T_m64[7 * 256 + EXT_BYTE(x[13], 3)] ^ m[shared_message_index_64];
    break;
  case 6:
    m[shared_message_index_64] = T_m64[0 * 256 + ((r) ^ 0x60 ^ EXT_BYTE(x[12], 0))] ^ m[shared_message_index_64];
    break;
  case 4:
    m[shared_message_index_64] = T_m64[2 * 256 + EXT_BYTE(x[12], 2)] ^ m[shared_message_index_64];
    break;
  case 2:
    m[shared_message_index_64] = T_m64[4 * 256 + EXT_BYTE(x[13], 0)] ^ m[shared_message_index_64];
    break;
  case 0:
    m[shared_message_index_64] = T_m64[6 * 256 + EXT_BYTE(x[13], 2)] ^ m[shared_message_index_64];
    break;
  }

  // __syncthreads();
  switch (local_message_block_index_32)
  {
  case 5:
    m[shared_message_index_64] = T_m64[0 * 256 + ((r) ^ 0x50 ^ EXT_BYTE(x[10], 0))] ^ m[shared_message_index_64];
    break;
  case 3:
    m[shared_message_index_64] = T_m64[2 * 256 + EXT_BYTE(x[10], 2)] ^ m[shared_message_index_64];
    break;
  case 1:
    m[shared_message_index_64] = T_m64[4 * 256 + EXT_BYTE(x[11], 0)] ^ m[shared_message_index_64];
    break;
  case 7:
    m[shared_message_index_64] = T_m64[6 * 256 + EXT_BYTE(x[11], 2)] ^ m[shared_message_index_64];
    break;
  case 6:
    m[shared_message_index_64] = T_m64[7 * 256 + EXT_BYTE(x[11], 3)] ^ m[shared_message_index_64];
    break;
  case 4:
    m[shared_message_index_64] = T_m64[1 * 256 + EXT_BYTE(x[10], 1)] ^ m[shared_message_index_64];
    break;
  case 2:
    m[shared_message_index_64] = T_m64[3 * 256 + EXT_BYTE(x[10], 3)] ^ m[shared_message_index_64];
    break;
  case 0:
    m[shared_message_index_64] = T_m64[5 * 256 + EXT_BYTE(x[11], 1)] ^ m[shared_message_index_64];
    break;
  }

  // __syncthreads();
  switch (local_message_block_index_32)
  {
  case 5:
    m[shared_message_index_64] = T_m64[7 * 256 + EXT_BYTE(x[9], 3)] ^ m[shared_message_index_64];
    break;
  case 3:
    m[shared_message_index_64] = T_m64[1 * 256 + EXT_BYTE(x[8], 1)] ^ m[shared_message_index_64];
    break;
  case 1:
    m[shared_message_index_64] = T_m64[3 * 256 + EXT_BYTE(x[8], 3)] ^ m[shared_message_index_64];
    break;
  case 7:
    m[shared_message_index_64] = T_m64[5 * 256 + EXT_BYTE(x[9], 1)] ^ m[shared_message_index_64];
    break;
  case 6:
    m[shared_message_index_64] = T_m64[6 * 256 + EXT_BYTE(x[9], 2)] ^ m[shared_message_index_64];
    break;
  case 4:
    m[shared_message_index_64] = T_m64[0 * 256 + ((r) ^ 0x40 ^ EXT_BYTE(x[8], 0))] ^ m[shared_message_index_64];
    break;
  case 2:
    m[shared_message_index_64] = T_m64[2 * 256 + EXT_BYTE(x[8], 2)] ^ m[shared_message_index_64];
    break;
  case 0:
    m[shared_message_index_64] = T_m64[4 * 256 + EXT_BYTE(x[9], 0)] ^ m[shared_message_index_64];
    break;
  }

 // __syncthreads();
  switch (local_message_block_index_32)
  {
  case 5:
    m[shared_message_index_64] = T_m64[6 * 256 + EXT_BYTE(x[7], 2)] ^ m[shared_message_index_64];
    break;
  case 3:
    m[shared_message_index_64] = T_m64[0 * 256 + ((r) ^ 0x30 ^ EXT_BYTE(x[6], 0))] ^ m[shared_message_index_64];
    break;
  case 1:
    m[shared_message_index_64] = T_m64[2 * 256 + EXT_BYTE(x[6], 2)] ^ m[shared_message_index_64];
    break;
  case 7:
    m[shared_message_index_64] = T_m64[4 * 256 + EXT_BYTE(x[7], 0)] ^ m[shared_message_index_64];
    break;
  case 6:
    m[shared_message_index_64] = T_m64[5 * 256 + EXT_BYTE(x[7], 1)] ^ m[shared_message_index_64];
    break;
  case 4:
    m[shared_message_index_64] = T_m64[7 * 256 + EXT_BYTE(x[7], 3)] ^ m[shared_message_index_64];
    break;
  case 2:
    m[shared_message_index_64] = T_m64[1 * 256 + EXT_BYTE(x[6], 1)] ^ m[shared_message_index_64];
    break;
  case 0:
    m[shared_message_index_64] = T_m64[3 * 256 + EXT_BYTE(x[6], 3)] ^ m[shared_message_index_64];
    break;
  }

  // __syncthreads();
  switch (local_message_block_index_32)
  {
  case 5:
    m[shared_message_index_64] = T_m64[5 * 256 + EXT_BYTE(x[5], 1)] ^ m[shared_message_index_64];
    break;
  case 3:
    m[shared_message_index_64] = T_m64[7 * 256 + EXT_BYTE(x[5], 3)] ^ m[shared_message_index_64];
    break;
  case 1:
    m[shared_message_index_64] = T_m64[1 * 256 + EXT_BYTE(x[4], 1)] ^ m[shared_message_index_64];
    break;
  case 7:
    m[shared_message_index_64] = T_m64[3 * 256 + EXT_BYTE(x[4], 3)] ^ m[shared_message_index_64];
    break;
  case 6:
    m[shared_message_index_64] = T_m64[4 * 256 + EXT_BYTE(x[5], 0)] ^ m[shared_message_index_64];
    break;
  case 4:
    m[shared_message_index_64] = T_m64[6 * 256 + EXT_BYTE(x[5], 2)] ^ m[shared_message_index_64];
    break;
  case 2:
    m[shared_message_index_64] = T_m64[0 * 256 + ((r) ^ 0x20 ^ EXT_BYTE(x[4], 0))] ^ m[shared_message_index_64];
    break;
  case 0:
    m[shared_message_index_64] = T_m64[2 * 256 + EXT_BYTE(x[4], 2)] ^ m[shared_message_index_64];
    break;
  }

  // __syncthreads();
  switch (local_message_block_index_32)
  {
  case 5:
    m[shared_message_index_64] = T_m64[4 * 256 + EXT_BYTE(x[3], 0)] ^ m[shared_message_index_64];
    break;
  case 3:
    m[shared_message_index_64] = T_m64[6 * 256 + EXT_BYTE(x[3], 2)] ^ m[shared_message_index_64];
    break;
  case 1:
    m[shared_message_index_64] = T_m64[0 * 256 + ((r) ^ 0x10 ^ EXT_BYTE(x[2], 0))] ^ m[shared_message_index_64];
    break;
  case 7:
    m[shared_message_index_64] = T_m64[2 * 256 + EXT_BYTE(x[2], 2)] ^ m[shared_message_index_64];
    break;
  case 6:
    m[shared_message_index_64] = T_m64[3 * 256 + EXT_BYTE(x[2], 3)] ^ m[shared_message_index_64];
    break;
  case 4:
    m[shared_message_index_64] = T_m64[5 * 256 + EXT_BYTE(x[3], 1)] ^ m[shared_message_index_64];
    break;
  case 2:
    m[shared_message_index_64] = T_m64[7 * 256 + EXT_BYTE(x[3], 3)] ^ m[shared_message_index_64];
    break;
  case 0:
    m[shared_message_index_64] = T_m64[1 * 256 + EXT_BYTE(x[2], 1)] ^ m[shared_message_index_64];
    break;
  }

 // __syncthreads();
}

__device__ void ROUNDQ512(u64 *m_in, u64 *m, int r, u64 *T_shared, int shared_message_index_64)
{

  u64 *T_m64 = (u64 *)T_shared;
  u64 zero;
  u64 ff;
  int local_message_block_index_32 = shared_message_index_64 % COLS512;
  int start_message_index = shared_message_index_64 - local_message_block_index_32; // need to find the start offset for the current message block
  u32 *x = (u32 *)&m_in[start_message_index];
  zero = 0;
  ff = -1;

  m_in[shared_message_index_64] = (m_in[shared_message_index_64] ^ ff); // even is for shared_message_index_64 64 bits array, we can use it in the 2 bit array in this case

  switch (local_message_block_index_32)
  {
  case 7:
    m[shared_message_index_64] = T_m64[0 * 256 + EXT_BYTE(x[0], 0)] ^ zero;
    break;

  case 5:
    m[shared_message_index_64] = T_m64[1 * 256 + EXT_BYTE(x[0], 1)] ^ zero;
    break;

  case 3:
    m[shared_message_index_64] = T_m64[2 * 256 + EXT_BYTE(x[0], 2)] ^ zero;
    break;

  case 1:
    m[shared_message_index_64] = T_m64[3 * 256 + EXT_BYTE(x[0], 3)] ^ zero;
    break;

  case 0:
    m[shared_message_index_64] = T_m64[4 * 256 + EXT_BYTE(x[1], 0)] ^ zero;
    break;

  case 6:
    m[shared_message_index_64] = T_m64[5 * 256 + EXT_BYTE(x[1], 1)] ^ zero;
    break;

  case 4:
    m[shared_message_index_64] = T_m64[6 * 256 + EXT_BYTE(x[1], 2)] ^ zero;
    break;

  case 2:
    m[shared_message_index_64] = T_m64[7 * 256 + ((r) ^ EXT_BYTE(x[1], 3))] ^ zero;
    break;
  }

  // __syncthreads();

  switch (local_message_block_index_32)
  {
  case 7:
    m[shared_message_index_64] = T_m64[4 * 256 + EXT_BYTE(x[15], 0)] ^ m[shared_message_index_64];
    break;

  case 5:
    m[shared_message_index_64] = T_m64[5 * 256 + EXT_BYTE(x[15], 1)] ^ m[shared_message_index_64];
    break;

  case 3:
    m[shared_message_index_64] = T_m64[6 * 256 + EXT_BYTE(x[15], 2)] ^ m[shared_message_index_64];
    break;

  case 1:
    m[shared_message_index_64] = T_m64[7 * 256 + ((r) ^ 0x70 ^ EXT_BYTE(x[15], 3))] ^ m[shared_message_index_64];
    break;

  case 0:
    m[shared_message_index_64] = T_m64[3 * 256 + EXT_BYTE(x[14], 3)] ^ m[shared_message_index_64];
    break;

  case 6:
    m[shared_message_index_64] = T_m64[0 * 256 + EXT_BYTE(x[14], 0)] ^ m[shared_message_index_64];
    break;

  case 4:
    m[shared_message_index_64] = T_m64[1 * 256 + EXT_BYTE(x[14], 1)] ^ m[shared_message_index_64];
    break;

  case 2:
    m[shared_message_index_64] = T_m64[2 * 256 + EXT_BYTE(x[14], 2)] ^ m[shared_message_index_64];
    break;
  }

  // __syncthreads();

  switch (local_message_block_index_32)
  {
  case 5:
    m[shared_message_index_64] = T_m64[0 * 256 + EXT_BYTE(x[12], 0)] ^ m[shared_message_index_64];
    break;
  case 3:
    m[shared_message_index_64] = T_m64[1 * 256 + EXT_BYTE(x[12], 1)] ^ m[shared_message_index_64];
    break;
  case 1:
    m[shared_message_index_64] = T_m64[2 * 256 + EXT_BYTE(x[12], 2)] ^ m[shared_message_index_64];
    break;
  case 7:
    m[shared_message_index_64] = T_m64[3 * 256 + EXT_BYTE(x[12], 3)] ^ m[shared_message_index_64];
    break;
  case 6:
    m[shared_message_index_64] = T_m64[4 * 256 + EXT_BYTE(x[13], 0)] ^ m[shared_message_index_64];
    break;
  case 4:
    m[shared_message_index_64] = T_m64[5 * 256 + EXT_BYTE(x[13], 1)] ^ m[shared_message_index_64];
    break;
  case 2:
    m[shared_message_index_64] = T_m64[6 * 256 + EXT_BYTE(x[13], 2)] ^ m[shared_message_index_64];
    break;
  case 0:
    m[shared_message_index_64] = T_m64[7 * 256 + ((r) ^ 0x60 ^ EXT_BYTE(x[13], 3))] ^ m[shared_message_index_64];
    break;
  }
  // __syncthreads();

  switch (local_message_block_index_32)
  {
  case 5:
    m[shared_message_index_64] = T_m64[4 * 256 + EXT_BYTE(x[11], 0)] ^ m[shared_message_index_64];
    break;
  case 3:
    m[shared_message_index_64] = T_m64[5 * 256 + EXT_BYTE(x[11], 1)] ^ m[shared_message_index_64];
    break;
  case 1:
    m[shared_message_index_64] = T_m64[6 * 256 + EXT_BYTE(x[11], 2)] ^ m[shared_message_index_64];
    break;
  case 7:
    m[shared_message_index_64] = T_m64[7 * 256 + ((r) ^ 0x50 ^ EXT_BYTE(x[11], 3))] ^ m[shared_message_index_64];
    break;
  case 6:
    m[shared_message_index_64] = T_m64[3 * 256 + EXT_BYTE(x[10], 3)] ^ m[shared_message_index_64];
    break;
  case 4:
    m[shared_message_index_64] = T_m64[0 * 256 + EXT_BYTE(x[10], 0)] ^ m[shared_message_index_64];
    break;
  case 2:
    m[shared_message_index_64] = T_m64[1 * 256 + EXT_BYTE(x[10], 1)] ^ m[shared_message_index_64];
    break;
  case 0:
    m[shared_message_index_64] = T_m64[2 * 256 + EXT_BYTE(x[10], 2)] ^ m[shared_message_index_64];
    break;
  }

  // __syncthreads();

  switch (local_message_block_index_32)
  {
  case 5:
    m[shared_message_index_64] = T_m64[3 * 256 + EXT_BYTE(x[8], 3)] ^ m[shared_message_index_64];
    break;
  case 3:
    m[shared_message_index_64] = T_m64[0 * 256 + EXT_BYTE(x[8], 0)] ^ m[shared_message_index_64];
    break;
  case 1:
    m[shared_message_index_64] = T_m64[1 * 256 + EXT_BYTE(x[8], 1)] ^ m[shared_message_index_64];
    break;
  case 7:
    m[shared_message_index_64] = T_m64[2 * 256 + EXT_BYTE(x[8], 2)] ^ m[shared_message_index_64];
    break;
  case 6:
    m[shared_message_index_64] = T_m64[7 * 256 + ((r) ^ 0x40 ^ EXT_BYTE(x[9], 3))] ^ m[shared_message_index_64];
    break;
  case 4:
    m[shared_message_index_64] = T_m64[4 * 256 + EXT_BYTE(x[9], 0)] ^ m[shared_message_index_64];
    break;
  case 2:
    m[shared_message_index_64] = T_m64[5 * 256 + EXT_BYTE(x[9], 1)] ^ m[shared_message_index_64];
    break;
  case 0:
    m[shared_message_index_64] = T_m64[6 * 256 + EXT_BYTE(x[9], 2)] ^ m[shared_message_index_64];
    break;
  }

  // __syncthreads();

  switch (local_message_block_index_32)
  {
  case 5:
    m[shared_message_index_64] = T_m64[7 * 256 + ((r) ^ 0x30 ^ EXT_BYTE(x[7], 3))] ^ m[shared_message_index_64];
    break;
  case 3:
    m[shared_message_index_64] = T_m64[4 * 256 + EXT_BYTE(x[7], 0)] ^ m[shared_message_index_64];
    break;
  case 1:
    m[shared_message_index_64] = T_m64[5 * 256 + EXT_BYTE(x[7], 1)] ^ m[shared_message_index_64];
    break;
  case 7:
    m[shared_message_index_64] = T_m64[6 * 256 + EXT_BYTE(x[7], 2)] ^ m[shared_message_index_64];
    break;
  case 6:
    m[shared_message_index_64] = T_m64[2 * 256 + EXT_BYTE(x[6], 2)] ^ m[shared_message_index_64];
    break;
  case 4:
    m[shared_message_index_64] = T_m64[3 * 256 + EXT_BYTE(x[6], 3)] ^ m[shared_message_index_64];
    break;
  case 2:
    m[shared_message_index_64] = T_m64[0 * 256 + EXT_BYTE(x[6], 0)] ^ m[shared_message_index_64];
    break;
  case 0:
    m[shared_message_index_64] = T_m64[1 * 256 + EXT_BYTE(x[6], 1)] ^ m[shared_message_index_64];
    break;
  }

  // __syncthreads();

  switch (local_message_block_index_32)
  {
  case 5:
    m[shared_message_index_64] = T_m64[2 * 256 + EXT_BYTE(x[4], 2)] ^ m[shared_message_index_64];
    break;
  case 3:
    m[shared_message_index_64] = T_m64[3 * 256 + EXT_BYTE(x[4], 3)] ^ m[shared_message_index_64];
    break;
  case 1:
    m[shared_message_index_64] = T_m64[0 * 256 + EXT_BYTE(x[4], 0)] ^ m[shared_message_index_64];
    break;
  case 7:
    m[shared_message_index_64] = T_m64[1 * 256 + EXT_BYTE(x[4], 1)] ^ m[shared_message_index_64];
    break;
  case 6:
    m[shared_message_index_64] = T_m64[6 * 256 + EXT_BYTE(x[5], 2)] ^ m[shared_message_index_64];
    break;
  case 4:
    m[shared_message_index_64] = T_m64[7 * 256 + ((r) ^ 0x20 ^ EXT_BYTE(x[5], 3))] ^ m[shared_message_index_64];
    break;
  case 2:
    m[shared_message_index_64] = T_m64[4 * 256 + EXT_BYTE(x[5], 0)] ^ m[shared_message_index_64];
    break;
  case 0:
    m[shared_message_index_64] = T_m64[5 * 256 + EXT_BYTE(x[5], 1)] ^ m[shared_message_index_64];
    break;
  }

  // __syncthreads();

  switch (local_message_block_index_32)
  {
  case 5:
    m[shared_message_index_64] = T_m64[6 * 256 + EXT_BYTE(x[3], 2)] ^ m[shared_message_index_64];
    break;
  case 3:
    m[shared_message_index_64] = T_m64[7 * 256 + ((r) ^ 0x10 ^ EXT_BYTE(x[3], 3))] ^ m[shared_message_index_64];
    break;
  case 1:
    m[shared_message_index_64] = T_m64[4 * 256 + EXT_BYTE(x[3], 0)] ^ m[shared_message_index_64];
    break;
  case 7:
    m[shared_message_index_64] = T_m64[5 * 256 + EXT_BYTE(x[3], 1)] ^ m[shared_message_index_64];
    break;
  case 6:
    m[shared_message_index_64] = T_m64[1 * 256 + EXT_BYTE(x[2], 1)] ^ m[shared_message_index_64];
    break;
  case 4:
    m[shared_message_index_64] = T_m64[2 * 256 + EXT_BYTE(x[2], 2)] ^ m[shared_message_index_64];
    break;
  case 2:
    m[shared_message_index_64] = T_m64[3 * 256 + EXT_BYTE(x[2], 3)] ^ m[shared_message_index_64];
    break;
  case 0:
    m[shared_message_index_64] = T_m64[0 * 256 + EXT_BYTE(x[2], 0)] ^ m[shared_message_index_64];
    break;
  }

  // __syncthreads();
}

#define TABLE_SIZE 2048
#define SHARED_Q_RESULTS_SIZE 512

/* digest part of a message in short variants */
__global__ void Transform512(u32 *outputTransformation, int outputTransformSize, const u8 *msg, int msglen, const u8 *msg_transformation_buffer)
{

  int i;

  //TODO some of these shared mem vars can be re-used and redced 
  __shared__ u64 m64_m[COLS512];
  __shared__ u64 *m64_h;
  __shared__ u64 m64_hm[COLS512];
  __shared__ u64 tmp[COLS512];
  __shared__ u64 tmp_output[COLS512];
  __shared__ u64 output_transformation_shared[COLS512];
  __shared__ u64 T_shared[TABLE_SIZE];
  __shared__ u64 q_shared_results_64[SHARED_Q_RESULTS_SIZE];
  __shared__ u64 tmp_shared_64[SHARED_Q_RESULTS_SIZE];

  int threadsPerBlock = blockDim.x;

  if (threadsPerBlock > SHARED_Q_RESULTS_SIZE)
  {
    printf("we have an error, block dimensions cannot be bigger than shared q results size  ");
    return;
  }

  int totalLoads = TABLE_SIZE / threadsPerBlock; // Each thread loads this many 64 bit values
  int sharedMemoryLoadIndex = threadIdx.x * totalLoads;
  int end = sharedMemoryLoadIndex + totalLoads;

  // Load data into shared memory
  for (int runningLoadIndex = sharedMemoryLoadIndex; runningLoadIndex < end; ++runningLoadIndex)
  {
    // sharedData[runningLoadIndex] = globalData[runningLoadIndex];
    if (runningLoadIndex < TABLE_SIZE)
    { // Ensure we don't go out of bounds of shared memory
      T_shared[runningLoadIndex] = T[runningLoadIndex];
    }
  }

 // Synchronize to ensure all data is loaded
 // __syncthreads();

  int grid_stride_local_index = blockIdx.x * blockDim.x + threadIdx.x;
  int stride = blockDim.x * gridDim.x;
  u64 *msg_64 = (u64 *)msg;

  u64 *transformation_buffer_q_64 = (u64 *)msg_transformation_buffer;
  int msglen_64 = msglen / COLS512; // length of char* data type to  u64* type

  for (int global_index_64 = grid_stride_local_index; global_index_64 < (msglen_64); global_index_64 += stride)
  {
    if (global_index_64 < msglen_64)
    {
      int q_local_index_shared_64 = global_index_64 % SHARED_Q_RESULTS_SIZE;
      q_shared_results_64[q_local_index_shared_64] = msg_64[global_index_64];

      ROUNDQ512(q_shared_results_64, tmp_shared_64, 0, (u64 *)T_shared, q_local_index_shared_64);
      ROUNDQ512(tmp_shared_64, q_shared_results_64, 1, (u64 *)T_shared, q_local_index_shared_64);
      ROUNDQ512(q_shared_results_64, tmp_shared_64, 2, (u64 *)T_shared, q_local_index_shared_64);
      ROUNDQ512(tmp_shared_64, q_shared_results_64, 3, (u64 *)T_shared, q_local_index_shared_64);
      ROUNDQ512(q_shared_results_64, tmp_shared_64, 4, (u64 *)T_shared, q_local_index_shared_64);
      ROUNDQ512(tmp_shared_64, q_shared_results_64, 5, (u64 *)T_shared, q_local_index_shared_64);
      ROUNDQ512(q_shared_results_64, tmp_shared_64, 6, (u64 *)T_shared, q_local_index_shared_64);
      ROUNDQ512(tmp_shared_64, q_shared_results_64, 7, (u64 *)T_shared, q_local_index_shared_64);
      ROUNDQ512(q_shared_results_64, tmp_shared_64, 8, (u64 *)T_shared, q_local_index_shared_64);
      ROUNDQ512(tmp_shared_64, q_shared_results_64, 9, (u64 *)T_shared, q_local_index_shared_64);

      // __syncthreads();
      transformation_buffer_q_64[global_index_64] = q_shared_results_64[q_local_index_shared_64];
    }
  }

  
  // Processing P blocks and thread coarsening to 8 threads. P is not very parallelizable 
  if ((blockIdx.x == 0) && (threadIdx.x < 8))
  {
    int tid = threadIdx.x;
    m64_h = (u64 *)outputTransformation;
    output_transformation_shared[tid] = m64_h[tid];
    u64 *current_q_transformed_block = transformation_buffer_q_64;

    int tmpIndex = 0;

    while (msglen >= SIZE512)
    {
      msg_64 = (u64 *)msg;
      m64_hm[tid] = output_transformation_shared[tid] ^ msg_64[tid];

      ROUNDP512(m64_hm, tmp, 0, T_shared, tid);
      ROUNDP512(tmp, m64_hm, 1, T_shared, tid);
      ROUNDP512(m64_hm, tmp, 2, T_shared, tid);
      ROUNDP512(tmp, m64_hm, 3, T_shared, tid);
      ROUNDP512(m64_hm, tmp, 4, T_shared, tid);
      ROUNDP512(tmp, m64_hm, 5, T_shared, tid);
      ROUNDP512(m64_hm, tmp, 6, T_shared, tid);
      ROUNDP512(tmp, m64_hm, 7, T_shared, tid);
      ROUNDP512(m64_hm, tmp, 8, T_shared, tid);
      ROUNDP512(tmp, m64_hm, 9, T_shared, tid);

      output_transformation_shared[tid] = output_transformation_shared[tid] ^ current_q_transformed_block[tid];
      output_transformation_shared[tid] = output_transformation_shared[tid] ^ m64_hm[tid];

      msg += SIZE512;
      current_q_transformed_block += COLS512;
      msglen -= SIZE512;
      tmpIndex++;
    }

//    __syncthreads();

    tmp[tid] = output_transformation_shared[tid];

    ROUNDP512(tmp, tmp_output, 0, T_shared, tid);
    ROUNDP512(tmp_output, tmp, 1, T_shared, tid);
    ROUNDP512(tmp, tmp_output, 2, T_shared, tid);
    ROUNDP512(tmp_output, tmp, 3, T_shared, tid);
    ROUNDP512(tmp, tmp_output, 4, T_shared, tid);
    ROUNDP512(tmp_output, tmp, 5, T_shared, tid);
    ROUNDP512(tmp, tmp_output, 6, T_shared, tid);
    ROUNDP512(tmp_output, tmp, 7, T_shared, tid);
    ROUNDP512(tmp, tmp_output, 8, T_shared, tid);
    ROUNDP512(tmp_output, tmp, 9, T_shared, tid);

    m64_h[tid] = output_transformation_shared[tid] ^ tmp[tid];
    return;
  }
}

/* digest part of a message after identifying variant */
int Transform(u32 *outputTransformation, int outputTransformSize, const u8 *msg, int msglen)
{

  u8 *deviceMessage, *deviceOutputTransform, *deviceTransformationBuffer;
  cudaDeviceSynchronize();
  checkCudaErrors(cudaMalloc((void **)&deviceMessage, msglen));
  checkCudaErrors(cudaMalloc((void **)&deviceTransformationBuffer, msglen)); // to store the q results on device.
  checkCudaErrors(cudaMalloc((void **)&deviceOutputTransform, outputTransformSize));
  checkCudaErrors(cudaMemcpy(deviceMessage, msg, msglen, cudaMemcpyHostToDevice));
  checkCudaErrors(cudaMemcpy(deviceOutputTransform, outputTransformation, outputTransformSize, cudaMemcpyHostToDevice));

  struct cudaDeviceProp prop;
  cudaGetDeviceProperties(&prop, 0);

  dim3 dimBlock(32, 1, 1); // should always be a multiple of 8 so that we can process 8 64bit elements message blocks within cuda blocks. Cannot be greater than 512
  dim3 dimGrid(32, 1, 1);

  getLastCudaError();

  for (int i = 0; i < 1; i++)
  {
    cudaDeviceSynchronize();
    cudaEvent_t start, stop;
    cudaEventCreate(&start);
    cudaEventCreate(&stop);

    cudaEventRecord(start);

    Transform512<<<dimGrid, dimBlock>>>((u32 *)deviceOutputTransform, outputTransformSize, (u8 *)deviceMessage, msglen, deviceTransformationBuffer);

    cudaEventRecord(stop);
    cudaEventSynchronize(stop);
    float milliseconds = 0;
    cudaEventElapsedTime(&milliseconds, start, stop);
    printf("Execution time: %f milliseconds\n", milliseconds);

    cudaEventDestroy(start);
    cudaEventDestroy(stop);

    cudaDeviceSynchronize();
    getLastCudaError();
  }
  // gettimeofday(&start, NULL);
  checkCudaErrors(cudaMemcpy(outputTransformation, deviceOutputTransform, outputTransformSize, cudaMemcpyDeviceToHost));
  checkCudaErrors(cudaFree(deviceTransformationBuffer));
  checkCudaErrors(cudaFree(deviceMessage));
  checkCudaErrors(cudaFree(deviceOutputTransform));
  cudaDeviceSynchronize();
  return 1;
}

/* initialise context */
HashReturn Init(hashState *ctx,
                int hashbitlen)
{
  /* output size (in bits) must be a positive integer less than or
     equal to 512, and divisible by 8 */
  if (hashbitlen <= 0 || (hashbitlen % 8) || hashbitlen > 512)
    return BAD_HASHLEN;

  /* set number of state columns and state size depending on
     variant */
  if (hashbitlen <= 256)
  {
    ctx->columns = COLS512;
    ctx->statesize = SIZE512;
  }
  else
  {
    // ctx->columns = COLS1024;
    // ctx->statesize = SIZE1024;
  }

  /* set other variables */
  ctx->hashbitlen = hashbitlen;
  ctx->block_counter = 0;

  return SUCCESS;
}

/* update state with databitlen bits of input */
HashReturn Update(hashState *ctx,
                  const BitSequence *input,
                  DataLength databitlen, u32 *transformedOutput)
{
  int index = 0;
  const int msglen = (int)(databitlen / 8);
  int newMsgLen = msglen;
  int rem = (int)(databitlen % 8);
  uchar *byteInput = (uchar *)input;

  ctx->block_counter = msglen / ctx->statesize;
  byteInput[newMsgLen] = 0x80;
  newMsgLen++;

  const int remainder = (newMsgLen) % ctx->statesize;
  int remainderIndex = remainder;
  /* store remaining data in buffer */
  if (remainderIndex > ctx->statesize - LENGTHFIELDLEN)
  {
    // extra buffer
    while (remainderIndex < ctx->statesize)
    {
      byteInput[newMsgLen] = 0;
      remainderIndex++;
      newMsgLen++;
    }
    // newMsgLen = newMsgLen + (ctx->statesize - remainder);
    remainderIndex = 0;
    ctx->block_counter++;
  }

  while (remainderIndex < ctx->statesize - LENGTHFIELDLEN)
  {
    byteInput[newMsgLen] = 0;
    remainderIndex++;
    newMsgLen++;
  }
  ctx->block_counter++;

  // byteInput[newMsgLen + (remainderIndex -1 )] = (u8)ctx->block_counter;
  newMsgLen += LENGTHFIELDLEN;

  int lengthPad = LENGTHFIELDLEN;
  int lengthPadIndex = 1;
  while (lengthPadIndex <= LENGTHFIELDLEN)
  {
    byteInput[newMsgLen - lengthPadIndex] = (u8)ctx->block_counter;
    lengthPadIndex++;
    ctx->block_counter >>= 8;
  }

  Transform(transformedOutput, ctx->statesize, input, newMsgLen);
  return SUCCESS;
}

HashReturn Final(hashState *ctx, u32 *input,
                 BitSequence *output)
{
  int i, j = 0, hashbytelen = ctx->hashbitlen / 8;
  u8 *s = (u8 *)input;

  /* store hash result in output */
  for (i = ctx->statesize - hashbytelen; i < ctx->statesize; i++, j++)
  {
    output[j] = s[i];
  }

  /* zeroise relevant variables and deallocate memory */
  for (i = 0; i < ctx->columns; i++)
  {
    input[i] = 0;
  }
  return SUCCESS;
}

/* hash bit sequence */
HashReturn Hash(int hashbitlen,
                const BitSequence *data,
                DataLength databitlen,
                BitSequence *hashval)
{
  HashReturn ret;
  hashState context;

  /* initialise */
  if ((ret = Init(&context, hashbitlen)) != SUCCESS)
    return ret;

  u32 *transformedOutput = (u32 *)calloc(context.statesize, 1);
  /* allocate memory for state and data buffer */
  transformedOutput[2 * context.columns - 1] = U32BIG((u32)context.hashbitlen);

  /* process message */
  if ((ret = Update(&context, data, databitlen, transformedOutput)) != SUCCESS)
    return ret;

  /* finalise */
  ret = Final(&context, transformedOutput, hashval);

  free(transformedOutput);

  return ret;
}

void PrintHash(const BitSequence *hash,
               int hashbitlen)
{
  int i;
  for (i = 0; i < hashbitlen / 8; i++)
  {
    printf("%02x", hash[i]);
  }
  printf("\n");
}

// /* eBash API */
#define crypto_hash_BYTES 32u
#ifdef crypto_hash_BYTES
int crypto_hash(unsigned char *out, const unsigned char *in, u64 inlen)
{
  if (Hash(256, in, inlen * 8, out) == SUCCESS)
    return 0;
  return -1;
}
#endif

void printHexArray(unsigned char *array, uint size)
{
  int i;
  for (i = 0; i < size; i++)
    printf("%02x", array[i]);
  printf("\n");
}

int main(int argc, char **argv)
{
  uint *ct, *pt;
  ct = (uint *)malloc(8 * sizeof(uint)); // Allocating memory for 8 uints

  int dataSize; // Total data size
  size_t maxSharedMemory;
  FILE *file = fopen("text_generator/pt_1MB.txt", "r");

  if (file == NULL)
  {
    printf("Error opening the file.\n");
    return -1;
  }

  fseek(file, 0, SEEK_END);
  dataSize = ftell(file);
  fseek(file, 0, SEEK_SET);

  // Host array
  unsigned char *hostData = (unsigned char *)malloc(dataSize + (SIZE512 * 2));
  if (hostData == NULL)
  {
    printf("Error allocating memory.\n");
    fclose(file);
    return -1;
  }

  fread(hostData, sizeof(unsigned char), dataSize, file);
  fclose(file);

  // const char *message = "my message gdfjhghjkfdhgjklfdshgjklfdhgjkfdshkfjsdhgjfdlshgjkfdsghfjdklhgjfkdlghfjdkslhgfdjksgsdfhj    dsdscxcd3232322cc";
  // size_t size = strlen(message);

  // unsigned char* data = (unsigned char*)malloc(size + (SIZE512 * 2));
  // memcpy(data, message, size);
  // crypto_hash((uchar*)ct, data, size);

  printf("Data: %s\n", hostData);
  printf("Size: %zu\n", dataSize);
  crypto_hash((uchar *)ct, hostData, dataSize);

  printHexArray((uchar *)ct, 32);
  printf("done done\n");
  return 1;
}

#else
#error "MMX instructions must be enabled"
#endif /* __MMX__ */
