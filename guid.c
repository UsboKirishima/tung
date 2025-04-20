#include <stdio.h>
#include <unistd.h>

#include "tung.h"
#include "logs.h"

int is_root() {
	return (getuid() == 0);
}

void check_root() {
	if(is_root()) return;
	LOG_ERROR("This function requires root privilages");
	exit(EXIT_FAILURE);
}
