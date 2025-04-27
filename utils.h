#ifndef _UTILS_H
#define _UTILS_H

#include <stdint.h>

void fill_buffer_with_random_bytes(char *buffer);
char *random_ipv4();
uint16_t random_port();
size_t strftok(char *sep, char *str, char **tptrs, size_t nptrs);
int strisnum(char *s);

#endif /* _UTILS_H */
