#include <stdio.h>
#include <errno.h>
#include <stdlib.h>

#include "common.h"

void raise_error(char *line, int ln) {
	static char buffer[1024] = {};
	#ifdef DEBUG
	snprintf(buffer, 1023, "%d : %s : %d", ln,
				line, errno);
	perror(buffer);
	return;
	#endif
	perror("");
} 