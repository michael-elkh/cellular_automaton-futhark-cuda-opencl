#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdint.h>

//#include "../backend.h"

/* Device */
__inline__ __device__ int get_index(){
    return blockIdx.x * blockDim.x + threadIdx.x;
}

__inline__ __device__ int4 get_neighborhood(int index, int width, int length){
	int4 neighbors;

	int col = index % width;
	
	//left
	neighbors.x = col == 0 ? index + (width - 1) : index - 1;
	//up
	neighbors.y = index < width ? length - (width - index) : index - width; 
	//right
	neighbors.z = col == (width - 1) ? index - (width - 1) : index + 1;
	//down
	neighbors.w = (index + width) >= length ? col /* idx mod width */ : index + width;

	return neighbors;
}

__global__ void parity_automaton(uint* src, uint*dst, int width, int length){
	// Recover the index
	int index = get_index();

	// get direct neighbors of the pixel
	int4 neighbors = get_neighborhood(index, width, length);
	
	dst[index] = src[neighbors.x] ^ src[neighbors.y] ^ src[neighbors.z] ^ src[neighbors.w];
}

// cyclic next state function
__inline__ __device__ uint cyclic(uint center, uint left, uint up, uint right, uint down, uint max){
	uint k1 = (center + 1) % (max + 1);
	return (left == k1) || (up == k1) || (right == k1) || (down == k1) ? k1 : center;
}

__global__ void cyclic_automaton(uint *src, uint *dst, int width, int length, uint max_val) {
	// Recover the index
	int index = get_index();

	// get direct neighbors of the pixel
	int4 neighbors = get_neighborhood(index, width, length);

	// set the pixel value in destination matrix
	dst[index] = cyclic(src[index], src[neighbors.x], src[neighbors.y], src[neighbors.z], src[neighbors.w], max_val);
}

// Host
#define THREADS_PER_BLOCK 64

static uint *d_src = NULL;
static uint *d_dst = NULL;
static int32_t d_width = 0;
static int32_t d_length = 0;
static bool d_parity = true;
static uint32_t d_max_value = 0;
static int32_t size = 0;

extern "C" {
	void init(){}
	void set_args(bool parity, uint32_t *matrix, int32_t width, int32_t length, uint32_t max_value){
		d_parity = parity;
		d_width = width;
		d_length = length;
		d_max_value = max_value;
		size = length * sizeof(*matrix);
		
		if(d_src){
			cudaFree(d_src);
		}
		if(d_dst){
			cudaFree(d_dst);
		}
		/* allocate space for device copies src and dst */
		cudaMalloc((void **) &d_src, size);
		cudaMalloc((void **) &d_dst, size);
		/* copy src to device */
		cudaMemcpy(d_src, matrix, size, cudaMemcpyHostToDevice);
	}
	void iterate(uint32_t iteration){
		/* launch the kernel on the GPU */
		/* insert the launch parameters to launch the kernel properly using blocks and threads */ 
		uint* tmp;
		#pragma unroll 4
		for(int i = 0; i < iteration; i++){
			if(d_parity){
				parity_automaton<<<(d_length + (THREADS_PER_BLOCK - 1)) / THREADS_PER_BLOCK, THREADS_PER_BLOCK>>>(d_src, d_dst, d_width, d_length);
			}else{
				cyclic_automaton<<<(d_length + (THREADS_PER_BLOCK - 1)) / THREADS_PER_BLOCK, THREADS_PER_BLOCK>>>(d_src, d_dst, d_width, d_length, d_max_value);
			}
			// Swap dst and src
			tmp = d_src;
			d_src = d_dst;
			d_dst = tmp; 
		}
		// At the end the final result is in d_src
	}
	void get_result(uint32_t *matrix){
		/* copy src back to host */
		cudaMemcpy(matrix, d_src, size, cudaMemcpyDeviceToHost);
	}
	void destroy(){
		cudaFree(d_src);
		cudaFree(d_dst);
		d_src = d_dst = NULL;
	}
}