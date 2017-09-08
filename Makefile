
CC=gcc
CFLAGS=-Wall -g -std=c99

BINS=leakcount memory_shim.so sctracer


all: $(BINS)

leakcount: leakcount.c
	$(CC) $(CFLAGS) -o leakcount leakcount.c

sctracer: sctracer.c
	$(CC) $(CFLAGS) -o sctracer sctracer.c

memory_shim.so:  memory_shim.c
	$(CC) $(CFLAGS) -fPIC -shared -o memory_shim.so memory_shim.c -ldl


clean:
	rm $(BINS)

