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
// Extend functionality – Consider adding features like 
// record deletion, input validation (e.g., guard 
// 	against buffer overflow in strcpy), or richer 
// CLI feedback. Doing so will require touching the 
// main control flow and parse helpers.
// Improve tee functions individually.
// TODO: Support updating existing records through the CLI (e.g., add an -u flag and helper).​:codex-file-citation[codex-file-citation]{line_range_start=14 line_range_end=50 path=src/main.c git_url="https://github.com/maxsheffield/employee_database/blob/master/src/main.c#L14-L50"}​
// TODO: Implement employee deletion functionality (CLI flag plus record-shifting helper).​:codex-file-citation[codex-file-citation]{line_range_start=14 line_range_end=50 path=src/main.c git_url="https://github.com/maxsheffield/employee_database/blob/master/src/main.c#L14-L50"}​
// TODO: Improve validation when parsing new employee data before writing to the array.​:codex-file-citation[codex-file-citation]{line_range_start=36 line_range_end=61 path=src/parse.c git_url="https://github.com/maxsheffield/employee_database/blob/master/src/parse.c#L36-L61"}​
// TODO: Offer filtered or sorted listing views when printing employees.​:codex-file-citation[codex-file-citation]{line_range_start=12 line_range_end=25 path=src/parse.c git_url="https://github.com/maxsheffield/employee_database/blob/master/src/parse.c#L12-L25"}​
// use gzip compression to compress employees struct
// allow for adding multiple employees from 
// a file perhaps
// add unique id to employee so that same employees
// cannot be added and also allow for sorting
// look at strtok source code

/** Homework
 * Remove employee by name -r
 * way to update the hours of an employee by name
 * **/

void print_usage(char *argv[]) {
	printf("Usage: %s -n -f <database file>\n", argv[0]);
	printf("\t -n  - create new database file\n");
	printf("\t -f  - (required) path to database file\n");
	printf("\t -l  - list the employees\n");
	printf("\t -a  - add employee name,address,hours (required)\n");
	printf("\t -r  - remove employee with name (required)\n");
}

int main(int argc, char *argv[]) { 
	int c, fd = -1;
	bool newfile = false;
	bool list = false;
	bool removing = false;
	char *remove_name = NULL;
	char *filepath = NULL;
	char *addstring = NULL;
	bool adding = false;
	dbheader_t *header = NULL;
	employee_t *employees = NULL;
	bool verbose = false;

	while ((c = getopt(argc, argv, "vlnf:a:r:")) != -1) {
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
			case 'r':
				remove_name = optarg;
				removing = true;
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

	if (removing && remove_name == NULL) {
		printf("Removal name is required\n");
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
		if (add_employee(header, &employees,
					 addstring) == STATUS_ERROR) {
			printf("Failed to add employee\n");
			return 0;
		}
	}

	if (remove_name != NULL) {
		if (remove_employee(header, employees,
					remove_name) == STATUS_ERROR) {
			printf("Failed to remove employee\n");
			return 0;
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
