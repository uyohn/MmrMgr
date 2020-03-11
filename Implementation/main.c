// INCLUDES //
#include <stdio.h>
#include <stdlib.h>

#define REGION_SIZE 100
#define HEADER_SIZE sizeof(int) + sizeof(char)


// FUNCTION DEFINITIONS //

// mandatory:
void* memory_alloc (unsigned int size);
int   memory_free  (void *valid_ptr);
int   memory_check (void *ptr);
void  memory_init  (void *ptr, unsigned int size);

// helpers:
void 			gen_header		(void *ptr, int size, int locked);
unsigned int 	get_block_size 	(void *ptr);
char 			get_block_lock 	(void *ptr);
char 			get_header_size ();


// START OF REGION BLOCK //
char* region = NULL;


// MAIN //
int main ()
{
	// first prepare memory region to work with
	region = (char*) malloc(REGION_SIZE * sizeof(char));

	// initialize region
	printf("Let's initialize the memory\n");
	printf("---------------------------\n");

	memory_init(region, REGION_SIZE);


	memory_alloc(5);
	memory_alloc(10);
	return 0;
}


// FUNCTION IMPLEMENTATIONS //
// MEMORY INIT
// +------------+--------------+
// | block size | is populated |
// +------------+--------------+

void memory_init (void *ptr, unsigned int size)
{
	// header for whole region, lock = 0 (memory is free)
	gen_header(ptr, size, 0);

	return;
}

void* memory_alloc (unsigned int size)
{
	printf("memory_alloc:\n");

	// find free space
	char* ptr = region;

	while (get_block_lock(ptr))
	{
		if ( memory_check(ptr + get_block_size(ptr) + HEADER_SIZE) )
		{
			ptr += get_block_size(ptr) + HEADER_SIZE;
			
			printf("block size: %d\n", get_block_size(ptr));
			printf("block lock: %d\n", get_block_lock(ptr));
		}
		else
		{
			return NULL;
		}
	}

	// SPLIT IT
	// header for allocated space
	gen_header(ptr + size + HEADER_SIZE, get_block_size(ptr) - size, 0);
	// header for rest
	gen_header(ptr, size, 1);

	return ptr;
}

int memory_free (void *valid_ptr)
{
}

int memory_check (void *ptr)
{
	printf("memory_check:\n");

	if ( (char *)ptr >= (char *)region &&
		 (char *)ptr <  (char *)(region + REGION_SIZE) )
		return 1;
	else
		return 0;
}

// helpers:
void gen_header (void *ptr, int size, int locked)
{
	printf("gen_header:\n");
	int int_size  = sizeof(unsigned int);

	// block length
	*(unsigned int *)ptr = size;

	// lock
	*(char*)((char*)ptr + int_size) = locked;

	return;
}

unsigned int get_block_size (void *ptr)
{
	return *(unsigned int *)ptr;
}

char get_block_lock (void *ptr)
{
	return *(char*)((char*)ptr + sizeof(unsigned int));
}

