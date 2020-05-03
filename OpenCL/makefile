CC=gcc -c
CFLAGS=-std=gnu11 -O3

all: ac_opencl.o

%.o: %.c
	$(CC) $(CFLAGS) $^

clean:
	rm -vf *.o