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
unsigned int	block_size		(void *ptr);
char			block_locked	(void *ptr);
int 			can_alloc		(void *ptr, int size);
char			*next_block 	(void *ptr);
int 			in_range 		(void *ptr);


// START OF REGION BLOCK //
char* region = NULL;


// MAIN //
int main ()
{
	// first prepare memory region to work with
	region = (char*) malloc(REGION_SIZE * sizeof(char));

	// initialize region
	memory_init(region, REGION_SIZE);


	// testing
	// TODO: move to tester
	memory_alloc(10);
	memory_alloc(10);
	memory_alloc(40);
	memory_alloc(20);

	return 0;
}


// FUNCTION IMPLEMENTATIONS //
// MEMORY INIT
// +------------+--------------+---- -   -
// | block size | is populated |
// +------------+--------------+---- -   -

// initialize region
// generate header with region size and unlock it
void memory_init (void *ptr, unsigned int size)
{
	gen_header(ptr, size, 0);
	return;
}

// try to allocate memory block
// look for unlocked space
// if you find enough of it, split it and lock allocated portion
void* memory_alloc (unsigned int size)
{
	// look for free space
	char* ptr = region;

	while ( !can_alloc(ptr, size) ) // the block is locked or too small
	{
		if ( in_range(next_block(ptr)) )	// is the new pointer still in range?
			ptr = next_block(ptr);
		else
			return NULL;					// didn't find enough free space AND you reached end
	}

	// SPLIT IT
	// header for rest
	gen_header(ptr + HEADER_SIZE + size, block_size(ptr) - HEADER_SIZE - size, 0);
	// header for allocated space
	gen_header(ptr, size, 1);

	// return pointer to beginning of usable memory
	return ptr + HEADER_SIZE;
}

// free memory block at pointer
// implemented by unlocking the block in header
// join free blocks
// TODO: does this function ever fail?
int memory_free (void *valid_ptr)
{
	// unlock this block
	gen_header(valid_ptr, block_size(valid_ptr), 0);


	// join free blocks together
	char *ptr = region;

	while ( in_range(ptr) )
	{
		if ( !block_locked(ptr) && !block_locked(next_block(ptr)))
			gen_header(ptr, block_size(ptr) + block_size(next_block(ptr)) + HEADER_SIZE, 0);

		ptr = next_block(ptr);
	}

	return 0;
}

// TODO: add more restrictions
int memory_check (void *ptr)
{
	if (in_range(ptr))
		return 1;

	return 0;
}

int in_range (void *ptr)
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

unsigned int block_size (void *ptr)
{
	return *(unsigned int *)ptr;
}

char block_locked (void *ptr)
{
	return *(char*)((char*)ptr + sizeof(unsigned int));
}

// return true (1) if the block is unlocked and is big enough
// TODO: join memory blocks here as well?
int can_alloc (void *ptr, int size)
{
	return !block_locked(ptr) && block_size(ptr) >= size + HEADER_SIZE;
}

// return pointer to next block
// calculated by HEADER_SIZE + size of block we are currently at
char* next_block (void *ptr)
{
	return (ptr + block_size(ptr) + HEADER_SIZE);
}
