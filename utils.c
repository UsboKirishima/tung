#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

void fill_buffer_with_random_bytes(char *buffer) {
	for (uint16_t i = 0; i < sizeof(buffer); i++)
        	buffer[i] = rand() % 256;
}
