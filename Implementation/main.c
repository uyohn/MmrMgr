//  INCLUDES //
#include <stdio.h>
#include <stdlib.h>


// FUNCTION DEFINITIONS //

void memory_init 	 (void *ptr, unsigned int size);
int* memory_allocate (unsigned int size);
int  memory_free 	 (void *valid_ptr);
int  memory_check 	 (void *ptr);


// START OF MEMORY BLOCK //
char* memory = NULL;


// MAIN //

int main () {
	printf("Let's init the memory\n");
	printf("----------------------------------\n");

	memory = (char*) malloc(100 * sizeof(char));
	memory_init(memory, 100);

	memory[50] = 100;

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
