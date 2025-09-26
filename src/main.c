#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdlib.h>
#include <errno.h>

#include "common.h"
#include "file.h"
#include "parse.h"

// play with other getopt functions and itself
// write better tests in makefile

void print_usage(char *argv[]) {
	printf("Usage: %s -n -f <database file>\n", argv[0]);
	printf("\t -n  - create new database file\n");
	printf("\t -f  - (required) path to database file\n");
	printf("\t -l  - list the employees\n");
	printf("\t -a  - add employee name/address/hours\n");
}

int main(int argc, char *argv[]) { 
	int c, fd = -1;
	bool newfile = false;
	bool list = false;
	char *filepath = NULL;
	char *addstring = NULL;
	bool adding = false;
	dbheader_t *header = NULL;
	employee_t *employees = NULL;
	bool verbose = false;

	while ((c = getopt(argc, argv, "vlnf:a:")) != -1) {
		switch (c) {
			case 'n':
				newfile = true;
				break;
			case 'f':
				filepath = optarg;
				break;
			case 'a':
				addstring = optarg;
				adding = true;
				break;
			case 'l':
				list = true;
				break;
			case 'v':
				verbose = true;
				break;
			case '?':
				printf("Unknown option -%c\n", c);
				break;
			default:
				return STATUS_ERROR;
		}
	}

	if (adding && addstring == NULL) {
		printf("Employee string required\n");
		print_usage(argv);
		return 0;
	}

	if (filepath == NULL) {
		printf("Database path is a required argument\n");
		print_usage(argv);
		return 0;
	}

	if (verbose) {
		printf("New file: %d\n", newfile);
		printf("Filepath: %s\n", filepath);
		printf("List: %d\n", list);
		printf("New employee: %s\n", addstring);
}

	if (newfile) {
		if ((fd = create_db_file(filepath)) == \
									STATUS_ERROR) {
			printf("Unable to create db file\n");
			return -1;
		}
		else {
			if (create_db_header(&header) \
									== STATUS_ERROR) {
				printf("Unable to create db header\n");
				return -1;
			}
		}
	}
	else if ((fd = open_db_file(filepath)) == \
									STATUS_ERROR) {
		printf("Unable to open db file\n");
		return -1;
	}
	else {
		if (validate_db_header(fd, &header) == \
									STATUS_ERROR) {
			printf("Unable to validate db header\n");
			return -1;
		}
	}

	if (read_employees(fd, header, &employees)) {
		printf("Failed to read employees\n");
		return -1;
	}

	if (addstring != NULL) {
		if (add_employee(header, employees,
					 addstring) == STATUS_ERROR) {
			printf("Failed to add employee\n");
			return -1;
		}
	}


	if (list)
		list_employees(header, employees);
	
	if (output_file(fd, header, employees) \
								== STATUS_ERROR) {
		printf("Failed to output to file\n");
		return -1;
	}
	return 0;
}
