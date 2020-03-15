// INCLUDES //
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// COLORFUL OUTPUT
#define ANSI_RED 			"\x1b[31m"
#define ANSI_RED_BOLD		"\x1b[31;1m"
#define ANSI_GREEN			"\x1b[32m"
#define ANSI_YELLOW			"\x1b[33m"
#define ANSI_YELLOW_BOLD	"\x1b[33;1m"
#define ANSI_BLUE			"\x1b[34m"
#define ANSI_BLUE_BOLD		"\x1b[34;1m"
#define ANSI_MAGENTA		"\x1b[35m"
#define ANSI_CYAN			"\x1b[36m"
#define ANSI_GREY_FAINT		"\x1b[37;2m"
#define ANSI_WHITE_BOLD 	"\x1b[37;1m"
#define ANSI_RESET			"\x1b[0m"

// HEADER SIZE - using bitwise operations to minimalize its size
#define HEADER_SIZE 3
// REGION HEADER SIZE - using 1 uint to remember size of region
#define REGION_HEADER_SIZE sizeof(unsigned int)

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
unsigned int	get_region_size ();
int 			can_alloc		(void *ptr, int size);
char			*next_block 	(void *ptr);
int 			in_range 		(void *ptr);
void			join_free_blocks();

// tests:
void	test_regular_blocks  	(int block_size, int region_size);
void	test_small_irregular_blocks	(int region_size);
void	test_large_irregular_blocks	(int region_size);

// REGION BLOCK SETUP //
char *region;



// MAIN //
int main ()
{
	// testing
	// small regular blocks
	test_regular_blocks(15, 50);
	test_regular_blocks( 8, 100);
	test_regular_blocks(24, 200);

	// small irregular blocks
	test_small_irregular_blocks(50);
	test_small_irregular_blocks(100);
	test_small_irregular_blocks(200);

	// large irregular blocks
	test_large_irregular_blocks(1000);
	test_large_irregular_blocks(10000);
	test_large_irregular_blocks(100000);

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
	// remember region size
	*(unsigned int *)ptr = size - REGION_HEADER_SIZE;

	// generate header for rest of space
	gen_header(ptr + REGION_HEADER_SIZE, size - REGION_HEADER_SIZE, 0);

	// offset region
	region += REGION_HEADER_SIZE;

	return;
}

// try to allocate memory block
// look for unlocked space
// if you find enough of it, split it and lock allocated portion
void* memory_alloc (unsigned int size)
{
	// look for free space
	char *ptr = region;

	while ( !can_alloc(ptr, size) ) // the block is locked or too small
	{
		if ( in_range(next_block(ptr)) )	// is the new pointer still in range?
			ptr = next_block(ptr);
		else
			return NULL;					// didn't find enough free space AND you reached end
	}

													// the free space is at least same size as we want to allocate
	if ( block_size(ptr) <= size + HEADER_SIZE) 	// but it's not big enough for it to be worth splitting
		gen_header(ptr, block_size(ptr) + HEADER_SIZE, 1);		// just allocate it as it is
	else
	{
		// SPLIT IT
		// header for rest
		gen_header(ptr + HEADER_SIZE + size, block_size(ptr) - size, 0);
		// header for allocated space
		gen_header(ptr, size + HEADER_SIZE, 1);
	}

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
	gen_header(valid_ptr - HEADER_SIZE, block_size(valid_ptr - HEADER_SIZE) + HEADER_SIZE, 0);

	// join consecutive free blocks together
	join_free_blocks();

	return 0;
}

// check if ptr is pointing to valid block
int memory_check (void *ptr)
{
	// calculate pointer to head
	char *head = (char *)(ptr - (HEADER_SIZE) );

	// is the pointer even in range?
	if ( !in_range(head) )
		return 0;

	// is the pointer allocated?
	char *tmp = region;
	while ( in_range(tmp) )
		if ( head == tmp && block_locked(tmp) )
			return 1;
		else
			tmp = next_block(tmp);

	return 1;
}

