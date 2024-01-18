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
#include <iostream>
#include <time.h>
#include <sys/time.h>

// void OutputTransformation512(u32 *outputTransformation, u64* T_shared);

#define checkCudaErrors(val) check((val), #val, __FILE__, __LINE__)
// #define getLastCudaError(msg) __getLastCudaError(msg, __FILE__, __LINE__)

void check(cudaError_t err, const char* const func, const char* const file,
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



#define CHECK_LAST_CUDA_ERROR() checkLast(__FILE__, __LINE__)
void checkLast(const char* const file, const int line)
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


__device__ void ROUNDP512_function(u64* m_in, u64* m, int r, u64* T_shared, int shared_message_index_64   ){

    u64* T_m64 = (u64*)T_shared;						
    u64 zero;	
    int start_message_index = shared_message_index_64 / COLS512;  //need to find the start offset for the current message block
    u32* x = (u32*)&m_in[start_message_index];
    zero = 0;						
    int local_message_block_index_32 = shared_message_index_64 % COLS512; 
    
    switch	(local_message_block_index_32){
      case 5 :
          m[shared_message_index_64] = T_m64[3*256+EXT_BYTE(x[ 0],3)] ^zero;		
          break;
      case 3:
        m[shared_message_index_64] = T_m64[5*256+EXT_BYTE(x[ 1],1)] ^ zero;		
        break;
      case 1:
        m[shared_message_index_64] = T_m64[7*256+EXT_BYTE(x[ 1],3)] ^ zero;		
        break;
      case 7:
        m[shared_message_index_64] = T_m64[1*256+EXT_BYTE(x[ 0],1)] ^ zero;		
        break;
      case 6:
        m[shared_message_index_64] = T_m64[2*256+EXT_BYTE(x[ 0],2)] ^ zero;		
        break;
      case 4:
        m[shared_message_index_64] = T_m64[4*256+EXT_BYTE(x[ 1],0)] ^ zero;		
        break;
      case 2:
      m[shared_message_index_64] = T_m64[6*256+EXT_BYTE(x[ 1],2)] ^ zero;		
        break;
      case 0:
        m[shared_message_index_64] = T_m64[0*256+((r)^EXT_BYTE(x[ 0],0))] ^ zero; 
        break;
   }
   __syncthreads();
    switch	(local_message_block_index_32){
      case 5 :
          m[shared_message_index_64] = T_m64[2*256+EXT_BYTE(x[14],2)] ^m[shared_message_index_64];		
          break;
      case 3:
        m[shared_message_index_64] = T_m64[4*256+EXT_BYTE(x[15],0)] ^ m[shared_message_index_64];		
        break;
      case 1:
        m[shared_message_index_64] = T_m64[6*256+EXT_BYTE(x[15],2)] ^ m[shared_message_index_64];		
        break;
      case 7:
        m[shared_message_index_64] = T_m64[0*256+((r)^0x70^EXT_BYTE(x[14],0))] ^ m[shared_message_index_64];		
        break;
      case 6:
        m[shared_message_index_64] = T_m64[1*256+EXT_BYTE(x[14],1)] ^ m[shared_message_index_64];		
        break;
      case 4:
        m[shared_message_index_64] = T_m64[3*256+EXT_BYTE(x[14],3)] ^ m[shared_message_index_64];		
        break;
      case 2:
      m[shared_message_index_64] = T_m64[5*256+EXT_BYTE(x[15],1)] ^ m[shared_message_index_64];		
        break;
      case 0:
        m[shared_message_index_64] = T_m64[7*256+EXT_BYTE(x[15],3)] ^ m[shared_message_index_64]; 
        break;
   }

   __syncthreads();
    switch	(local_message_block_index_32){
      case 5 :
          m[shared_message_index_64] = T_m64[1*256+EXT_BYTE(x[12],1)] ^m[shared_message_index_64];		
          break;
      case 3:
        m[shared_message_index_64] = T_m64[3*256+EXT_BYTE(x[12],3)] ^ m[shared_message_index_64];		
        break;
      case 1:
        m[shared_message_index_64] = T_m64[5*256+EXT_BYTE(x[13],1)] ^ m[shared_message_index_64];		
        break;
      case 7:
        m[shared_message_index_64] = T_m64[7*256+EXT_BYTE(x[13],3)] ^ m[shared_message_index_64];		
        break;
      case 6:
        m[shared_message_index_64] = T_m64[0*256+((r)^0x60^EXT_BYTE(x[12],0))] ^ m[shared_message_index_64];		
        break;
      case 4:
        m[shared_message_index_64] = T_m64[2*256+EXT_BYTE(x[12],2)] ^ m[shared_message_index_64];		
        break;
      case 2:
      m[shared_message_index_64] = T_m64[4*256+EXT_BYTE(x[13],0)] ^ m[shared_message_index_64];		
        break;
      case 0:
        m[shared_message_index_64] = T_m64[6*256+EXT_BYTE(x[13],2)] ^ m[shared_message_index_64]; 
        break;
   }

   __syncthreads();
    switch	(local_message_block_index_32){
      case 5 :
          m[shared_message_index_64] = T_m64[0*256+((r)^0x50^EXT_BYTE(x[10],0))] ^m[shared_message_index_64];		
          break;
      case 3:
        m[shared_message_index_64] = T_m64[2*256+EXT_BYTE(x[10],2)] ^ m[shared_message_index_64];		
        break;
      case 1:
        m[shared_message_index_64] = T_m64[4*256+EXT_BYTE(x[11],0)] ^ m[shared_message_index_64];		
        break;
      case 7:
        m[shared_message_index_64] = T_m64[6*256+EXT_BYTE(x[11],2)] ^ m[shared_message_index_64];		
        break;
      case 6:
        m[shared_message_index_64] = T_m64[7*256+EXT_BYTE(x[11],3)] ^ m[shared_message_index_64];		
        break;
      case 4:
        m[shared_message_index_64] = T_m64[1*256+EXT_BYTE(x[10],1)] ^ m[shared_message_index_64];		
        break;
      case 2:
      m[shared_message_index_64] = T_m64[3*256+EXT_BYTE(x[10],3)] ^ m[shared_message_index_64];		
        break;
      case 0:
        m[shared_message_index_64] = T_m64[5*256+EXT_BYTE(x[11],1)] ^ m[shared_message_index_64]; 
        break;
   }

   __syncthreads();
    switch	(local_message_block_index_32){
      case 5 :
          m[shared_message_index_64] = T_m64[7*256+EXT_BYTE(x[ 9],3)] ^m[shared_message_index_64];		
          break;
      case 3:
        m[shared_message_index_64] = T_m64[1*256+EXT_BYTE(x[ 8],1)] ^ m[shared_message_index_64];		
        break;
      case 1:
        m[shared_message_index_64] = T_m64[3*256+EXT_BYTE(x[ 8],3)] ^ m[shared_message_index_64];		
        break;
      case 7:
        m[shared_message_index_64] = T_m64[5*256+EXT_BYTE(x[ 9],1)] ^ m[shared_message_index_64];		
        break;
      case 6:
        m[shared_message_index_64] = T_m64[6*256+EXT_BYTE(x[ 9],2)] ^ m[shared_message_index_64];		
        break;
      case 4:
        m[shared_message_index_64] = T_m64[0*256+((r)^0x40^EXT_BYTE(x[ 8],0))] ^ m[shared_message_index_64];		
        break;
      case 2:
      m[shared_message_index_64] = T_m64[2*256+EXT_BYTE(x[ 8],2)] ^ m[shared_message_index_64];		
        break;
      case 0:
        m[shared_message_index_64] = T_m64[4*256+EXT_BYTE(x[ 9],0)] ^ m[shared_message_index_64]; 
        break;
   }
    
   __syncthreads();
    switch	(local_message_block_index_32){
      case 5 :
          m[shared_message_index_64] = T_m64[6*256+EXT_BYTE(x[ 7],2)] ^m[shared_message_index_64];		
          break;
      case 3:
        m[shared_message_index_64] = T_m64[0*256+((r)^0x30^EXT_BYTE(x[ 6],0))] ^ m[shared_message_index_64];		
        break;
      case 1:
        m[shared_message_index_64] = T_m64[2*256+EXT_BYTE(x[ 6],2)] ^ m[shared_message_index_64];		
        break;
      case 7:
        m[shared_message_index_64] = T_m64[4*256+EXT_BYTE(x[ 7],0)] ^ m[shared_message_index_64];		
        break;
      case 6:
        m[shared_message_index_64] = T_m64[5*256+EXT_BYTE(x[ 7],1)] ^ m[shared_message_index_64];		
        break;
      case 4:
        m[shared_message_index_64] = T_m64[7*256+EXT_BYTE(x[ 7],3)] ^ m[shared_message_index_64];		
        break;
      case 2:
      m[shared_message_index_64] = T_m64[1*256+EXT_BYTE(x[ 6],1)] ^ m[shared_message_index_64];		
        break;
      case 0:
        m[shared_message_index_64] = T_m64[3*256+EXT_BYTE(x[ 6],3)] ^ m[shared_message_index_64]; 
        break;
   }

   __syncthreads();
    switch	(local_message_block_index_32){
      case 5 :
          m[shared_message_index_64] = T_m64[5*256+EXT_BYTE(x[ 5],1)] ^m[shared_message_index_64];		
          break;
      case 3:
        m[shared_message_index_64] = T_m64[7*256+EXT_BYTE(x[ 5],3)] ^ m[shared_message_index_64];		
        break;
      case 1:
        m[shared_message_index_64] = T_m64[1*256+EXT_BYTE(x[ 4],1)] ^ m[shared_message_index_64];		
        break;
      case 7:
        m[shared_message_index_64] = T_m64[3*256+EXT_BYTE(x[ 4],3)] ^ m[shared_message_index_64];		
        break;
      case 6:
        m[shared_message_index_64] = T_m64[4*256+EXT_BYTE(x[ 5],0)] ^ m[shared_message_index_64];		
        break;
      case 4:
        m[shared_message_index_64] = T_m64[6*256+EXT_BYTE(x[ 5],2)] ^ m[shared_message_index_64];		
        break;
      case 2:
      m[shared_message_index_64] = T_m64[0*256+((r)^0x20^EXT_BYTE(x[ 4],0))] ^ m[shared_message_index_64];		
        break;
      case 0:
        m[shared_message_index_64] = T_m64[2*256+EXT_BYTE(x[ 4],2)] ^ m[shared_message_index_64]; 
        break;
   }

   __syncthreads();
    switch	(local_message_block_index_32){
      case 5 :
          m[shared_message_index_64] = T_m64[4*256+EXT_BYTE(x[ 3],0)] ^m[shared_message_index_64];		
          break;
      case 3:
        m[shared_message_index_64] = T_m64[6*256+EXT_BYTE(x[ 3],2)] ^ m[shared_message_index_64];		
        break;
      case 1:
        m[shared_message_index_64] = T_m64[0*256+((r)^0x10^EXT_BYTE(x[ 2],0))] ^ m[shared_message_index_64];		
        break;
      case 7:
        m[shared_message_index_64] = T_m64[2*256+EXT_BYTE(x[ 2],2)] ^ m[shared_message_index_64];		
        break;
      case 6:
        m[shared_message_index_64] = T_m64[3*256+EXT_BYTE(x[ 2],3)] ^ m[shared_message_index_64];		
        break;
      case 4:
        m[shared_message_index_64] = T_m64[5*256+EXT_BYTE(x[ 3],1)] ^ m[shared_message_index_64];		
        break;
      case 2:
      m[shared_message_index_64] = T_m64[7*256+EXT_BYTE(x[ 3],3)] ^ m[shared_message_index_64];		
        break;
      case 0:
        m[shared_message_index_64] = T_m64[1*256+EXT_BYTE(x[ 2],1)] ^ m[shared_message_index_64]; 
        break;
   }

   __syncthreads();

}


/* compute a round of P512 */
// #define ROUNDP512(m_in, m, r, T_shared) do {					\
//     u64* T_m64 = (u64*)T_shared;						\
//     u64 zero;								\
//     u32* x = (u32*)m_in;						\
//     zero = 0;						\
//   } while (0)

/* compute a round of Q512 */

__device__ void ROUNDQ512_function(u64* m_in, u64* m, int r, u64* T_shared, int shared_message_index_64   ){

    u64* T_m64 = (u64*)T_shared;						
    u64 zero;								
    u64 ff;						
    int start_message_index = shared_message_index_64 / COLS512;  //need to find the start offset for the current message block
    u32* x = (u32*)&m_in[start_message_index];
    zero = 0;						
    ff   = -1;				
    int local_message_block_index_32 = shared_message_index_64 % COLS512; 
									
    m_in[shared_message_index_64] = (m_in[shared_message_index_64]^ff);		// even is for shared_message_index_64 64 bits array, we can use it in the 2 bit array in this case
    // m_in[1] = (m_in[1]^ff);					
    // m_in[2] = (m_in[2]^ff);					
    // m_in[3] = (m_in[3]^ff);					
    // m_in[4] = (m_in[4]^ff);					
    // m_in[5] = (m_in[5]^ff);					
    // m_in[6] = (m_in[6]^ff);					
    // m_in[7] = (m_in[7]^ff);					
    if (local_message_block_index_32 == 7) {
      m[shared_message_index_64] = T_m64[0*256+EXT_BYTE(x[ 0],0)] ^ zero;	
    }
    else if (local_message_block_index_32 == 5) {
      m[shared_message_index_64] = T_m64[1*256+EXT_BYTE(x[ 0],1)] ^ zero;		
    }
    else if (local_message_block_index_32 == 3) {
      m[shared_message_index_64] = T_m64[2*256+EXT_BYTE(x[ 0],2)] ^ zero;		
    }
    else if (local_message_block_index_32 == 1) {
      m[shared_message_index_64] = T_m64[3*256+EXT_BYTE(x[ 0],3)] ^ zero;		
    }
    else if (local_message_block_index_32 == 0) {
      m[shared_message_index_64] = T_m64[4*256+EXT_BYTE(x[ 1],0)] ^ zero;		
    }
    else if (local_message_block_index_32 == 6) {
      m[shared_message_index_64] = T_m64[5*256+EXT_BYTE(x[ 1],1)] ^ zero;		
    }
    else if (local_message_block_index_32 == 4) {
      m[shared_message_index_64] = T_m64[6*256+EXT_BYTE(x[ 1],2)] ^ zero;		
    }
    else if (local_message_block_index_32 == 2) {
      m[shared_message_index_64] = T_m64[7*256+((r)^EXT_BYTE(x[ 1],3))] ^ zero;	
    }

    __syncthreads();

    if (local_message_block_index_32 == 7) {
      m[shared_message_index_64] = T_m64[4*256+EXT_BYTE(x[15],0)] ^ m[shared_message_index_64];		
    }
    else if (local_message_block_index_32 == 5) {
      m[shared_message_index_64] = T_m64[5*256+EXT_BYTE(x[15],1)] ^ m[shared_message_index_64];		
    }
    else if (local_message_block_index_32 == 3) {
      m[shared_message_index_64] = T_m64[6*256+EXT_BYTE(x[15],2)] ^ m[shared_message_index_64];		
    }
    else if (local_message_block_index_32 == 1) {
       m[shared_message_index_64] = T_m64[7*256+((r)^0x70^EXT_BYTE(x[15],3))] ^ m[shared_message_index_64]; 
    }
    else if (local_message_block_index_32 == 0) {
      m[shared_message_index_64] = T_m64[3*256+EXT_BYTE(x[14],3)] ^ m[shared_message_index_64];		
    }
    else if (local_message_block_index_32 == 6) {
      m[shared_message_index_64] = T_m64[0*256+EXT_BYTE(x[14],0)] ^ m[shared_message_index_64];		
    }
    else if (local_message_block_index_32 == 4) {
      m[shared_message_index_64] = T_m64[1*256+EXT_BYTE(x[14],1)] ^ m[shared_message_index_64];		
    }
    else if (local_message_block_index_32 == 2) {
      m[shared_message_index_64] = T_m64[2*256+EXT_BYTE(x[14],2)] ^ m[shared_message_index_64];		
    }

    __syncthreads();

   switch	(local_message_block_index_32){
      case 5 :
        m[shared_message_index_64] = T_m64[0*256+EXT_BYTE(x[12],0)] ^ m[shared_message_index_64];		
        break;
    case 3:
      m[shared_message_index_64] = T_m64[1*256+EXT_BYTE(x[12],1)] ^ m[shared_message_index_64];		
      break;
    case 1:
      m[shared_message_index_64] = T_m64[2*256+EXT_BYTE(x[12],2)] ^ m[shared_message_index_64];		
      break;
    case 7:
      m[shared_message_index_64] = T_m64[3*256+EXT_BYTE(x[12],3)] ^ m[shared_message_index_64];		
      break;
    case 6:
      m[shared_message_index_64] = T_m64[4*256+EXT_BYTE(x[13],0)] ^ m[shared_message_index_64];		
      break;
    case 4:
      m[shared_message_index_64] = T_m64[5*256+EXT_BYTE(x[13],1)] ^ m[shared_message_index_64];		
      break;
    case 2:
    m[shared_message_index_64] = T_m64[6*256+EXT_BYTE(x[13],2)] ^ m[shared_message_index_64];		
      break;
    case 0:
      m[shared_message_index_64] = T_m64[7*256+((r)^0x60^EXT_BYTE(x[13],3))] ^ m[shared_message_index_64]; 
      break;

   }
    __syncthreads();

    switch	(local_message_block_index_32){
      case 5 :
          m[shared_message_index_64] = T_m64[4*256+EXT_BYTE(x[11],0)] ^ m[shared_message_index_64];		
          break;
      case 3:
        m[shared_message_index_64] = T_m64[5*256+EXT_BYTE(x[11],1)] ^ m[shared_message_index_64];		
        break;
      case 1:
        m[shared_message_index_64] = T_m64[6*256+EXT_BYTE(x[11],2)] ^ m[shared_message_index_64];		
        break;
      case 7:
        m[shared_message_index_64] = T_m64[7*256+((r)^0x50^EXT_BYTE(x[11],3))] ^ m[shared_message_index_64];		
        break;
      case 6:
        m[shared_message_index_64] = T_m64[3*256+EXT_BYTE(x[10],3)] ^ m[shared_message_index_64];		
        break;
      case 4:
        m[shared_message_index_64] = T_m64[0*256+EXT_BYTE(x[10],0)] ^ m[shared_message_index_64];		
        break;
      case 2:
      m[shared_message_index_64] = T_m64[1*256+EXT_BYTE(x[10],1)] ^ m[shared_message_index_64];		
        break;
      case 0:
        m[shared_message_index_64] = T_m64[2*256+EXT_BYTE(x[10],2)] ^ m[shared_message_index_64]; 
        break;
   }

									
    __syncthreads();

    switch	(local_message_block_index_32){
      case 5 :
          m[shared_message_index_64] = T_m64[3*256+EXT_BYTE(x[ 8],3)] ^ m[shared_message_index_64];		
          break;
      case 3:
        m[shared_message_index_64] = T_m64[0*256+EXT_BYTE(x[ 8],0)] ^ m[shared_message_index_64];		
        break;
      case 1:
        m[shared_message_index_64] = T_m64[1*256+EXT_BYTE(x[ 8],1)] ^ m[shared_message_index_64];		
        break;
      case 7:
        m[shared_message_index_64] = T_m64[2*256+EXT_BYTE(x[ 8],2)] ^ m[shared_message_index_64];		
        break;
      case 6:
        m[shared_message_index_64] = T_m64[7*256+((r)^0x40^EXT_BYTE(x[ 9],3))] ^ m[shared_message_index_64];		
        break;
      case 4:
        m[shared_message_index_64] = T_m64[4*256+EXT_BYTE(x[ 9],0)] ^ m[shared_message_index_64];		
        break;
      case 2:
      m[shared_message_index_64] = T_m64[5*256+EXT_BYTE(x[ 9],1)] ^ m[shared_message_index_64];		
        break;
      case 0:
        m[shared_message_index_64] = T_m64[6*256+EXT_BYTE(x[ 9],2)] ^ m[shared_message_index_64]; 
        break;
   }
									
																	
    __syncthreads();

    switch	(local_message_block_index_32){
      case 5 :
          m[shared_message_index_64] = T_m64[7*256+((r)^0x30^EXT_BYTE(x[ 7],3))] ^ m[shared_message_index_64];		
          break;
      case 3:
        m[shared_message_index_64] = T_m64[4*256+EXT_BYTE(x[ 7],0)] ^ m[shared_message_index_64];		
        break;
      case 1:
        m[shared_message_index_64] = T_m64[5*256+EXT_BYTE(x[ 7],1)] ^ m[shared_message_index_64];		
        break;
      case 7:
        m[shared_message_index_64] = T_m64[6*256+EXT_BYTE(x[ 7],2)] ^ m[shared_message_index_64];		
        break;
      case 6:
        m[shared_message_index_64] = T_m64[2*256+EXT_BYTE(x[ 6],2)] ^ m[shared_message_index_64];		
        break;
      case 4:
        m[shared_message_index_64] = T_m64[3*256+EXT_BYTE(x[ 6],3)] ^ m[shared_message_index_64];		
        break;
      case 2:
      m[shared_message_index_64] = T_m64[0*256+EXT_BYTE(x[ 6],0)] ^ m[shared_message_index_64];		
        break;
      case 0:
        m[shared_message_index_64] = T_m64[1*256+EXT_BYTE(x[ 6],1)] ^ m[shared_message_index_64]; 
        break;
   }
									
    __syncthreads();

    switch	(local_message_block_index_32){
      case 5 :
          m[shared_message_index_64] = T_m64[2*256+EXT_BYTE(x[ 4],2)] ^ m[shared_message_index_64];		
          break;
      case 3:
        m[shared_message_index_64] = T_m64[3*256+EXT_BYTE(x[ 4],3)] ^ m[shared_message_index_64];		
        break;
      case 1:
        m[shared_message_index_64] = T_m64[0*256+EXT_BYTE(x[ 4],0)] ^ m[shared_message_index_64];		
        break;
      case 7:
        m[shared_message_index_64] = T_m64[1*256+EXT_BYTE(x[ 4],1)] ^ m[shared_message_index_64];		
        break;
      case 6:
        m[shared_message_index_64] = T_m64[6*256+EXT_BYTE(x[ 5],2)] ^ m[shared_message_index_64];		
        break;
      case 4:
        m[shared_message_index_64] = T_m64[7*256+((r)^0x20^EXT_BYTE(x[ 5],3))] ^ m[shared_message_index_64];		
        break;
      case 2:
      m[shared_message_index_64] = T_m64[4*256+EXT_BYTE(x[ 5],0)] ^ m[shared_message_index_64];		
        break;
      case 0:
        m[shared_message_index_64] = T_m64[5*256+EXT_BYTE(x[ 5],1)] ^ m[shared_message_index_64]; 
        break;
   }
									
    __syncthreads();

    switch	(local_message_block_index_32){
      case 5 :
          m[shared_message_index_64] = T_m64[6*256+EXT_BYTE(x[ 3],2)] ^ m[shared_message_index_64];		
          break;
      case 3:
        m[shared_message_index_64] = T_m64[7*256+((r)^0x10^EXT_BYTE(x[ 3],3))] ^ m[shared_message_index_64];		
        break;
      case 1:
        m[shared_message_index_64] = T_m64[4*256+EXT_BYTE(x[ 3],0)] ^ m[shared_message_index_64];		
        break;
      case 7:
        m[shared_message_index_64] = T_m64[5*256+EXT_BYTE(x[ 3],1)] ^ m[shared_message_index_64];		
        break;
      case 6:
        m[shared_message_index_64] = T_m64[1*256+EXT_BYTE(x[ 2],1)] ^ m[shared_message_index_64];		
        break;
      case 4:
        m[shared_message_index_64] = T_m64[2*256+EXT_BYTE(x[ 2],2)] ^ m[shared_message_index_64];		
        break;
      case 2:
      m[shared_message_index_64] = T_m64[3*256+EXT_BYTE(x[ 2],3)] ^ m[shared_message_index_64];		
        break;
      case 0:
        m[shared_message_index_64] = T_m64[0*256+EXT_BYTE(x[ 2],0)] ^ m[shared_message_index_64]; 
        break;
   }
									
    __syncthreads();

}

// #define ROUNDQ512(m_in, m, r, T_shared, sharedMessageIndex ) do {				\	
//     u64* T_m64 = (u64*)T_shared;						\
//     u64 zero;								\
//     u64 ff;								\
//     u32* x = (u32*)m_in;						\
//     zero = 0;						\
//     ff   = -1;				\
//     int localMessageBlockIndex = sharedMessageIndex % COLS512; \
// 									\
//     m_in[localMessageBlockIndex] = (m_in[localMessageBlockIndex]^ff);					\
//     // m_in[1] = (m_in[1]^ff);					\
//     // m_in[2] = (m_in[2]^ff);					\
//     // m_in[3] = (m_in[3]^ff);					\
//     // m_in[4] = (m_in[4]^ff);					\
//     // m_in[5] = (m_in[5]^ff);					\
//     // m_in[6] = (m_in[6]^ff);					\
//     // m_in[7] = (m_in[7]^ff);					
//     (localMessageBlockIndex == 7) 

//     m[7] = T_m64[0*256+EXT_BYTE(x[ 0],0)] ^ zero;		\
//     m[5] = T_m64[1*256+EXT_BYTE(x[ 0],1)] ^ zero;		\
//     m[3] = T_m64[2*256+EXT_BYTE(x[ 0],2)] ^ zero;		\
//     m[1] = T_m64[3*256+EXT_BYTE(x[ 0],3)] ^ zero;		\
//     m[0] = T_m64[4*256+EXT_BYTE(x[ 1],0)] ^ zero;		\
//     m[6] = T_m64[5*256+EXT_BYTE(x[ 1],1)] ^ zero;		\
//     m[4] = T_m64[6*256+EXT_BYTE(x[ 1],2)] ^ zero;		\
//     m[2] = T_m64[7*256+((r)^EXT_BYTE(x[ 1],3))] ^ zero;	\
// 									\
//     m[6] = T_m64[0*256+EXT_BYTE(x[14],0)] ^ m[6];		\
//     m[4] = T_m64[1*256+EXT_BYTE(x[14],1)] ^ m[4];		\
//     m[2] = T_m64[2*256+EXT_BYTE(x[14],2)] ^ m[2];		\
//     m[0] = T_m64[3*256+EXT_BYTE(x[14],3)] ^ m[0];		\
//     m[7] = T_m64[4*256+EXT_BYTE(x[15],0)] ^ m[7];		\
//     m[5] = T_m64[5*256+EXT_BYTE(x[15],1)] ^ m[5];		\
//     m[3] = T_m64[6*256+EXT_BYTE(x[15],2)] ^ m[3];		\
//     m[1] = T_m64[7*256+((r)^0x70^EXT_BYTE(x[15],3))] ^ m[1]; \
// 									\
//     m[5] = T_m64[0*256+EXT_BYTE(x[12],0)] ^ m[5];		\
//     m[3] = T_m64[1*256+EXT_BYTE(x[12],1)] ^ m[3];		\
//     m[1] = T_m64[2*256+EXT_BYTE(x[12],2)] ^ m[1];		\
//     m[7] = T_m64[3*256+EXT_BYTE(x[12],3)] ^ m[7];		\
//     m[6] = T_m64[4*256+EXT_BYTE(x[13],0)] ^ m[6];		\
//     m[4] = T_m64[5*256+EXT_BYTE(x[13],1)] ^ m[4];		\
//     m[2] = T_m64[6*256+EXT_BYTE(x[13],2)] ^ m[2];		\
//     m[0] = T_m64[7*256+((r)^0x60^EXT_BYTE(x[13],3))] ^ m[0]; \
// 									\
//     m[4] = T_m64[0*256+EXT_BYTE(x[10],0)] ^ m[4];		\
//     m[2] = T_m64[1*256+EXT_BYTE(x[10],1)] ^ m[2];		\
//     m[0] = T_m64[2*256+EXT_BYTE(x[10],2)] ^ m[0];		\
//     m[6] = T_m64[3*256+EXT_BYTE(x[10],3)] ^ m[6];		\
//     m[5] = T_m64[4*256+EXT_BYTE(x[11],0)] ^ m[5];		\
//     m[3] = T_m64[5*256+EXT_BYTE(x[11],1)] ^ m[3];		\
//     m[1] = T_m64[6*256+EXT_BYTE(x[11],2)] ^ m[1];		\
//     m[7] = T_m64[7*256+((r)^0x50^EXT_BYTE(x[11],3))] ^ m[7]; \
// 									\
//     m[3] = T_m64[0*256+EXT_BYTE(x[ 8],0)] ^ m[3];		\
//     m[1] = T_m64[1*256+EXT_BYTE(x[ 8],1)] ^ m[1];		\
//     m[7] = T_m64[2*256+EXT_BYTE(x[ 8],2)] ^ m[7];		\
//     m[5] = T_m64[3*256+EXT_BYTE(x[ 8],3)] ^ m[5];		\
//     m[4] = T_m64[4*256+EXT_BYTE(x[ 9],0)] ^ m[4];		\
//     m[2] = T_m64[5*256+EXT_BYTE(x[ 9],1)] ^ m[2];		\
//     m[0] = T_m64[6*256+EXT_BYTE(x[ 9],2)] ^ m[0];		\
//     m[6] = T_m64[7*256+((r)^0x40^EXT_BYTE(x[ 9],3))] ^ m[6]; \
// 									\
//     m[2] = T_m64[0*256+EXT_BYTE(x[ 6],0)] ^ m[2];		\
//     m[0] = T_m64[1*256+EXT_BYTE(x[ 6],1)] ^ m[0];		\
//     m[6] = T_m64[2*256+EXT_BYTE(x[ 6],2)] ^ m[6];		\
//     m[4] = T_m64[3*256+EXT_BYTE(x[ 6],3)] ^ m[4];		\
//     m[3] = T_m64[4*256+EXT_BYTE(x[ 7],0)] ^ m[3];		\
//     m[1] = T_m64[5*256+EXT_BYTE(x[ 7],1)] ^ m[1];		\
//     m[7] = T_m64[6*256+EXT_BYTE(x[ 7],2)] ^ m[7];		\
//     m[5] = T_m64[7*256+((r)^0x30^EXT_BYTE(x[ 7],3))] ^ m[5]; \
// 									\
//     m[1] = T_m64[0*256+EXT_BYTE(x[ 4],0)] ^ m[1];		\
//     m[7] = T_m64[1*256+EXT_BYTE(x[ 4],1)] ^ m[7];		\
//     m[5] = T_m64[2*256+EXT_BYTE(x[ 4],2)] ^ m[5];		\
//     m[3] = T_m64[3*256+EXT_BYTE(x[ 4],3)] ^ m[3];		\
//     m[2] = T_m64[4*256+EXT_BYTE(x[ 5],0)] ^ m[2];		\
//     m[0] = T_m64[5*256+EXT_BYTE(x[ 5],1)] ^ m[0];		\
//     m[6] = T_m64[6*256+EXT_BYTE(x[ 5],2)] ^ m[6];		\
//     m[4] = T_m64[7*256+((r)^0x20^EXT_BYTE(x[ 5],3))] ^ m[4]; \
// 									\
//     m[0] = T_m64[0*256+EXT_BYTE(x[ 2],0)] ^ m[0];		\
//     m[6] = T_m64[1*256+EXT_BYTE(x[ 2],1)] ^ m[6];		\
//     m[4] = T_m64[2*256+EXT_BYTE(x[ 2],2)] ^ m[4];		\
//     m[2] = T_m64[3*256+EXT_BYTE(x[ 2],3)] ^ m[2];		\
//     m[1] = T_m64[4*256+EXT_BYTE(x[ 3],0)] ^ m[1];		\
//     m[7] = T_m64[5*256+EXT_BYTE(x[ 3],1)] ^ m[7];		\
//     m[5] = T_m64[6*256+EXT_BYTE(x[ 3],2)] ^ m[5];		\
//     m[3] = T_m64[7*256+((r)^0x10^EXT_BYTE(x[ 3],3))] ^ m[3]; \
//   } while (0)



// typedef struct {
//     // hashState *ctx;        // Pointer to the hash state
//     const u8 *msg_block;   // Pointer to the current message block
//     u64 *resultBlock ;
//     pthread_t thread_id;        // Thread ID
//     // ... any other arguments needed for processing ...
// } ThreadArgs;


// ThreadArgs *setup_thread_args(ThreadArgs *args,  const u8 *msg_block) {
//     if (msg_block == NULL) {
//         // Handle memory allocation failure
//         printf('msg block nill');
//     }
//     // args->ctx = ctx;
//     args->msg_block = msg_block;
//     args-> resultBlock = malloc(COLS512 * sizeof(u64));
//     return args;
// }


// void ProcessBlock(ThreadArgs* args) {
//     u64 *m64_m = args->resultBlock, tmp[COLS512];
//     u64 *msg_64 = (u64*)args->msg_block;

//     for (int i = 0; i < COLS512; i++) {
//       m64_m[i] = msg_64[i];
//       // m64_hm[i] = m64_h[i] ^ m64_m[i];
//     }

//     // Perform the ROUNDQ512 operations
//     ROUNDQ512(m64_m, tmp, 0);
//     ROUNDQ512(tmp, m64_m, 1);
//     ROUNDQ512(m64_m, tmp, 2);
//     ROUNDQ512(tmp, m64_m, 3);
//     ROUNDQ512(m64_m, tmp, 4);
//     ROUNDQ512(tmp, m64_m, 5);
//     ROUNDQ512(m64_m, tmp, 6);
//     ROUNDQ512(tmp, m64_m, 7);
//     ROUNDQ512(m64_m, tmp, 8);
//     ROUNDQ512(tmp, m64_m, 9);

//     // ... and so on for the rest of the ROUNDQ512 calls ...

//     // // Final combination steps (similar to the end of the while loop in Transform512)
//     // for (int i = 0; i < COLS512; i++) {
//     //     // ... Same combination steps as in Transform512 ...
//     // }
// }

/* apply the output transformation after identifying variant */
// __device__ void OutputTransformation(u32 *output, u64 *T_shared) {
//     OutputTransformation512(output, T_shared);
// }


/* digest part of a message in short variants */
__global__ void Transform512(u32 *outputTransformation, int outputTransformSize, const u8 *msg, int msglen, const u8*  msg_transformation_buffer)  {

   printf("Transform512\n");
   return;
   
   int i;
  __shared__ u64 m64_m[COLS512];
  __shared__ u64 *m64_h;
  __shared__ u64 m64_hm[COLS512];
  __shared__ u64 tmp[COLS512];
  __shared__ u64 tmp_output[COLS512];
  __shared__ u64 output_transformation_shared[COLS512];

  // u8 *tmp_msg_transform;
  // if (blockIdx.x && threadId.x == 0) {
  //     tmp_msg_transform = (u8*)malloc(msglen);
  // }

  __syncthreads(); // Synchronize to ensure the allocation is done

  int threadsPerBlock = blockDim.x;
  int totalLoads = 2048 / threadsPerBlock; // Each thread loads this many bytes

  int sharedMemoryLoadIndex = threadIdx.x * totalLoads;
  int end = sharedMemoryLoadIndex + totalLoads;

  #define TABLE_SIZE 2048
  __shared__ u64 T_shared[TABLE_SIZE];
  // Load data into shared memory
  for (int runningLoadIndex = sharedMemoryLoadIndex; runningLoadIndex < end; ++runningLoadIndex) {
      // sharedData[runningLoadIndex] = globalData[runningLoadIndex];
      if (runningLoadIndex < TABLE_SIZE) { // Ensure we don't go out of bounds of shared memory
        printf("runningLoadIndex:%d\n", runningLoadIndex);
        T_shared[runningLoadIndex] = T[runningLoadIndex];
      }
  }

  // Synchronize to ensure all data is loaded
  __syncthreads();

  int grid_stride_local_index = blockIdx.x * blockDim.x + threadIdx.x; 
  int stride = blockDim.x * gridDim.x;
  u64 *msg_64 = (u64*)msg;

  #define SHARED_Q_RESULTS_SIZE 512 
  
  __shared__ u64 q_shared_results_64[SHARED_Q_RESULTS_SIZE];
  // __shared__ int index_q_shared_results_64;
  __shared__ u64 tmp_shared_64[SHARED_Q_RESULTS_SIZE];
  __shared__ u64 p_shared_results_64[SHARED_Q_RESULTS_SIZE];

  if (blockDim.x > SHARED_Q_RESULTS_SIZE ){
      printf("we have an error, block dimensions cannot be bigger than shared q results size  ");
      return;
  }

  u64* transformation_buffer_q_64 = (u64*)msg_transformation_buffer;
  int msglen_64 = msglen/8; // length of char* data type to  u64* type

  for (int global_index_64 = grid_stride_local_index; global_index_64 < (msglen_64 + stride); global_index_64 += stride) {
      if (global_index_64 < msglen_64){
        int q_local_index_shared_64 = global_index_64 % SHARED_Q_RESULTS_SIZE;
        q_shared_results_64[q_local_index_shared_64] = msg_64[global_index_64];
        // p_shared_results_64[q_local_index_shared_64] = msg_64[global_index_64] ^  ;
        
        ROUNDQ512_function(q_shared_results_64, tmp_shared_64, 0,(u64*) T_shared, q_local_index_shared_64);
        ROUNDQ512_function(tmp_shared_64, q_shared_results_64, 1,(u64*) T_shared, q_local_index_shared_64);
        ROUNDQ512_function(q_shared_results_64, tmp_shared_64, 2,(u64*) T_shared, q_local_index_shared_64);
        ROUNDQ512_function(tmp_shared_64, q_shared_results_64, 3,(u64*) T_shared, q_local_index_shared_64);
        ROUNDQ512_function(q_shared_results_64, tmp_shared_64, 4,(u64*) T_shared, q_local_index_shared_64);
        ROUNDQ512_function(tmp_shared_64, q_shared_results_64, 5,(u64*) T_shared, q_local_index_shared_64);
        ROUNDQ512_function(q_shared_results_64, tmp_shared_64, 6,(u64*) T_shared, q_local_index_shared_64);
        ROUNDQ512_function(tmp_shared_64, q_shared_results_64, 7,(u64*) T_shared, q_local_index_shared_64);
        ROUNDQ512_function(q_shared_results_64, tmp_shared_64, 8,(u64*) T_shared, q_local_index_shared_64);
        ROUNDQ512_function(tmp_shared_64, q_shared_results_64, 9,(u64*) T_shared, q_local_index_shared_64);

        transformation_buffer_q_64[global_index_64] = q_shared_results_64[q_local_index_shared_64];
      }
  }

  // thread coarsening to 8 total threads
  if (blockIdx.x == 0 && threadIdx.x < 8){
    int threadId = threadIdx.x;
    m64_h = (u64*)outputTransformation;
    output_transformation_shared[threadId] = outputTransformation[threadId];

    while (msglen >= SIZE512) {
        msg_64 = (u64*)msg;

        // for (i = 0; i < COLS512; i++) {
          // printf("i:%d\n", i);
          // m64_m[i] = msg_64[i];
        // q_shared_results_64[threadId] = msg_64[threadId];
        m64_hm[threadId] = output_transformation_shared[threadId] ^ msg_64[threadId];
        // }

        ROUNDP512_function(m64_hm, tmp, 0, T_shared, threadId);
        ROUNDP512_function(tmp, m64_hm, 1, T_shared, threadId);
        ROUNDP512_function(m64_hm, tmp, 2, T_shared, threadId);
        ROUNDP512_function(tmp, m64_hm, 3, T_shared, threadId);
        ROUNDP512_function(m64_hm, tmp, 4, T_shared, threadId);
        ROUNDP512_function(tmp, m64_hm, 5, T_shared, threadId);
        ROUNDP512_function(m64_hm, tmp, 6, T_shared, threadId);
        ROUNDP512_function(tmp, m64_hm, 7, T_shared, threadId);
        ROUNDP512_function(m64_hm, tmp, 8, T_shared, threadId);
        ROUNDP512_function(tmp, m64_hm, 9, T_shared, threadId);

        // ROUNDQ512(m64_m, tmp, 0, T_shared);
        // ROUNDQ512(tmp, m64_m, 1, T_shared);
        // ROUNDQ512(m64_m, tmp, 2, T_shared);
        // ROUNDQ512(tmp, m64_m, 3, T_shared);
        // ROUNDQ512(m64_m, tmp, 4, T_shared);
        // ROUNDQ512(tmp, m64_m, 5, T_shared);
        // ROUNDQ512(m64_m, tmp, 6, T_shared);
        // ROUNDQ512(tmp, m64_m, 7, T_shared);
        // ROUNDQ512(m64_m, tmp, 8, T_shared);
        // ROUNDQ512(tmp, m64_m, 9, T_shared);
      
        // m64_m = threads_args[block_index].resultBlock;
        
        // for (i = 0; i < COLS512; i++) {
        output_transformation_shared[threadId] = output_transformation_shared[threadId] ^  msg_transformation_buffer [msglen/8 + threadId];
        output_transformation_shared[threadId] = output_transformation_shared[threadId] ^ m64_hm[threadId];
        // }
        // for (i = 0; i < COLS512; i++) {
        //   m64_h[i] = m64_h[i] ^ threads_args[block_index].resultBlock[i];
        //   m64_h[i] = m64_h[i] ^ m64_hm[i];
        // }

        msg += SIZE512;
        msglen -= SIZE512;    
        // block_index++;

      }

      __syncthreads();

      // OutputTransformation(output_transformation_shared, (u64 *)T_shared);

      // int i;
      // u64 *m64_h, tmp1[COLS512], tmp2[COLS512];
      // m64_h = (u64*)outputTransformation;

      // for (i = 0; i < COLS512; i++) {
      tmp[threadId] = output_transformation_shared[threadId];
      // }

      ROUNDP512_function(tmp, tmp_output, 0, T_shared, threadId);
      ROUNDP512_function(tmp_output, tmp, 1, T_shared, threadId);
      ROUNDP512_function(tmp, tmp_output, 2, T_shared, threadId);
      ROUNDP512_function(tmp_output, tmp, 3, T_shared, threadId);
      ROUNDP512_function(tmp, tmp_output, 4, T_shared, threadId);
      ROUNDP512_function(tmp_output, tmp, 5, T_shared, threadId);
      ROUNDP512_function(tmp, tmp_output, 6, T_shared, threadId);
      ROUNDP512_function(tmp_output, tmp, 7, T_shared, threadId);
      ROUNDP512_function(tmp, tmp_output, 8, T_shared, threadId);
      ROUNDP512_function(tmp_output, tmp, 9, T_shared, threadId);

      // for (i = 0; i < COLS512; i++) {
      m64_h[threadId] = output_transformation_shared[threadId] ^ tmp[threadId];
      // }

      return;
  }

  // __shared__ u64 T_shared[8 * 256];

  // if (tid < 4 * 256) {
  //     T_shared[tid*2] = T[tid*2];
  //     T_shared[tid*2 + 1] = T[tid*2 + 1];
  // }
  // __syncthreads();

  // u64 *T_shared = (u64 *)T;

  // Determine the number of blocks
  // int num_blocks = msglen / SIZE512;
  //  pthread_t threads[num_blocks];
  // ThreadArgs threads_args[num_blocks];

  // int block_index = 0;

  // for (block_index = 0; block_index < num_blocks; block_index++) {
  //   // Arguments for ProcessBlock function
  //   setup_thread_args(&threads_args[block_index], msg + (block_index * SIZE512));
  //   pthread_create(&threads_args[block_index].thread_id, NULL, ProcessBlock, &threads_args[block_index]);
  // }

  // // Wait for all threads to complete
  // for (block_index = 0;block_index < num_blocks; block_index++) {
  //   pthread_join(threads_args[block_index].thread_id, NULL);
  // }

  // block_index = 0;

}

__global__ void TestTransform(){
  printf("test transform \n");
}

/* digest part of a message after identifying variant */
int Transform(u32 *outputTransformation, int outputTransformSize, const u8 *msg, int msglen) {

    u8 * deviceMessage, *deviceOutputTransform, *deviceTransformationBuffer;
    // uint size = n*sizeof(uint);
    struct timeval start, end;
    u64 usec;

    cudaDeviceSynchronize();
    checkCudaErrors(cudaMalloc((void**)&deviceMessage, msglen));
    checkCudaErrors(cudaMalloc((void**)&deviceTransformationBuffer, msglen)); // to store the q results on device.
    checkCudaErrors(cudaMalloc((void**)&deviceOutputTransform, outputTransformSize));
    gettimeofday(&start, NULL);
    checkCudaErrors(cudaMemcpy(deviceMessage, msg, msglen, cudaMemcpyHostToDevice));
    checkCudaErrors(cudaMemcpy(deviceOutputTransform, outputTransformation, outputTransformSize, cudaMemcpyHostToDevice));
    gettimeofday(&end, NULL);
    usec = (end.tv_sec * 1000000 + end.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec);
    printf("HtoD %lld usec\n", usec);

    struct cudaDeviceProp prop;
    cudaGetDeviceProperties(&prop, 0);

    dim3 dimBlock(512, 1, 1);
    dim3 dimGrid(8, 1, 1);

    //cudaFuncSetCacheConfig(MyKernel, cudaFuncCachePreferShared);
    //cudaFuncSetCacheConfig(AES_encrypt, cudaFuncCachePreferL1);

    // warmup
    // AES_encrypt<<<dimGrid, dimBlock>>>(cpt, cct, ce_sched, Nr, n);

    // debug<<<1, 1>>>();
    getLastCudaError();

    for (int i = 0; i < 1; i++) {
        cudaDeviceSynchronize();
        gettimeofday(&start, NULL);
        // Transform512<<<dimGrid, dimBlock>>>((u32 *)deviceOutputTransform, outputTransformSize, (u8*)deviceMessage, msglen, deviceTransformationBuffer );
        TestTransform<<<dimGrid, dimBlock>>>();
        // AES_encrypt<<<dimGrid, dimBlock>>>(cpt, cct, ce_sched, Nr, n);
        cudaDeviceSynchronize();
        //exit(0);
        getLastCudaError();
        gettimeofday(&end, NULL);
        usec = (end.tv_sec * 1000000 + end.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec);
        u64 size_in_MB = msglen / 1024 / 1024;
        const char *gors = "smem" ;
        printf(" %s %d MB %lld usec %lf Gbps\n", 
                 gors, size_in_MB, usec, 
                ((double)size_in_MB*8/1024) / ((double)usec/1000000));
    }
    gettimeofday(&start, NULL);
    checkCudaErrors(cudaMemcpy(outputTransformation, deviceOutputTransform, outputTransformSize, cudaMemcpyDeviceToHost));
    gettimeofday(&end, NULL);
    usec = (end.tv_sec * 1000000 + end.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec);
    printf("DtoH %lld usec\n", usec);
	
    checkCudaErrors(cudaFree(deviceMessage));
    checkCudaErrors(cudaFree(deviceOutputTransform));
    cudaDeviceSynchronize();
    return 1;

}

// /* apply the output transformation of short variants */
// __device__ void OutputTransformation512(u32 *outputTransformation, u64* T_shared) {
//   int i;
//   u64 *m64_h, tmp1[COLS512], tmp2[COLS512];
//   m64_h = (u64*)outputTransformation;

//   for (i = 0; i < COLS512; i++) {
//     tmp1[i] = m64_h[i];
//   }

//   ROUNDP512(tmp1, tmp2, 0, T_shared);
//   ROUNDP512(tmp2, tmp1, 1, T_shared);
//   ROUNDP512(tmp1, tmp2, 2, T_shared);
//   ROUNDP512(tmp2, tmp1, 3, T_shared);
//   ROUNDP512(tmp1, tmp2, 4, T_shared);
//   ROUNDP512(tmp2, tmp1, 5, T_shared);
//   ROUNDP512(tmp1, tmp2, 6, T_shared);
//   ROUNDP512(tmp2, tmp1, 7, T_shared);
//   ROUNDP512(tmp1, tmp2, 8, T_shared);
//   ROUNDP512(tmp2, tmp1, 9, T_shared);

//   for (i = 0; i < COLS512; i++) {
//     m64_h[i] = m64_h[i] ^ tmp1[i];
//   }
// }



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
  uchar* byteInput = (uchar*)input;

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

  Transform(transformedOutput, ctx->statesize, input, newMsgLen);
  return SUCCESS;
}


/* finalise: process remaining data (including padding), perform
   output transformation, and write hash result to 'output' */
HashReturn Final(hashState* ctx, u32* input,
		 BitSequence* output) {
  int i, j = 0, hashbytelen = ctx->hashbitlen/8;
  u8 *s = (u8*)input;

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
    
  u32* transformedOutput =(u32*) calloc(context.statesize,1);
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
int crypto_hash(unsigned char *out, const unsigned char *in, u64 inlen)
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
    FILE *file = fopen("groestl256.blb", "r");

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
    crypto_hash((uchar*)ct, data, size);

    // printf("Data: %s\n", hostData);
    // printf("Size: %zu\n", dataSize);
    // crypto_hash(ct, hostData, dataSize);

    printHexArray((uchar*)ct, 32);
    printf("done done\n");
    return 1;
}


#else
#error "MMX instructions must be enabled"
#endif /* __MMX__ */
