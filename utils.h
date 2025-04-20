#ifndef _UTILS_H
#define _UTILS_H

#include <stdint.h>

void fill_buffer_with_random_bytes(char *buffer);
char *random_ipv4();
uint16_t random_port();

#endif /* _UTILS_H */
