#ifndef PARSE_H
#define PARSE_H

#define HEADER_MAGIC 0x4c4c4144
#define VERSION 1
#define EMP_STR_LEN 256

typedef struct dbheader_t {
	unsigned int magic;
	unsigned short version;
	unsigned short count;
	signed int filesize;
} dbheader_t;

typedef struct employee_t {
	char name[EMP_STR_LEN];
	char address[EMP_STR_LEN];
	unsigned int hours;
} employee_t;


void print_line(int length);
int create_db_header(dbheader_t **headerOut);
int validate_db_header(int fd, dbheader_t **headerOut);
int read_employees(int fd, dbheader_t *, employee_t **employeesOut);
int output_file(int fd, dbheader_t *, employee_t *employees);
void list_employees(dbheader_t *dbhdr, employee_t *employees);
int add_employee(dbheader_t *dbhdr, employee_t **employees, char *addstring);
int remove_employee(dbheader_t *dbhdr, employee_t *employees, char *name);
int update_hours(dbheader_t *dbhdr, employee_t *employees, char *update);

#endif
