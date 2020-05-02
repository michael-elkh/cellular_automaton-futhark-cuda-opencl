CC=gcc
LD=$(CC) -c
CFLAGS=-std=gnu11 -O3 #-Wall -Wextra -g -fsanitize=leak -fsanitize=undefined
GUI_LIBS=-lSDL2
LIBS=-lm -lcuda -lnvrtc -lOpenCL
TARGET=cuda

fut_bench: ac_fut.o fut_wrap.o fut_bench.o
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)

fut_ui: ac_fut.o fut_wrap.o AC_gfx.o gfx.o 
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS) $(GUI_LIBS)

AC_gfx.o: UI/AC_gfx.c UI/gfx.c
	$(LD) $^

ac_fut.o: ac_futhark.fut
	futhark $(TARGET) --library $< -o ac_fut
	$(LD) ac_fut.c

%.o: %c
	$(LD) $^

clean:
	rm -vf ac_fut.c ac_fut.h *.o fut_ui fut_bench