int in_range (void *ptr)
{
	if ( (char *)ptr >= (char *)region &&
		 (char *)ptr <  (char *)(region + get_region_size()) )
		return 1;
	else
		return 0;
}

// helpers:
void gen_header (void *ptr, int size, int locked)
{
	// block length
	*(unsigned int *)ptr = size - HEADER_SIZE;
	// block are at most 50 000 bites long,
	// so 2 bytes will suffice to store block length

	// lock
	*(char*)((char*)ptr + 2) = locked;

	return;
}

unsigned int block_size (void *ptr)
{
	/* return *(unsigned int *)ptr & ~(1 << 16); */
	return *(unsigned int *)ptr & 8454143;
}

unsigned int get_region_size ()
{
	return *(unsigned int *)(region - sizeof(unsigned int));
}

char block_locked (void *ptr)
{
	return *(char *)((char *)ptr + 2);
}

// return true (1) if the block is unlocked and is big enough
// TODO: join memory blocks here as well?
int can_alloc (void *ptr, int size)
{
	return !block_locked(ptr) && block_size(ptr) >= (unsigned int)size;
}

// return pointer to next block
// calculated by HEADER_SIZE + size of block we are currently at
char* next_block (void *ptr)
{
	return (ptr + block_size(ptr) + HEADER_SIZE);
}

// go through whole memory region
// and join consecutive free blocks 
void join_free_blocks()
{
	char *ptr = region;

	while ( in_range(ptr) )
	{
		if ( !block_locked(ptr) && in_range(next_block(ptr)) && !block_locked(next_block(ptr)))
			gen_header(ptr, block_size(ptr) + HEADER_SIZE + block_size(next_block(ptr)) + HEADER_SIZE, 0);

		ptr = next_block(ptr);
	}
}

void print_memory(void *start, unsigned int len)
{
	for (unsigned int i = 0; i <= len; i++)
	{
		if ( !(i % 8) ) 
			printf("\n" ANSI_BLUE "0x%p " ANSI_RESET "<" ANSI_YELLOW "region+" ANSI_YELLOW_BOLD "%02d" ANSI_RESET">", start, i);

		printf(ANSI_GREY_FAINT " 0x" ANSI_RESET ANSI_WHITE_BOLD "%02x " ANSI_RESET, *(char *)(start + i));

	}

	printf("\n\n");
}


// tests:
void test_regular_blocks(int block_size, int region_size)
{
	printf(ANSI_RED_BOLD "\nRUNNING TEST" ANSI_RESET " / regular blocks\n");

	// prepare the region
	region = (char *) malloc(region_size);
	memory_init(region, region_size);

	// print some info
	printf("\nRegion size:\t" ANSI_YELLOW "%d\n" ANSI_RESET, region_size);
	printf("Usable size:\t" ANSI_YELLOW "%d\n" ANSI_RESET, get_region_size());
	printf("Block size:\t" ANSI_YELLOW "%d\n" ANSI_RESET, block_size);

	// var for percentage calculation
	int successfull_allocs = 0;

	// alloc as many blocks as will fit in the region
	char *last_block = NULL;
	while ( (last_block = memory_alloc(block_size)) != NULL )
	{
		successfull_allocs++;
		memset(last_block, '#', block_size);
	}

	// print memory table
	printf(ANSI_RED_BOLD "\nREGION HEXDUMP" ANSI_RESET);
	print_memory(region, get_region_size());

	// print test results
	printf(ANSI_RED_BOLD "\nTEST RESULTS\n" ANSI_RESET);
	printf("\nIdeal result:\t" ANSI_YELLOW "%d\n" ANSI_RESET, region_size / block_size);
	printf("Actual result:\t" ANSI_YELLOW "%d\n" ANSI_RESET, successfull_allocs);
	printf(ANSI_BLUE "\nEffectivity:\t" ANSI_BLUE_BOLD "%02.0lf\%\n\n" ANSI_RESET, 100 * (double)successfull_allocs / ((double)region_size / (double)block_size) );

	printf(ANSI_RED_BOLD "\nEND TEST\n" ANSI_RESET);
	return;
}

