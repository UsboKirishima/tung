#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <string.h>
#include <ctype.h>

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

uint8_t string_includes_char(const char *buff, size_t len, const char chr) {
	for(size_t i = 0; i < len; i++) {
		if(buff[len] == chr) 
			return 1;
	}
	return 0;
}

// SPDX-License-Identifier: GPL-2.0
size_t strftok(char *sep, char *str, char **tptrs, size_t nptrs) {
	size_t seplen = strlen(sep);
	size_t i, j = 0;
	int inside = 0;

	while(*str) {
		for(i = 0; i < seplen; i++) {
			if (sep[i] == *str)
				break;
		}
		if (i == seplen) { /* no match */
			if (!inside) {
				tptrs[j++] = str;
				inside = 1;
			}
		} else { /* match */
			if (inside) {
				*str = '\0';
				if (j == nptrs)
					return j;
				inside = 0;
			}
		}
		str++;
	}
	return j;
}

// SPDX-License-Identifier: GPL-2.0
int strisnum(const char *s) {
	if (s == NULL)
        	return 0;

    	int digits = 0;

    	while (isspace(*s))
        	s++;

    	if (*s == '-')
        	s++;

    	while (*s) {
        	if (isspace(*s)) {
            		while (isspace(*s))
                		s++;
            		return *s ? 0 : digits ? 1 : 0;
        	}
        	if (!isdigit(*s))
            		return 0;
        	digits++;
        	s++;
    	}

    	return digits ? 1 : 0;
}

