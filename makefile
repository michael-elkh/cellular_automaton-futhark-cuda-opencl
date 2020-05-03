CC=gcc
NVCC=nvcc
CFLAGS=-std=gnu11 -O3 #-Wall -Wextra -g -fsanitize=leak -fsanitize=undefined
ifeq ($(TARGET),)
TARGET=cuda
endif

all: uis benchs

uis: futhark_ui opencl_ui cuda_ui
benchs: futhark_bench opencl_bench cuda_bench

futhark_ui: Futhark/ac_futhark.o Futhark/futhark_wrap.o Interface/gfx/gfx.o Interface/visual.o
	$(CC) $(CFLAGS) $(notdir $^) -o $@ -lm -lcuda -lcudart -lnvrtc -lOpenCL -lSDL2

futhark_bench: OpenCL/ac_opencl.o Interface/bench.o
	$(CC) $(CFLAGS) $(notdir $^) -o $@ -lm -lcuda -lcudart -lnvrtc -lOpenCL

opencl_ui: OpenCL/ac_opencl.o Interface/gfx/gfx.o Interface/visual.o
	$(CC) $(CFLAGS) $(notdir $^) -o $@ -lSDL2 -lOpenCL

opencl_bench: OpenCL/ac_opencl.o Interface/bench.o
	$(CC) $(CFLAGS) $(notdir $^) -o $@ -lOpenCL

cuda_ui: Cuda/ac_cuda.o Interface/gfx/gfx.o Interface/visual.o
	$(NVCC) $(notdir $^) -o $@ -lSDL2

cuda_bench: Cuda/ac_cuda.o Interface/bench.o
	$(NVCC) $(notdir $^) -o $@

%.c: %.fut
	futhark $(TARGET) --library $< -o $(@:%.c=%)

%.o: %.cu
	$(NVCC) -c $^

%.o: %.c
	$(CC) -c $(CFLAGS) $^

clean:
	rm -vf *.o *_ui *_bench Futhark/ac_futhark.h Futhark/ac_futhark.c