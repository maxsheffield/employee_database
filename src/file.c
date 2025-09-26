#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "file.h"
#include "common.h"

/**
 * This function will deal with any errors that occur
 * and send the appropriate message, main will simply
 * know if an error has occurred, so we'll be saying
 * errors of the OS and logic errors from the user will
 * be dealt with the same in main by returning -1
 * **/
int create_db_file(char *filename) {
	if (!access(filename, F_OK)) {
		printf("File already exists\n");
		return STATUS_ERROR;
	}	
	int fd = open(filename, O_CREAT | O_RDWR, 0664);
	if (fd < 0) {
		RAISE_ERROR;
		return STATUS_ERROR;
	}
	else {
		return fd;
	}
}

int open_db_file(char *filename) {
	int fd = open(filename, O_RDWR);
	if (fd < 0) {
		RAISE_ERROR;
		if (errno == 2) {
			printf("File: %s not found \n", filename);
			printf("Use -n to create a new database\n");
		}
		return STATUS_ERROR;
	}
	return fd;
}


