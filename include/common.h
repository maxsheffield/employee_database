#ifndef COMMON_H
#define COMMON_H

#define STATUS_ERROR   -1
#define STATUS_SUCCESS 0

#define DEBUG

void raise_error(char *line, int ln);

#define RAISE_ERROR raise_error(__FILE__, __LINE__)


#endif
