// INCLUDES //
#include <stdio.h>
#include <stdlib.h>

#define REGION_SIZE 100


// FUNCTION DEFINITIONS //

// mandatory:
void memory_alloc (unsigned int size);
int  memory_free  (void *valid_ptr);
int  memory_check (void *ptr);
void memory_init  (void *ptr, unsigned int size);

// helpers:
void gen_header (void *ptr, int size, int locked);


// START OF REGION BLOCK //
char* region = NULL;


// MAIN //
int main () {
	// first prepare memory region to work with
	region = (char*) malloc(REGION_SIZE * sizeof(char));

	// initialize region
	printf("Let's initialize the memory\n");
	printf("---------------------------\n");

	memory_init(region, REGION_SIZE);


	// test value to see in debug memory
	region[50] = 100;

	return 0;
}


// FUNCTION IMPLEMENTATIONS //
// MEMORY INIT
// +------------+--------------+
// | block size | is populated |
// +------------+--------------+

void memory_init (void *ptr, unsigned int size) {
	// header for whole region, lock = 0 (memory is free)
	gen_header(ptr, size, 0);

	return;
}

int* memory_allocate (unsigned int size) {
}

int memory_free (void *valid_ptr) {
}

int memory_check (void *ptr) {
	return 1;
}

// helpers:
void gen_header (void *ptr, int size, int locked) {
	int int_size = sizeof(unsigned int);

	// block length
	*(unsigned int*)ptr = size;

	// lock
	*(char*)((char*)ptr + int_size) = locked;

	return;
}
