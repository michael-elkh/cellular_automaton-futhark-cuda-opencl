#include <stdio.h>
#include <time.h>
#include <string.h>

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
	int index = get_index();
	int4 neighbors = get_neighborhood(index, width, length);
	
	dst[index] = src[neighbors.x] ^ src[neighbors.y] ^ src[neighbors.z] ^ src[neighbors.w];
}

// Host
#define THREADS_PER_BLOCK 64

int main(int argc, const char* argv[])
{
	int iteration = 1000, width = 1000, height = 1000;
    if (argc > 1)
    {
        iteration = atoi(argv[1]);
    }else{
        printf("Usage:\n    %s iteration [width] [height]\n", argv[0]);
        exit(1);
    }
    
    if(argc > 2){
        width = atoi(argv[2]);
        height = width;
    }
    else if (argc > 3)
    {
        width = atoi(argv[2]);
        height = atoi(argv[3]);
	}
	
	int length = width * height;

	uint *src, *dst;
	uint *d_src, *d_dst;
	int size = length * sizeof(uint);

	/* allocate space for device copies src and dst */
	cudaMalloc((void **) &d_src, size);
	cudaMalloc((void **) &d_dst, size);

	/* allocate space for host copies of src and dst and setup input values */
	src = (uint *)malloc(size);
	dst = (uint *)malloc(size);

	for(int i = 0; i < length; i++)
	{
		src[i] = i % 2;
	}

	/* copy src to device */
	cudaMemcpy(d_src, src, size, cudaMemcpyHostToDevice);

	struct timespec start, finish;
    double seconds_elapsed = 0.0;

    clock_gettime(CLOCK_MONOTONIC, &start);
	/* launch the kernel on the GPU */
	/* insert the launch parameters to launch the kernel properly using blocks and threads */ 
	uint* tmp;
	for(int i = 0; i < iteration; i++){
		parity_automaton<<<(length + (THREADS_PER_BLOCK - 1)) / THREADS_PER_BLOCK, THREADS_PER_BLOCK>>>(d_src, d_dst, width, length);
		// Swap dst and src
		tmp = d_src;
		d_src = d_dst;
		d_dst = tmp; 
	}
	// At the end the final result is in d_src

	/* copy src back to host */
	cudaMemcpy(src, d_src, size, cudaMemcpyDeviceToHost);
	clock_gettime(CLOCK_MONOTONIC, &finish);
    seconds_elapsed += (double)(finish.tv_sec - start.tv_sec) + (finish.tv_nsec - start.tv_nsec) / 1.0e9;
    printf("Result: %lf\n", seconds_elapsed);

	/* clean up */
	free(src);
	free(dst);
	cudaFree(d_src);
	cudaFree(d_dst);
	
	return EXIT_SUCCESS;
}