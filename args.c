#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "args.h"
#include "cmds.h"
#include "defs.h"

/* prototypes */
int8_t args_dict_add(struct ttts_cmd_arg_dict_item *dict, const uint16_t index, struct ttts_cmd_arg arg);
struct ttts_cmd_arg_dict_item *args_dict_init(void);
void args_load_all(struct ttts_cmd_arg_dict_item *args);
void parse_cmd_args(int argc, char **argv);
char *args_extract_value(int argc, char **argv, struct ttts_cmd_arg arg_tf);
bool args_line_contains_arg(int argc, char **argv, struct ttts_cmd_arg arg_tf);
void args_parse_full_buffer(int _argc, char **_argv);

/* This function add an argument (struct ttts_cmd_arg) 
 * to the argument dictionary (pointer of struct ttts_cmd_arg_dict_item)  */
int8_t args_dict_add(struct ttts_cmd_arg_dict_item *dict, 
		const uint16_t index, struct ttts_cmd_arg arg) {
	if(index < 0 || index >= MAX_ARGS_NUM)
		return -1;

	dict[index].arg = arg;
	dict[index].index = index;

	return 1;
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

void args_parse_full_buffer(int _argc, char **_argv) {

	#define argcv _argc, _argv

	/* Help argument */
	if(args_line_contains_arg(argcv, arg_help)) 
		return (void)printf("%s\n", "This is the help menu!");
	
	if(args_line_contains_arg(argcv, arg_version))
		return (void)printf("%s %d.%d.%d %s\n", "Tung Tung Tung Sahur",
                        VERSION, "copyright 2025 333revenge");

}

