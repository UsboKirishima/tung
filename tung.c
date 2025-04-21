#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include "args.h"

int main(int argc, char **argv) {
	
	srand(time(NULL) ^ getpid());
	
	args_parse_full_buffer(argc, argv);

	return 0;
}
