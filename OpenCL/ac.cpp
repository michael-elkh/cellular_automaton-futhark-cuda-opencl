#define __CL_ENABLE_EXCEPTIONS

#ifdef __APPLE__
#include <OpenCL/cl.hpp>
#else
#include <CL/cl.hpp>
#endif

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <unistd.h>
#include <ctime>
#include <chrono>

#include "Display.h"

// show available platforms and devices
void showPlatforms()
{
	std::vector<cl::Platform> platforms;
	cl::Platform::get(&platforms);
	for (int i = 0; i < platforms.size(); i++)
	{
		std::cout << "Platform: " << platforms[i].getInfo<CL_PLATFORM_NAME>() << std::endl;
		std::vector<cl::Device> devices;
		platforms[i].getDevices(CL_DEVICE_TYPE_GPU | CL_DEVICE_TYPE_CPU, &devices);
		for (int j = 0; j < devices.size(); j++)
		{
			if (devices[j].getInfo<CL_DEVICE_TYPE>() == CL_DEVICE_TYPE_CPU)
				std::cout << "Device: "
						  << " CPU "
						  << " : " << devices[j].getInfo<CL_DEVICE_MAX_COMPUTE_UNITS>() << " compute units "
						  << "( " << devices[j].getInfo<CL_DEVICE_NAME>() << " )" << std::endl;
			if (devices[j].getInfo<CL_DEVICE_TYPE>() == CL_DEVICE_TYPE_GPU)
				std::cout << "Device: "
						  << " GPU "
						  << " : " << devices[j].getInfo<CL_DEVICE_MAX_COMPUTE_UNITS>() << " compute units "
						  << "( " << devices[j].getInfo<CL_DEVICE_NAME>() << " )" << std::endl;
		}
	}
}

// try to construct a context with the first platform of the requested type
cl::Context getContext(cl_device_type requestedDeviceType, std::vector<cl::Platform> &platforms)
{
	// try to create a context of the requested platform type
	for (unsigned int i = 0; i < platforms.size(); i++)
	{
		try
		{
			cl_context_properties cps[] = {CL_CONTEXT_PLATFORM, (cl_context_properties)(platforms[i])(), 0};
			return cl::Context(requestedDeviceType, cps);
		}
		catch (...)
		{
		}
	}
	throw CL_DEVICE_NOT_AVAILABLE;
}

void draw_square(cl_uint *screen, cl_uint size_x, cl_uint size_y, uint x, uint y, uint width, uint value)
{
	for (uint i = y; i < (y + width); i++)
	{
		for (uint j = x; j < (x + width); j++)
		{
			screen[i * size_x + j] = value;
		}
	}
}
//Draw a M in a given position, and a border on a given matrix. 
void set_start_state(cl_uint *screen, cl_uint x, cl_uint y, uint value)
{
	uint border = 1;
	for (uint i = 0; i < y; i++)
	{
		for (uint j = 0; j < x; j++)
		{
			screen[i * x + j] = value * (i < border || i >= (y - border) || j < border || j >= (x - border));
		}
	}
	uint sx = 125;
	uint sy = 125;
	uint sqs = 50;

	draw_square(screen, x, y, sx, sy, sqs, value);
	draw_square(screen, x, y, sx, sy + (1 * sqs), sqs, value);
	draw_square(screen, x, y, sx, sy + (2 * sqs), sqs, value);
	draw_square(screen, x, y, sx, sy + (3 * sqs), sqs, value);
	draw_square(screen, x, y, sx, sy + (4 * sqs), sqs, value);

	draw_square(screen, x, y, sx + (1 * sqs), sy + (1 * sqs), sqs, value);
	draw_square(screen, x, y, sx + (2 * sqs), sy + (2 * sqs), sqs, value);
	draw_square(screen, x, y, sx + (3 * sqs), sy + (1 * sqs), sqs, value);

	draw_square(screen, x, y, sx + (4 * sqs), sy, sqs, value);
	draw_square(screen, x, y, sx + (4 * sqs), sy+ (1 * sqs), sqs, value);
	draw_square(screen, x, y, sx + (4 * sqs), sy+ (2 * sqs), sqs, value);
	draw_square(screen, x, y, sx + (4 * sqs), sy+ (3 * sqs), sqs, value);
	draw_square(screen, x, y, sx + (4 * sqs), sy+ (4 * sqs), sqs, value);
}

