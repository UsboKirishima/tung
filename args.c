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

/* prototypes */
int8_t args_dict_add(struct ttts_cmd_arg_dict_item *dict, const uint16_t index, struct ttts_cmd_arg arg);
struct ttts_cmd_arg args_dict_get(struct ttts_cmd_arg_dict_item *args, const uint16_t index);
struct ttts_cmd_arg_dict_item *args_dict_init(void);
void args_dict_free(struct ttts_cmd_arg_dict_item *args);
void args_load_all(struct ttts_cmd_arg_dict_item *args);
void parse_cmd_args(int argc, char **argv);
char *args_extract_value(int argc, char **argv, struct ttts_cmd_arg arg_tf);
bool args_line_contains_arg(int argc, char **argv, struct ttts_cmd_arg arg_tf);
void args_parse_full_buffer(int _argc, char **_argv);

/* This function add an argument (struct ttts_cmd_arg) 
 * to the argument dictionary (pointer of struct ttts_cmd_arg_dict_item)  */
int8_t args_dict_add(struct ttts_cmd_arg_dict_item *dict, 
		const uint16_t index, struct ttts_cmd_arg arg) {
	if(index >= MAX_ARGS_NUM)
		return -1;

	dict[index].arg = arg;
	dict[index].index = index;

	return 1;
}

/* This function return a ttts_cmd_arg 
 * from dictionary by giving the index  */
struct ttts_cmd_arg args_dict_get(struct ttts_cmd_arg_dict_item *args, 
		const uint16_t index) {

	struct ttts_cmd_arg empty = {0};	
	
	if(index >= MAX_ARGS_NUM || args == NULL)
		return empty;

	return args[index].arg;
}

/* This function creates a new emtpy dictionary
 * of type `struct ttts_cmd_arg`  */
struct ttts_cmd_arg_dict_item *args_dict_init() {
	struct ttts_cmd_arg_dict_item *args = 
		(struct ttts_cmd_arg_dict_item *)malloc(sizeof(struct ttts_cmd_arg_dict_item) * MAX_ARGS_NUM);
	
	if(args != NULL)
		return args;

	return NULL;
}

void args_dict_free(struct ttts_cmd_arg_dict_item *args) {
	free(args);
}

void args_load_all(struct ttts_cmd_arg_dict_item *args) {
	args_dict_add(args, DICT_HELP_ARG, arg_help);
	args_dict_add(args, DICT_VERSION_ARG, arg_version);
	args_dict_add(args, DICT_ATTACK_ARG, arg_attack);
}

/* This function exctract the value of a 
 * specified argument from the command buffer */
char *args_extract_value(int argc, char **argv, struct ttts_cmd_arg arg_tf) {

	if(!arg_tf.expects_value)
		return NULL;
	
	for(int arg_i = 0; arg_i < argc; arg_i++) {
		if(strcmp(argv[arg_i], arg_tf.short_flag) == 0 || strcmp(argv[arg_i], arg_tf.long_flag) == 0)
			return argv[arg_i + 1];
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

	atk->atk_type = NULL;
	atk->atk_target = NULL;
	atk->atk_port = NULL;
	atk->atk_duration = 0;
	atk->atk_rate = 0;

       	if((atk->atk_type = args_extract_value(argcv, arg_attack)) == NULL) {
		LOG_ERROR("Error: Missing value in parameter `--attack`");
		exit(EXIT_FAILURE);
	}
	
	if((atk->atk_target = args_extract_value(argcv, arg_target)) == NULL) {
		LOG_ERROR("Error: Missing value in paramater `--target` or `-t`");
		exit(EXIT_FAILURE);
	}
	
	
	if((atk->atk_port = args_extract_value(argcv, arg_port)) == NULL) {
		atk->atk_port = (char *)malloc(strlen(DEFAULT_PORT) + 1);			

		if (atk->atk_port == NULL) {
			LOG_ERROR("Error: Memory allocation failed for port.");
			exit(EXIT_FAILURE);
		}
		
		strncpy(atk->atk_port, DEFAULT_PORT, strlen(DEFAULT_PORT) + 1);
	}

	if(args_extract_value(argcv, arg_duration) == NULL) {
		LOG_ERROR("Error: Missing value or parameter `--duration` or `-d`");
		exit(EXIT_FAILURE);
	}

	if((atk->atk_duration = atoi(args_extract_value(argcv, arg_duration))) == 0) {
		LOG_ERROR("Error: Missing value in parameter `--duration` or `-d`");
		exit(EXIT_FAILURE);
	}

	if (strcmp(atk->atk_type, "udp") == 0) {
		perform_udp_flood(atk);
	}	
}

void args_parse_full_buffer(int _argc, char **_argv) {
	if(args_line_contains_arg(argcv, arg_help)) show_usage();	
	if(args_line_contains_arg(argcv, arg_version)) show_version();
	if(args_line_contains_arg(argcv, arg_attack)) _parse_atks(argcv);
}

