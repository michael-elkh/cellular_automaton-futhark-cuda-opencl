CC=gcc
NVCC=nvcc
CFLAGS=-std=gnu11 -O3 #-Wall -Wextra -g -fsanitize=leak -fsanitize=undefined
GFX_LIB=-lSDL2

ifeq ($(TARGET),)
TARGET=cuda
endif


LIBS=-lm
ifeq ($(TARGET),cuda)
LIBS+=-lcuda -lcudart -lnvrtc
else ifeq ($(TARGET),opencl)
LIBS+=-lOpenCL 
endif

all: uis benchs

uis: futhark_$(TARGET)_ui $(TARGET)_ui
benchs: futhark_$(TARGET)_bench $(TARGET)_bench

futhark_$(TARGET)_ui: Futhark/ac_futhark.o Futhark/futhark_wrap.o Interface/gfx/gfx.o Interface/visual.o
	$(CC) $(CFLAGS) $(notdir $^) -o $@ $(LIBS) $(GFX_LIB)

futhark_$(TARGET)_bench: Futhark/ac_futhark.o Futhark/futhark_wrap.o Interface/bench.o
	$(CC) $(CFLAGS) $(notdir $^) -o $@ $(LIBS)

opencl_ui: OpenCL/ac_opencl.o Interface/gfx/gfx.o Interface/visual.o
	$(CC) $(CFLAGS) $(notdir $^) -o $@ $(LIBS) $(GFX_LIB)

opencl_bench: OpenCL/ac_opencl.o Interface/bench.o
	$(CC) $(CFLAGS) $(notdir $^) -o $@ -lOpenCL

cuda_ui: Cuda/ac_cuda.o Interface/gfx/gfx.o Interface/visual.o
	$(NVCC) $(notdir $^) -o $@ $(GFX_LIB)

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