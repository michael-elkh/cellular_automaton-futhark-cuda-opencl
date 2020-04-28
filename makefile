CC=gcc
LD=$(CC) -c
CFLAGS=-std=gnu11 -O3 #-Wall -Wextra -g -fsanitize=leak -fsanitize=undefined
LIBS=-lSDL2 -lm -lcuda -lnvrtc -lOpenCL
TARGET=cuda

all: fut_wrap

ac.o: ac.fut
	futhark $(TARGET) --library ac.fut
	$(LD) ac.c

%.o: %c
	$(LD) $^ 

AC_gfx.o: UI/AC_gfx.c UI/gfx.c
	$(LD) $^

fut_wrap: ac.o fut_wrap.o AC_gfx.o gfx.o 
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)

python:
	futhark python --library ac.fut

clean:
	rm -vf ac.py ac.c ac.h *.o fut_wrap