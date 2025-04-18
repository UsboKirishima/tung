#include <stdio.h>
#include <stdlib.h>

void fill_buffer_with_random_bytes(char *buffer) {
	for (int i = 0; i < sizeof(buffer); i++)
        	buffer[i] = rand() % 256;
}