void test_small_irregular_blocks(int region_size)
{
	printf(ANSI_RED_BOLD "\nRUNNING TEST" ANSI_RESET " / small irregular blocks\n");

	int blocks_size[10] = {8, 16, 9, 24, 10, 13, 18, 11, 20, 22};

	// prepare the region
	region = (char *) malloc(region_size);
	memory_init(region, region_size);

	// print some info
	printf("\nRegion size:\t" ANSI_YELLOW "%d\n" ANSI_RESET, region_size);
	printf("Usable size:\t" ANSI_YELLOW "%d\n" ANSI_RESET, get_region_size());
	printf("Block size:\t" ANSI_YELLOW "small irregular\n" ANSI_RESET);

	// var for percentage calculation
	int successfull_allocated_space = 0;

	// alloc as many blocks as will fit in the region
	char *last_block = NULL;
	int i;
	for (i = 0; (last_block = memory_alloc(blocks_size[i % 10])) != NULL; i++)
	{
		successfull_allocated_space += blocks_size[i % 10];
		memset(last_block, '#', block_size(last_block));
	}

	// print memory table
	printf(ANSI_RED_BOLD "\nREGION HEXDUMP" ANSI_RESET);
	print_memory(region, get_region_size());

	// print test results
	printf(ANSI_RED_BOLD "\nTEST RESULTS\n" ANSI_RESET);
	printf("Successfull allocs:\t" ANSI_YELLOW "%d\n" ANSI_RESET, i);
	printf(ANSI_BLUE "\nEffectivity:\t" ANSI_BLUE_BOLD "%02.0lf\%\n\n" ANSI_RESET, 100 * (double)successfull_allocated_space / (double)region_size );

	printf(ANSI_RED_BOLD "\nEND TEST\n" ANSI_RESET);
	return;
}

void test_large_irregular_blocks(int region_size)
{
	printf(ANSI_RED_BOLD "\nRUNNING TEST" ANSI_RESET " / large irregular blocks\n");

	int blocks_size[10] = {500, 825, 640, 5000, 3875, 789, 1598, 2155, 3128, 598};

	// prepare the region
	region = (char *) malloc(region_size);
	memory_init(region, region_size);

	// print some info
	printf("\nRegion size:\t" ANSI_YELLOW "%d\n" ANSI_RESET, region_size);
	printf("Usable size:\t" ANSI_YELLOW "%d\n" ANSI_RESET, get_region_size());
	printf("Block size:\t" ANSI_YELLOW "large irregular\n" ANSI_RESET);

	// var for percentage calculation
	int successfull_allocated_space = 0;

	// alloc as many blocks as will fit in the region
	char *last_block = NULL;
	int i;
	for (i = 0; (last_block = memory_alloc(blocks_size[i % 10])) != NULL; i++)
	{
		successfull_allocated_space += blocks_size[i % 10];
		memset(last_block, '#', block_size(last_block));
	}

	// print memory table
	/* printf(ANSI_RED_BOLD "\nREGION HEXDUMP" ANSI_RESET); */
	/* print_memory(region, get_region_size()); */

	// print test results
	printf(ANSI_RED_BOLD "\nTEST RESULTS\n" ANSI_RESET);
	printf("Successfull allocs:\t" ANSI_YELLOW "%d\n" ANSI_RESET, i);
	printf("Allocated space:\t" ANSI_YELLOW "%d\n" ANSI_RESET, successfull_allocated_space);
	printf(ANSI_BLUE "\nEffectivity:\t" ANSI_BLUE_BOLD "%02.0lf\%\n\n" ANSI_RESET, 100 * (double)successfull_allocated_space / (double)region_size );

	printf(ANSI_RED_BOLD "\nEND TEST\n" ANSI_RESET);
	return;
}
