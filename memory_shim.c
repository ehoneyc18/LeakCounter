#define _GNU_SOURCE

void __attribute__ ((constructor)) memfunc_init(void);
void __attribute__ ((destructor)) cleanup(void);

#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>

void *(*original_malloc)(size_t size);
void (*original_free)(void *ptr);

unsigned long int memUsed = 0;
unsigned long int numMalloc = 0;
unsigned long int numFree = 0;

//to hold amount of mem malloc'd
unsigned long int sizes[100000];

//set pointers to original functions
void memfunc_init(void)
{
	original_malloc = dlsym(RTLD_NEXT, "malloc");
	original_free = dlsym(RTLD_NEXT, "free");
}

//printer for library unload
void cleanup(void)
{
	unsigned long int numLeaks = numMalloc - numFree;

	//only print leaks if something to print
	if (numMalloc != 0)
	{
		for (int i = 0; i < numMalloc; i++)
		{
			if (sizes[i] != 0)
				fprintf(stderr, "%s	%lu\n", "LEAK", sizes[i]);
		}
	}

	fprintf(stderr, "%s	%lu	%lu\n", "TOTAL", numLeaks, memUsed);
}

void *malloc(size_t size)
{
	void *ptr;

	//malloc original size req'd and 2 additional ints to store size and tag
	ptr = original_malloc(size + sizeof(int)*2);

	//if no memory, return NULL
	if (ptr == NULL)
		return NULL;

	memUsed += size;

	//keep track of malloc'd amount
	int *amountStored = (int *)ptr;

	*amountStored = size;

	//keep track of label for malloc'd amount
	int *tag = (int *)(ptr + sizeof(int));

	*tag = numMalloc;

	//store size for printing later
	sizes[*tag] = size;

	numMalloc++;

	return (ptr + sizeof(int)*2);
}

//add check for leak
void free(void *ptr)
{
	//if nothing to free, return
	if (ptr == NULL)
		return;

	//get stored values
	void *checkMalloc = (ptr - sizeof(int)*2);

	int *amountStored = (int *)checkMalloc;

	memUsed -= *amountStored;

	int *tag = (int *)(ptr - sizeof(int));

	//empty out free'd vars
	sizes[*tag] = 0;

	numFree++;

	original_free(checkMalloc);
}
