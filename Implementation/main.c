// INCLUDES //
#include <stdio.h>
#include <stdlib.h>

#define REGION_SIZE 100
#define HEADER_SIZE sizeof(int) + sizeof(char)


// FUNCTION DEFINITIONS //

// mandatory:
void *memory_alloc (unsigned int size);
int   memory_free  (void *valid_ptr);
int   memory_check (void *ptr);
void  memory_init  (void *ptr, unsigned int size);

// helpers:
void			gen_header		(void *ptr, int size, int locked);
unsigned int 	get_block_size 	(void *ptr);
char			get_block_lock 	(void *ptr);
char			get_header_size ();
int 			can_alloc		(void *ptr, int size);
char 			*next_block		(void *ptr);


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


	char *first_block = (char *)memory_alloc(10);
	void *second_block = memory_alloc(10);

	memory_alloc(40);
	memory_alloc(20);

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
	printf("alloc memory block with size: %d\n", size);

	// find free space
	char* ptr = region;

	while (!can_alloc(ptr, size)) // the block is locked or too small
		// the block is free, but too small, maybe next block is free as well, so join them
		/* if (!get_block_lock(ptr) && !get_block_lock(ptr + get_block_size(ptr) + HEADER_SIZE)) */ 
		/* 	gen_header(ptr, get_block_size(ptr) + get_block_size(ptr + get_block_size(ptr) + HEADER_SIZE), 0); */
		if ( memory_check( next_block(ptr) ) ) //is the new pointer still in range?
			ptr = next_block(ptr);
		else
			return NULL;

	// SPLIT IT
	if ( get_block_size(ptr) >= size + HEADER_SIZE )			 //is there enough space?
	{
		// header for rest
		gen_header(ptr + HEADER_SIZE + size, get_block_size(ptr) - HEADER_SIZE - size, 0);
		// header for allocated space
		gen_header(ptr, size, 1);
	}

	// return pointer to usable memory, not to header of allocated block
	return ptr + HEADER_SIZE;
}

int memory_free (void *valid_ptr)
{
	// TODO - join free blocks together
	gen_header(valid_ptr, get_block_size(valid_ptr), 0);
	return 1;
}

int memory_check (void *ptr)
{
	if ( (char *)ptr >= (char *)region &&
		 (char *)ptr <  (char *)(region + REGION_SIZE) )
		return 1;
	else
		return 0;
}

// helpers:
void gen_header (void *ptr, int size, int locked)
{
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

// return true (1) if the block is unlocked and is big enough
// TODO: join memory blocks here as well?
int can_alloc (void *ptr, int size)
{
	return !get_block_lock(ptr) && get_block_size(ptr) > size + HEADER_SIZE;
}

// return pointer to next block
// calculated by HEADER_SIZE + size of block we are currently at
char* next_block (void *ptr)
{
	return (ptr + get_block_size(ptr) + HEADER_SIZE);
}
