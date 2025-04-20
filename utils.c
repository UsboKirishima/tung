#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>

void fill_buffer_with_random_bytes(char *buffer) {
	for (uint16_t i = 0; i < sizeof(buffer); i++)
        	buffer[i] = rand() % 256;
}

/* Returns NULL if error */
char *random_ipv4() {
	uint8_t o1, o2, o3, o4;

	char *res = malloc(16);
	if(!res) return NULL;

	o1 = rand() % 256;
	o2 = rand() % 256;
	o3 = rand() % 256;
	o4 = rand() % 256;

	int bytes_written = snprintf(res, 16, "%d.%d.%d.%d", o1, o2, o3, o4);
	
	if(bytes_written < 0 || bytes_written >= 16) {
		free(res);
		return NULL;
	}

	return res;
}

uint16_t random_port() {
	return (rand() % 65535) + 1;
}
