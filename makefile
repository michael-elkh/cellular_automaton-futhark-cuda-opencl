CC=gcc
LD=$(CC) -c
CFLAGS=-std=gnu11 -O3 #-Wall -Wextra -g -fsanitize=leak -fsanitize=undefined
LIBS=-lSDL2 -lm -lcuda -lnvrtc -lOpenCL
TARGET=cuda

UI: fut_bench

fut_bench: ac.o fut_wrap.o fut_bench.o
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)

fut_ui: ac.o fut_wrap.o AC_gfx.o gfx.o 
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)

AC_gfx.o: UI/AC_gfx.c UI/gfx.c
	$(LD) $^

ac.o: ac.fut
	futhark $(TARGET) --library ac.fut
	$(LD) ac.c

%.o: %c
	$(LD) $^ 

python:
	futhark python --library ac.fut

clean:
	rm -vf ac.py ac.c ac.h *.o fut_ui