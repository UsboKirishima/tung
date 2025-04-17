#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "args.h"


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

void ttts_load_args(struct ttts_cmd_arg_dict_item *args) {

	/* Example arg */
	struct ttts_cmd_arg arg_example = {
        	.short_flag = "-v",
        	.long_flag = "--verbose",
        	.description = "Enable verbose output",
        	.expects_value = false,
        	.value_name = NULL,
        	.default_value = NULL,
        	.is_required = false,
        	.is_repeatable = true
    	};
	args_dict_add(args, DICT_EX_ARG, arg_example);
}

/* TODO: 
 * 	1. Line handling and arg scanning
 * 	2. Value validation 
 * 	3. Callback function maybe
 * 	4. Move this code to another file maybe `args.c` */
void parse_cmd_args(int argc, char **argv);
void print_help();

