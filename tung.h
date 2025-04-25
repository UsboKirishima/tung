#ifndef _TUNG_H
#define _TUNG_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>

#include "logs.h"

#define DEFAULT_PORT "80"

struct attack_opts_t {
	char *atk_type; /* udp, ...  */
	char *atk_target;
	char *atk_port; /* default: 80  */
	uint16_t atk_duration; /* attack duration in seconds  */
	uint16_t atk_rate; /* packets per second  */
};

/* Globals CLI options  */
extern bool opt_verbose; 

extern char *opt_target;

extern uint16_t opt_port, 
	 	opt_duration, 
	 	opt_rate;

void show_usage(char **argv);
void show_version(void);
void show_attacks_list(char **argv);

/* check root priviliges */
int is_root();
void check_root();

/* checksum  */
uint16_t compute_checksum(const void *data, size_t len);

/* IP Address */
int validate_ip(const char *ip);

/* Target Utilities */
int is_target_up(char *ip_addr, int port);
int resolve_hostname(const char *hostname, char *ip_addr, size_t addr_len);

/* UDP attacks */
void perform_udp_flood(struct attack_opts_t *opts);
void perform_udp_ampl(struct attack_opts_t *opts);
void perform_udp_fraggle(struct attack_opts_t *opts);
void perform_udp_app_layer_dos(struct attack_opts_t *opts);
void perform_udp_socket_exhaustion(struct attack_opts_t *opts);

/* TCP Attacks  */
void syn_flood(struct attack_opts_t *opts);
void ack_flood(struct attack_opts_t *opts);
void rst_flood(struct attack_opts_t *opts);
void fin_flood(struct attack_opts_t *opts);
void psh_ack_flood(struct attack_opts_t *opts);
void xmas_flood(struct attack_opts_t *opts);
void null_flood(struct attack_opts_t *opts);

#endif /* _TUNG_H */