int main(int argc, char **argv)
{
	//Recover arguments
	if(argc < 4 || argc > 6){
        std::cout << "ac [automaton] [repetitions] [visual] [max=16]" << std::endl;
        exit(0);
    }
	bool parity = std::string(argv[1]).compare("parity")==0;
    int rep = atoi(argv[2]);
    bool visual = std::string(argv[3]).compare("visual")==0;
	cl_uint max = 16;
	//The max value is only set for cyclic AC
	if(argc == 5 && !parity){
		max = atoi(argv[4]);
	}
	
	std::vector<cl::Platform> platforms;
	cl::Platform::get(&platforms);

	// get a context on a platform containing at least one GPU
	cl::Context context = getContext(CL_DEVICE_TYPE_GPU, platforms);

	// get a list of devices on this platform
	std::vector<cl::Device> devices = context.getInfo<CL_CONTEXT_DEVICES>();

	// create a command queue and use the first device of the platform
	cl::CommandQueue queue = cl::CommandQueue(context, devices[0]);
	std::string deviceName;
	devices[0].getInfo(CL_DEVICE_NAME, &deviceName);
	//Remove a byte '\0', if it exist, at the end of the deviceName.
	deviceName = *(deviceName.c_str() + deviceName.size()-1) == 0 ? deviceName.substr(0, deviceName.size() - 1) : deviceName;
	std::cout << "Command queue created successfuly, kernels will be executed on : " << deviceName << std::endl;

	// read source file
	std::ifstream sourceFile("ac.cl");
	std::string sourceCode(std::istreambuf_iterator<char>(sourceFile), (std::istreambuf_iterator<char>()));
	cl::Program::Sources source(1, std::make_pair(sourceCode.c_str(), sourceCode.length() + 1));

	// make program of the source code in the context
	cl::Program program = cl::Program(context, source);

	// AC parameters
	cl_uint2 size;
	size.x = 1024;
	size.y = 1024;

	// code surounded by try catch in order to get OpenCL errors (eg. compilation error)
	try
	{
		// build program for these specific device
		program.build(devices);

		// construct kernel
		cl::Kernel kernel;
		if(parity){
			kernel = cl::Kernel(program, "parity_automaton");
		}else{
			kernel = cl::Kernel(program, "cyclic_automaton");
		}

		// host side memory allocation
		cl_uint *screen = new cl_uint[size.x * size.y];

		if(parity){
			max = 1;
			set_start_state(screen, size.x, size.y, max);
		}else{
			// if we are in cyclic AC, we initialize the matrix with random values.
			srand(time(NULL));
			for (uint i = 0; i < size.y; i++)
			{
				for (uint j = 0; j < size.x; j++)
				{
					screen[i * size.x + j] = rand()%(max+1);
				}
			}
		}
		
		// device side memory allocation, destination buffer
		cl::Buffer gpu_screen = cl::Buffer(context, CL_MEM_WRITE_ONLY, size.x * size.y * sizeof(cl_uint));
		cl::Buffer gpu_read;

		Display<unsigned int> d;
		if(visual){
			d = Display<unsigned int>(screen, size.x, size.y, max);
			d.show();
			d.waitForKey();
		}

		// start chono for performance measurements
		auto start = std::chrono::high_resolution_clock::now();
		// device side memory allocation, source buffer, initialized from screen matrix
		gpu_read = cl::Buffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, size.x * size.y * sizeof(cl_uint), screen);
		// set arguments to kernel
		kernel.setArg(0, gpu_screen); // destination buffer
		kernel.setArg(1, gpu_read); // source buffer
		kernel.setArg(2, size); // buffer size
		kernel.setArg(3, max); // max value in matrix

		// execute the kernel
		cl::NDRange local(8, 8);
		cl::NDRange global(std::ceil(float(size.x) / float(local[0])) * local[0], std::ceil(float(size.y) / float(local[1])) * local[1]);
		while (rep-- > 0)
		{
			// add the kernel in the execution queue
			queue.enqueueNDRangeKernel(kernel, cl::NullRange, global, local);

			// Copy gpu_dst -> gpu_src
            queue.enqueueCopyBuffer(gpu_screen, gpu_read, 0, 0, size.x*size.y * sizeof(cl_uint));

			if(visual){
				// getting result from device to host (write gpu_screen -> screen)
				queue.enqueueReadBuffer(gpu_screen, CL_TRUE, 0, size.x * size.y * sizeof(cl_uint), screen);

				// wait for queue to finish
				queue.finish();
				// display the result
				d.show();
			}
		}
		// wait for queue to finish
		if(!visual){
			queue.enqueueReadBuffer(gpu_screen, CL_TRUE, 0, size.x * size.y * sizeof(cl_uint), screen);
			queue.finish();
		}
		// print duration of execution
		auto stop = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start); 
        std::cout << "Duration: " << duration.count() / 1000.0 << " seconds" << std::endl; 
		
		if(visual)
			d.waitForKey();
	}
	catch (cl::Error error)
	{
		std::cerr << error.what() << "(" << error.err() << ")" << std::endl;
		std::string buildLog;
		program.getBuildInfo(devices[0], CL_PROGRAM_BUILD_LOG, &buildLog);
		std::cerr << buildLog << std::endl;
	}
}
