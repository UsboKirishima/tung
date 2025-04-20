/* This module is used by the software to perform all the attacks related to tcp/ip */
#include <stdio.h> /* logging */
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include "tung.h"
#include "logs.h"
#include "utils.h"

/* Internal Helpers */
static int create_tcp_socket();
static struct sockaddr_in build_target(const char *ip, const char *port);
static int should_continue(time_t start, uint16_t duration);

/* flood prototypes */
void syn_flood(const char *target_ip, uint16_t target_port, 
               	uint32_t packet_count, uint32_t delay_ms);
void ack_flood(const char *target_ip, uint16_t target_port,
               	uint32_t packet_count, uint32_t delay_ms);
void rst_flood(const char *target_ip, uint16_t target_port,
               	uint32_t packet_count, uint32_t delay_ms);
void fin_flood(const char *target_ip, uint16_t target_port,
               	uint32_t packet_count, uint32_t delay_ms);
void psh_ack_flood(const char *target_ip, uint16_t target_port, 
		uint32_t packet_count, uint32_t delay_ms);
void xmas_flood(const char *target_ip, uint16_t target_port, 
		uint32_t packet_count, uint32_t delay_ms);
void null_flood(const char *target_ip, uint16_t target_port, 
		uint32_t packet_count, uint32_t delay_ms);


