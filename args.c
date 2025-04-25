#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "args.h"
#include "cmds.h"
#include "defs.h"
#include "tung.h"
#include "logs.h"

/* cli options  */
bool opt_verbose = false; 
char *opt_target = NULL; 
uint16_t opt_port = 0, 
	 opt_duration = 0, 
	 opt_rate = 0;

/* prototypes */
void parse_cmd_args(int argc, char **argv);
char *args_extract_value(int argc, char **argv, struct ttts_cmd_arg arg_tf);
bool args_line_contains_arg(int argc, char **argv, struct ttts_cmd_arg arg_tf);
void args_parse_full_buffer(int _argc, char **_argv);

/* This function exctract the value of a 
 * specified argument from the command buffer */
char *args_extract_value(int argc, char **argv, struct ttts_cmd_arg arg_tf) {

	if(!arg_tf.expects_value)
		return NULL;
	
	for(int arg_i = 0; arg_i < argc; arg_i++) {
		if(strcmp(argv[arg_i], arg_tf.short_flag) == 0 || strcmp(argv[arg_i], arg_tf.long_flag) == 0) {
			if (arg_i + 1 < argc)
				return argv[arg_i + 1];
			
			return NULL;
		}
	}

	return NULL;
}

/* This function returns true if an argument 
 * is contained in the command line command */
bool args_line_contains_arg(int argc, char **argv, struct ttts_cmd_arg arg_tf) {
	for(int arg_i = 0; arg_i < argc; arg_i++) {
		if(strcmp(argv[arg_i], arg_tf.short_flag) == 0 || strcmp(argv[arg_i], arg_tf.long_flag) == 0)
			return true;
	}

	return false;
}

#define argcv _argc, _argv

static void _parse_atks(int _argc, char **_argv) {
	struct attack_opts_t *atk = (struct attack_opts_t *)malloc(sizeof(struct attack_opts_t));
	
	if (atk == NULL) {
		LOG_ERROR("Error: Memory allocation failed for attack options.");
		exit(EXIT_FAILURE);
	}

	char port[6];
	int cp_port = snprintf(port, sizeof(port), "%u", opt_port);

	if(cp_port < 0 && cp_port >= (int)sizeof(port)) {
		LOG_ERROR("Failed to parse port (from integer to string)");
		free(atk);
		exit(EXIT_FAILURE);
	}

	atk->atk_type = NULL;
	atk->atk_target = opt_target;
	atk->atk_port = strdup(port);
	atk->atk_duration = opt_duration;
	atk->atk_rate = opt_rate;

	if (atk->atk_port == NULL) {
		LOG_ERROR("Error: Memory allocation failed for port string");
		free(atk);
		exit(EXIT_FAILURE);
	}

       	if((atk->atk_type = args_extract_value(argcv, arg_attack)) == NULL) {
		LOG_ERROR("Error: Missing value in parameter `--attack`");
		free(atk->atk_port);
		free(atk);
		exit(EXIT_FAILURE);
	}

	/* check if ip addr is valid  */
	if(!validate_ip(atk->atk_target)) {
		LOG_ERROR("Invalid Ip Address: %s", atk->atk_target);
		free(atk->atk_port);
		free(atk);
		exit(EXIT_FAILURE);
	}

	/* check if target is up  */
	if(!is_target_up(atk->atk_target, atoi(atk->atk_port))) {
		LOG_ERROR("Failed to connect to %s (host is down)", atk->atk_target);
		free(atk->atk_port);
		free(atk);
		exit(EXIT_FAILURE);
	}

	bool attack_executed = true;

	/* TODO: Add target_addr resolve to ip addr */

	if (strcmp(atk->atk_type, "udp") == 0) perform_udp_flood(atk);
	else if(strcmp(atk->atk_type, "ampl") == 0) perform_udp_ampl(atk);
	else if(strcmp(atk->atk_type, "fraggle") == 0) perform_udp_fraggle(atk);
	else if(strcmp(atk->atk_type, "ald") == 0) perform_udp_app_layer_dos(atk);
	else if(strcmp(atk->atk_type, "sockex") == 0) perform_udp_socket_exhaustion(atk);
	else if(strcmp(atk->atk_type, "syn") == 0) syn_flood(atk);
	else if(strcmp(atk->atk_type, "ack") == 0) ack_flood(atk);
	else if(strcmp(atk->atk_type, "rst") == 0) rst_flood(atk);
	else if(strcmp(atk->atk_type, "fin") == 0) fin_flood(atk);
	else if(strcmp(atk->atk_type, "psh_ack") == 0) psh_ack_flood(atk);
	else if(strcmp(atk->atk_type, "xmas") == 0) xmas_flood(atk);
	else if(strcmp(atk->atk_type, "null") == 0) null_flood(atk);
	else {
		LOG_ERROR("Invalid Attack: %s", atk->atk_type);
		attack_executed = false;
		free(atk->atk_port);
		free(atk);
		exit(EXIT_FAILURE);
	}

	if(attack_executed) {
		free(atk->atk_port);
		free(atk);
	}
}

#define __CONTAINS(arg) args_line_contains_arg(argcv, arg)
#define __EXTRACT(arg) args_extract_value(argcv, arg)

void args_parse_full_buffer(int _argc, char **_argv) {
	if(__CONTAINS(arg_verbose)) opt_verbose = true;
	
	if(__CONTAINS(arg_attack)) {
		if((opt_target = __EXTRACT(arg_target)) == NULL) {
			LOG_ERROR("Error: Missing value in paramater `--target` or `-t`");
			exit(EXIT_FAILURE);
		}
		       
		char *port_str = __EXTRACT(arg_port);
		if(port_str == NULL || (opt_port = (uint16_t)atoi(port_str)) == 0) {
			LOG_ERROR("Error: Missing or invalid value in param --port");
			exit(EXIT_FAILURE);
		}
		
		char *duration_str = __EXTRACT(arg_duration);
		if(duration_str == NULL || (opt_duration = (uint16_t)atoi(duration_str)) == 0) {
			opt_duration = UINT16_MAX;
		}
	
		char *rate_str = __EXTRACT(arg_rate);
		opt_rate = rate_str ? (uint16_t)atoi(rate_str) : 0;
	}
	

	if(args_line_contains_arg(argcv, arg_help)) show_usage(_argv);
	else if(args_line_contains_arg(argcv, arg_version)) show_version();
	else if(args_line_contains_arg(argcv, arg_list)) show_attacks_list(_argv);
	else if(args_line_contains_arg(argcv, arg_attack)) _parse_atks(argcv);
	else {
		LOG_ERROR("No valid command specified. Use --help for usage information.");
		exit(EXIT_FAILURE);
	}
}

