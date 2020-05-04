static const char program[] = "// Recover direct neighborhood in vector of dim 4. The domain is periodic.\n\
inline int4 get_neighborhood(int index, int width, int length){\n\
	int4 neighbors;\n\
\n\
	int col = index % width;\n\
	\n\
	//left\n\
	neighbors.x = col == 0 ? index + (width - 1) : index - 1;\n\
	//up\n\
	neighbors.y = index < width ? length - (width - index) : index - width; \n\
	//right\n\
	neighbors.z = col == (width - 1) ? index - (width - 1) : index + 1;\n\
	//down\n\
	neighbors.w = (index + width) >= length ? col /* idx mod width */ : index + width;\n\
\n\
	return neighbors;\n\
}\n\
\n\
__kernel void parity_automaton(__global uint *src, __global uint *dst, int width, int length) {\n\
  // Recover the index\n\
  int index = get_global_id(0);\n\
\n\
  // get direct neighbors of the pixel\n\
  int4 neighbors = get_neighborhood(index, width, length);\n\
\n\
  // update pixel in destination matrix\n\
  dst[index] = src[neighbors.x] ^ src[neighbors.y] ^ src[neighbors.z] ^ src[neighbors.w];\n\
}\n\
\n\
// cyclic next state function\n\
inline uint cyclic(uint center, uint left, uint up, uint right, uint down, uint max){\n\
  uint k1 = (center + 1) % (max + 1);\n\
  return (left == k1) || (up == k1) || (right == k1) || (down == k1) ? k1 : center;\n\
}\n\
\n\
__kernel void cyclic_automaton(__global uint *src, __global uint *dst, int width, int length, uint max_val) {\n\
  // Recover the index\n\
  int index = get_global_id(0);\n\
\n\
  // get direct neighbors of the pixel\n\
  int4 neighbors = get_neighborhood(index, width, length);\n\
\n\
  // set the pixel value in destination matrix\n\
  dst[index] = cyclic(src[index], src[neighbors.x], src[neighbors.y], src[neighbors.z], src[neighbors.w], max_val);\n\
}\n\
";

// Src : https://www.eriksmistad.no/getting-started-with-opencl-and-gpu-computing/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <CL/cl.h>

#include "../backend.h"
#define uint unsigned int
#define UNUSED(x) ((void)(x))
#define LOCAL_GROUP_SIZE 64

typedef struct cl_params
{
	cl_platform_id platform_id;
	cl_device_id device_id;
	cl_uint ret_num_devices;
	cl_uint ret_num_platforms;
	cl_context context;
	cl_command_queue command_queue;
	cl_program program;
	size_t global_item_size;
	size_t local_item_size;
	cl_kernel kernel;
} cl_params_t;

static cl_params_t params;
static cl_mem d_src = NULL;
static cl_mem d_dst = NULL;
static int32_t d_width = 0;
static int32_t d_length = 0;
static bool d_parity = true;
static uint32_t d_max_value = 0;
static int32_t size = 0;

void init()
{
	cl_int ret = clGetPlatformIDs(1, &params.platform_id, &params.ret_num_platforms);
	ret = clGetDeviceIDs(params.platform_id, CL_DEVICE_TYPE_GPU, 1, &params.device_id, &params.ret_num_devices);

	// Create an OpenCL context
	params.context = clCreateContext(NULL, 1, &params.device_id, NULL, NULL, &ret);

	// Create a command queue
	params.command_queue = clCreateCommandQueue(params.context, params.device_id, 0, &ret);
	
	size_t source_size = sizeof(program);
	char* cpy = malloc(source_size);
	strcpy(cpy, program);
	// Create a program from the kernel source
	params.program = clCreateProgramWithSource(params.context, 1, (const char**)&cpy, (const size_t *)&source_size, &ret);
	free(cpy);
	// Build the program
	ret = clBuildProgram(params.program, 1, &params.device_id, NULL, NULL, NULL);
}
void set_args(bool parity, uint32_t *matrix, int32_t width, int32_t length, uint32_t max_value)
{
	d_parity = parity;
	d_width = width;
	d_length = length;
	d_max_value = max_value;
	size = length * sizeof(*matrix);

	cl_int ret;

	// Create memory buffers on the device for each array
	d_src = clCreateBuffer(params.context, CL_MEM_READ_WRITE, size, NULL, &ret);
	d_dst = clCreateBuffer(params.context, CL_MEM_READ_WRITE, size, NULL, &ret);

	// Copy the src array to the device buffer d_src
	ret = clEnqueueWriteBuffer(params.command_queue, d_src, CL_TRUE, 0, size, matrix, 0, NULL, NULL);

	// Create the OpenCL kernel
	params.kernel = clCreateKernel(params.program, d_parity ? "parity_automaton" : "cyclic_automaton", &ret);

	// Execute the OpenCL kernel on the list
	params.global_item_size = ((d_length / LOCAL_GROUP_SIZE) + ((d_length % LOCAL_GROUP_SIZE) > 0)) * LOCAL_GROUP_SIZE; // Process the entire lists
	params.local_item_size = LOCAL_GROUP_SIZE;										  // Divide work items into groups of 64

	ret = clSetKernelArg(params.kernel, 2, sizeof(cl_int), (void *)&d_width);
	ret = clSetKernelArg(params.kernel, 3, sizeof(cl_int), (void *)&d_length);
	if(!parity){
		ret = clSetKernelArg(params.kernel, 4, sizeof(cl_int), (void*)&d_max_value);
	}
}
void iterate(uint32_t iteration)
{
	cl_uint ret;

	cl_mem tmp;
	#pragma GCC unroll (4)
	for (uint32_t i = 0; i < iteration; i++)
	{
		ret = clSetKernelArg(params.kernel, 0, sizeof(cl_mem), (void *)&d_src);
		ret = clSetKernelArg(params.kernel, 1, sizeof(cl_mem), (void *)&d_dst);
		// At the end the final result is in d_src
		ret = clEnqueueNDRangeKernel(params.command_queue, params.kernel, 1, NULL, &params.global_item_size, &params.local_item_size, 0, NULL, NULL);
		// Swap dst and src
		tmp = d_src;
		d_src = d_dst;
		d_dst = tmp;
	}
	// At the end the final result is in d_src
	UNUSED(ret);
}
void get_result(uint32_t *matrix)
{
	/* copy src back to host */
	cl_uint ret = clEnqueueReadBuffer(params.command_queue, d_src, CL_TRUE, 0, size, matrix, 0, NULL, NULL);
	ret = clFlush(params.command_queue);
	ret = clFinish(params.command_queue);
	UNUSED(ret);
}
void destroy()
{
	cl_uint ret = clReleaseKernel(params.kernel);
	ret = clReleaseProgram(params.program);
	ret = clReleaseMemObject(d_src);
	ret = clReleaseMemObject(d_dst);
	ret = clReleaseCommandQueue(params.command_queue);
	ret = clReleaseContext(params.context);
	UNUSED(ret);
}