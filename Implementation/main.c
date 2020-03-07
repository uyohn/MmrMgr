//  INCLUDES //
#include <stdio.h>
#include <stdlib.h>

#define REGION_SIZE 100

// FUNCTION DEFINITIONS //

void memory_alloc (unsigned int size);
int  memory_free  (void *valid_ptr);
int  memory_check (void *ptr);
void memory_init  (void *ptr, unsigned int size);


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
	*(unsigned int*)ptr = size;
	*(char*)((char*)ptr + sizeof(unsigned int)) = 0;

	return;
}

int* memory_allocate (unsigned int size) {

}

int memory_free (void *valid_ptr) {

}

int memory_check (void *ptr) {

	return 1;
}
