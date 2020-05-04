GFX_LIB=-lSDL2

ifeq ($(TARGET),)
TARGET=cuda
endif

LIBS=-lm
ifeq ($(TARGET),cuda)
CC=nvcc
CFLAGS=-Xptxas -O3
LIBS+=-lcuda -lcudart -lnvrtc
else ifeq ($(TARGET),opencl)
CC=gcc
CFLAGS=-std=gnu11 -O3 #-Wall -Wextra -g -fsanitize=leak -fsanitize=undefined
LIBS+=-lOpenCL 
endif

all: uis benchs

uis: futhark_$(TARGET)_ui $(TARGET)_ui
benchs: futhark_$(TARGET)_bench $(TARGET)_bench

futhark_$(TARGET)_ui: Futhark/ac_futhark.o Futhark/futhark_wrap.o Interface/gfx/gfx.o Interface/visual.o
	$(CC) $(CFLAGS) $(notdir $^) -o $@ $(LIBS) $(GFX_LIB)

futhark_$(TARGET)_bench: Futhark/ac_futhark.o Futhark/futhark_wrap.o Interface/bench.o
	$(CC) $(CFLAGS) $(notdir $^) -o $@ $(LIBS)

$(TARGET)_ui: $(TARGET)/ac_$(TARGET).o Interface/gfx/gfx.o Interface/visual.o
	$(CC) $(CFLAGS) $(notdir $^) -o $@ $(LIBS) $(GFX_LIB)

$(TARGET)_bench: $(TARGET)/ac_$(TARGET).o Interface/bench.o
	$(CC) $(CFLAGS) $(notdir $^) -o $@ -lOpenCL

%.c: %.fut
	futhark $(TARGET) --library $< -o $(@:%.c=%)

%.o: %.cu
	$(CC) $(CFLAGS) -c $^

%.o: %.c
	$(CC) -c $(CFLAGS) $^

clean:
	rm -vf *.o *_ui *_bench Futhark/ac_futhark.h Futhark/ac_futhark.c