#include <stdio.h>
#include <stdlib.h>

#include "defs.h"

void show_version(void) {
	printf("Tung version n%d.%d.%d Copyright (c) 2025 Davide Usberti (333revenge)\n"
			"This software is under Apache 2.0 LICENSE all rights reserved.\n", VERSION);	
	exit(0);
}
