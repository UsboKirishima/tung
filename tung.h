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
void show_attacks_list(void); 

/* checksum  */
uint16_t compute_checksum(const void *data, size_t len);

/* IP Address */
int validate_ip(const char *ip);

/* UDP attacks */
void perform_udp_flood(struct attack_opts_t *opts);
void perform_udp_ampl(struct attack_opts_t *opts);
void perform_udp_fraggle(struct attack_opts_t *opts);
void perform_udp_app_layer_dos(struct attack_opts_t *opts);
void perform_udp_socket_exhaustion(struct attack_opts_t *opts);

/* TCP Attacks  */
void syn_flood(struct attack_opts_t *opts);

#endif /* _TUNG_H */
