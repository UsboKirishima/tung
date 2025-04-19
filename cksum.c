#include <stdint.h>
#include <stdlib.h>

uint16_t compute_checksum(const void *data, size_t len) {
	const uint8_t *bytes = data;
    	uint32_t sum = 0;

    	for (size_t i = 0; i < len; i += 2) {
        	uint16_t word;
        
		if (i + 1 < len)
            		word = (bytes[i] << 8) | bytes[i + 1];
        	else
            		word = bytes[i] << 8; /* handle odd lengths */
        
		sum += word;
        	sum = (sum & 0xFFFF) + (sum >> 16); /* add carry */
    	}

    	return (uint16_t)(~sum);
}
