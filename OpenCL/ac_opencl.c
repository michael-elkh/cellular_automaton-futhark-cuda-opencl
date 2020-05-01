// Src : https://www.eriksmistad.no/getting-started-with-opencl-and-gpu-computing/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <CL/cl.h>

#define MAX_SOURCE_SIZE (0x100000)
#define uint unsigned int

int main(int argc, const char *argv[])
{
	int iteration = 1000, width = 1000, height = 1000;
	if (argc > 1)
	{
		iteration = atoi(argv[1]);
	}
	else
	{
		printf("Usage:\n    %s iteration [width] [height]\n", argv[0]);
		exit(1);
	}

	if (argc > 2)
	{
		width = atoi(argv[2]);
		height = width;
	}
	else if (argc > 3)
	{
		width = atoi(argv[2]);
		height = atoi(argv[3]);
	}

	int length = width * height;

	// Load the kernel source code into the array source_str
	FILE *fp;
	char *source_str;
	size_t source_size;

	fp = fopen("ac.cl", "r");
	if (!fp)
	{
		fprintf(stderr, "Failed to load kernel.\n");
		exit(1);
	}
	source_str = (char *)malloc(MAX_SOURCE_SIZE);
	source_size = fread(source_str, 1, MAX_SOURCE_SIZE, fp);
	fclose(fp);

	// Get platform and device information
	cl_platform_id platform_id = NULL;
	cl_device_id device_id = NULL;
	cl_uint ret_num_devices;
	cl_uint ret_num_platforms;
	cl_int ret = clGetPlatformIDs(1, &platform_id, &ret_num_platforms);
	ret = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_GPU, 1, &device_id, &ret_num_devices);

	// Create an OpenCL context
	cl_context context = clCreateContext(NULL, 1, &device_id, NULL, NULL, &ret);

	// Create a command queue
	cl_command_queue command_queue = clCreateCommandQueue(context, device_id, 0, &ret);

	// Create memory buffers on the device for each vector
	cl_mem d_src = clCreateBuffer(context, CL_MEM_READ_WRITE, length * sizeof(uint), NULL, &ret);
	cl_mem d_dst = clCreateBuffer(context, CL_MEM_READ_WRITE, length * sizeof(uint), NULL, &ret);

	uint *src, *dst;
	src = malloc(length * sizeof(uint));
	dst = malloc(length * sizeof(uint));

	// Copy the lists A and B to their respective memory buffers
	ret = clEnqueueWriteBuffer(command_queue, d_src, CL_TRUE, 0, length * sizeof(uint), src, 0, NULL, NULL);

	// Create a program from the kernel source
	cl_program program = clCreateProgramWithSource(context, 1, (const char **)&source_str, (const size_t *)&source_size, &ret);

	// Build the program
	ret = clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);

	// Create the OpenCL kernel
	cl_kernel kernel = clCreateKernel(program, "parity_automaton", &ret);

	// Execute the OpenCL kernel on the list
	size_t global_item_size = ((length / 64) + ((length % 64) > 0)) * 64; // Process the entire lists
	size_t local_item_size = 64;										  // Divide work items into groups of 64
	
	struct timespec start, finish;
    double seconds_elapsed = 0.0;

    clock_gettime(CLOCK_MONOTONIC, &start);
	cl_uint2 size = {{ width, height }};
	cl_uint max = 1;
	// Set the arguments of the kernel
	ret = clSetKernelArg(kernel, 2, sizeof(cl_uint2), (void *)&size);
	ret = clSetKernelArg(kernel, 3, sizeof(cl_uint), (void *)&max);
	
	cl_mem tmp;
	for(int i = 0; i < iteration; i++){
		ret = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&d_dst);
		ret = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&d_src);
		// At the end the final result is in d_src
		ret = clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL, &global_item_size, &local_item_size, 0, NULL, NULL);

		// Swap dst and src
		tmp = d_src;
		d_src = d_dst;
		d_dst = tmp; 
	}
	// At the end the final result is in d_src

	/* copy src back to host */
	ret = clEnqueueReadBuffer(command_queue, d_dst, CL_TRUE, 0, length * sizeof(uint), dst, 0, NULL, NULL);
	clFinish(command_queue);
	clock_gettime(CLOCK_MONOTONIC, &finish);
    seconds_elapsed += (double)(finish.tv_sec - start.tv_sec) + (finish.tv_nsec - start.tv_nsec) / 1.0e9;
    printf("Result: %lf\n", seconds_elapsed);

	// Clean up
	ret = clFinish(command_queue);
	ret = clFlush(command_queue);
	ret = clReleaseKernel(kernel);
	ret = clReleaseProgram(program);
	ret = clReleaseMemObject(d_src);
	ret = clReleaseMemObject(d_dst);
	ret = clReleaseCommandQueue(command_queue);
	ret = clReleaseContext(context);
	free(source_str);
	free(src);
	free(dst);	
	return EXIT_SUCCESS;
}