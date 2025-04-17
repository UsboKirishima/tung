#include <stdio.h>
#include <stdbool.h>

#include "args.h"

int main(int argc, char **argv) {
	struct ttts_cmd_arg_dict_item *args = args_dict_init();
	ttts_load_args(args);
	
	printf("%s\n", "Welcome to Tung Tung Tung Sahur");

	parse_full_buffer(argc, argv);
	return 0;
}
