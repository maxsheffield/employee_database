#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

#include "common.h"
#include "parse.h"

void list_employees(dbheader_t *dbhdr, 
							employee_t *employees) {
	unsigned short line_length = 50;
	char *title = "Employees:";
	printf("%s", title);
	print_line(line_length - strlen(title));
	unsigned short employees_count = dbhdr->count;
	for (int i = 0; i < employees_count; i++) {
		employee_t *emp = employees + i;
		printf("Name:\t\t%s\n", emp->name);
		printf("Address:\t%s\n", emp->address);
		printf("Hours:\t\t%u\n", emp->hours);
		print_line(line_length);
	}
}

void print_line(int length) {
	char buffer[length + 2];
	memset(buffer, '-', length);
	memset(buffer + length, '\n', 1);
	memset(buffer + length + 1, '\0', 1);
	printf("%s", buffer);
}

int add_employee(dbheader_t *dbhdr, employee_t *employees, 
								char *addstring) {
	employee_t emp = {};
	static char *delim = ".,";
	char *tmp = NULL;
	if ((tmp = strtok(addstring, delim)) == NULL) {
		printf("No name given\n");
		return STATUS_ERROR;
	}
	strcpy(emp.name, tmp);
	if ((tmp = strtok(NULL, delim)) == NULL) {
		printf("No address given\n");
		return STATUS_ERROR;
	}
	strcpy(emp.address, tmp);
	char *end_ptr = NULL;
	emp.hours = strtol(strtok(NULL, delim),
									&end_ptr, 10);
	// Could overflow emp.hours
	if (*end_ptr != '\0') {
		printf("No hours given\n");
		return STATUS_ERROR;
	}
	*(employees + dbhdr->count) = emp;
	(dbhdr->count)++;
	dbhdr->filesize += sizeof(employee_t);

	return STATUS_SUCCESS;
}


int read_employees(int fd, dbheader_t *dbhdr,
						 employee_t **employeesOut) {
	unsigned short count = dbhdr->count;
	lseek(fd, sizeof(dbheader_t), SEEK_SET);
	*employeesOut = malloc(sizeof(employee_t) * (count+10));
	if (*employeesOut == NULL) {
		RAISE_ERROR;
		return STATUS_ERROR;
	}
	for (int i = 0; i < dbhdr->count; i++) {
		if (read(fd, (*employeesOut) + i, 
		   sizeof(employee_t)) != sizeof(employee_t)) {
			RAISE_ERROR;
			return STATUS_ERROR;
		}
		(*employeesOut + i)->hours = ntohl(
					(*employeesOut + i)->hours);
	}
	return STATUS_SUCCESS;
}

int output_file(int fd, dbheader_t *dbhdr, 
							employee_t *employees) {
	if (fd < 0) {
		RAISE_ERROR;
		return STATUS_ERROR;
	}
	unsigned short host_count = dbhdr->count;

	dbhdr->magic = htonl(dbhdr->magic);
	dbhdr->version = htons(dbhdr->version);
	dbhdr->count = htons(dbhdr->count);
	dbhdr->filesize = htonl(dbhdr->filesize);

	if(lseek(fd, 0, SEEK_SET)) {
		RAISE_ERROR;
		return STATUS_ERROR;
	}

	if (write(fd, dbhdr, sizeof(dbheader_t)) \
					!= sizeof(dbheader_t)) {
		RAISE_ERROR;
		return STATUS_ERROR;
	}

	int size = sizeof(employee_t) * host_count;
	for (int i = 0; i < host_count; i++) {
		(employees + i)->hours = htonl(
				(employees + i)->hours);
	}

	if (write(fd, employees, size) \
						 != size) {
			RAISE_ERROR;
			return STATUS_ERROR;
	}

	return STATUS_SUCCESS;
}	
/**To create validate the header of a file**/
int validate_db_header(int fd, dbheader_t **headerOut) {
	if (fd < 0) {
		RAISE_ERROR;
		return STATUS_ERROR;
	}

	*headerOut = malloc(sizeof(dbheader_t));

	if (*headerOut == NULL) {
		RAISE_ERROR;
		return STATUS_ERROR;
	}

	if (lseek(fd, 0, SEEK_SET) != 0) {
		RAISE_ERROR;
		return STATUS_ERROR;
	}

	if (read(fd, *headerOut, sizeof(dbheader_t)) \
							!= sizeof(dbheader_t)) {
		RAISE_ERROR;
		return STATUS_ERROR;
	}	

	(*headerOut)->version = ntohs((*headerOut)->version);
	(*headerOut)->count = ntohs((*headerOut)->count);
	(*headerOut)->filesize = ntohl((*headerOut)->filesize);
	(*headerOut)->magic = ntohl((*headerOut)->magic);

	if ((*headerOut)->magic != HEADER_MAGIC) {
		printf("0x%08x != 0x%08x\n", 
					(*headerOut)->magic, HEADER_MAGIC);
		printf("Invalid database file\n");
		free(*headerOut);
		return STATUS_ERROR;
	}

	struct stat dbstat = {};
	if (fstat(fd, &dbstat)) {
		RAISE_ERROR;
		return STATUS_ERROR;
	}

	if (dbstat.st_size != (*headerOut)->filesize) {
		printf("Header says incorrect size\n");
		free(*headerOut);
		return STATUS_ERROR;
	}

	if ((*headerOut)->version != VERSION) {
		printf("Program version: %u, db version: %u\n",
					VERSION, (*headerOut)->version);
		free(*headerOut);
		return STATUS_ERROR;
	}

	return STATUS_SUCCESS;
}
/**To create a new header file from scratch**/
int create_db_header(dbheader_t **headerOut) {
	*headerOut = malloc(sizeof(dbheader_t));
	if (*headerOut == NULL) {
		RAISE_ERROR;
		return STATUS_ERROR;
	}
	(*headerOut)->magic = HEADER_MAGIC;
	(*headerOut)->version = VERSION;
	(*headerOut)->count = 0;
	(*headerOut)->filesize = sizeof(dbheader_t); 


	return STATUS_SUCCESS;
}


