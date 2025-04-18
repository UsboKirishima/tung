#ifndef _TUNG_H
#define _TUNG_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define DEFAULT_PORT "80"

struct attack_opts_t {
	char *atk_type; /* udp, ...  */
	char *atk_target;
	char *atk_port; /* default: 80  */
	uint16_t atk_duration; /* attack duration in seconds  */
	uint16_t atk_rate; /* packets per second  */
};

void show_usage(void);
void show_version(void);

#endif /* _TUNG_H */
