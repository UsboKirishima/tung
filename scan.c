/*
 * Port scanning mode: this part of the program sends
 * requests to each port of a host to check if the port is open.
 * This mode implements different methods for scanning ports.
 *
 * Things we need to know:
 *
 *  1. Which ports we want to scan:
 *     Ports can be specified as multiple ranges (e.g., 30-50,60-80),
 *     using the `all` flag as an alias for 1-65535 (all ports),
 *     or using the `common` flag to scan the most common ports stored in common_ports[].
 *
 *  2. Type of attack (scan method):
 *     Some useful methods are used in port scanners to reach ports
 *     in the least amount of time possible and with as much stealth as possible.
 *     For example, tools like Nmap use SYN scan, Idle scan,
 *     Window scan, ACK scan, etc.
 *     This tool aims to implement only the simplest methods:
 *     SYN scan and ACK scan.
 *     In the future, implementing an Idle scan would be a great improvement,
 *     as it is one of the stealthiest scanning methods.
 *
 * Usage:
 *     tung --scan <ports> --attack <atk> --target <ip>
 */
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include "utils.h"
#include "logs.h"
#include "tung.h"

#define MAX_PORT 65535

struct port_t {
	bool is_active;
	bool retry;
};

const uint16_t common_ports[] = {
    20, 21, 22, 23, 25, 53, 67, 68, 69, 80,
    110, 123, 137, 138, 139, 143, 161, 162, 179, 194,
    389, 443, 445, 465, 514, 515, 520, 523, 548, 554,
    587, 631, 636, 666, 989, 990, 993, 995, 1080, 1194,
    1433, 1434, 1521, 1723, 2049, 2121, 3306, 3389, 3690, 4000,
    4040, 4321, 4444, 4662, 4899, 5000, 5060, 5432, 5500, 5631,
    5900, 6000, 6001, 6667, 6697, 7000, 8000, 8080, 8081, 8443,
    8888, 9000, 9090, 9200, 9999, 10000, 11211, 27017, 27018, 50000,
    49152, 49153, 49154, 49155, 49156, 49157, 49158, 49159, 49160, 49161,
    49162, 49163, 49164, 49165, 49166, 49167, 49168, 49169, 49170, 49171,
    49172, 49173, 49174, 49175, 49176, 49177, 49178, 49179, 49180, 49181,
    49182, 49183, 49184, 49185, 49186, 49187, 49188, 49189, 49190, 49191
};

const size_t common_ports_count = sizeof(common_ports) / sizeof(common_ports[0]);

/* prototypes */
void scan_init(void);

/* internal helpers */
static void __print_scan_debug(void);
static int __get_ports_to_scan(struct port_t *ports, const char *scan_val);

static void __print_scan_debug(void) {
	return;
}

static inline void __parse_ports_range(uint16_t low, uint16_t high, struct port_t *ports) {
	for(int i = low; i <= high; i++)
		ports[i].is_active = true;
}

/* This function simply parse the `--scan <value>`, and check
 * if the value is a range, single num, contains flag such as
 * 'all' or 'common'.
 * 
 * Returns 1 if the port_t *ports is correctly set.
 * Returns 0 if errors are found.
 * */
static int __get_ports_to_scan(struct port_t *ports, const char *scan_val) {
	char *ports_value = strdup(scan_val);
	char *args[32];
	int value_len = strftok(",", ports_value, args, 32);
	
	/* range  */
	for(int i = 0; i < value_len; i++) {
		char *current_arg = args[i];

		if(strchr(current_arg, '-')) {
			char *range[2];
			strftok("-", current_arg, range, 2);

			if(!strisnum(range[0]) || !strisnum(range[1])) {
				LOG_ERROR("Invalid input ports range");
				goto err;
			}

			int low = strtol(range[0], NULL, 0);
			int high = strtol(range[1], NULL, 0);

			if(low == high) { /* Input: 10-10 -> error invalid range  */
				LOG_ERROR("Invalid range, low port and high port are the same");
				goto err;
			} else if(low > high) { /* Input: 30-10 -> swap variables to 10-30  */
				int tmp = low;
				low = high;
				high = tmp;
			}

			__parse_ports_range(low, high, ports);
			return 1;
			
		} else if(strncmp(current_arg, "all", 3) == 0 && strlen(current_arg) == 3) {
			for(int j = 1; j <= MAX_PORT; j++) /* does not includes port 0 */
				ports[j].is_active = true;

			return 1;
		} else if(strncmp(current_arg, "common", 6) == 0 && strlen(current_arg) == 6) {
			for(size_t j = 0; j < common_ports_count; j++) 
				ports[common_ports[j]].is_active = true; 
			
			return 1;
		} else {
			if(!strisnum(current_arg)) {
				LOG_ERROR("Invalid port provided, port needs to be a number 1-%d", MAX_PORT);
				goto err;
			}

			int port = strtol(current_arg, NULL, 0);

			if(port <= 0 || port > MAX_PORT) {
				LOG_ERROR("Invalid port provided, port needs to be a number 1-%d", MAX_PORT);
				goto err;
			}

			ports[port].is_active = true;
			return 1;
		}
	}	

	return 0;

err:
	exit(EXIT_FAILURE); /* fatal */
	
}

void scan_init(void) {

	struct port_t *ports = NULL;

	ports = (struct port_t *)malloc(sizeof(*ports) * (MAX_PORT + 2));
	
	if(ports == NULL) {
		LOG_ERROR("Failed to allocate ports info");
		goto err;	
	}

	ports[MAX_PORT + 1].is_active = false; 
	
	if(!__get_ports_to_scan(ports, opt_scan)) {
		LOG_ERROR("Failed to parse ports: Unexpected Error");
		goto err;
	}

	printf("Starting port scan...");
	__print_scan_debug();


err:
	free(ports);
	exit(EXIT_FAILURE);
}
