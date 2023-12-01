#include "common.h"
#include <stdarg.h>

b32 testenv(char *name)
{
	char *env = getenv(name);
	if (!env) {
		return 0;
	}
	return strcmp(env, "0") != 0 && strcmp(env, "false") != 0;
}

int envint(char *name)
{
	char *env = getenv(name);
	if (!env) {
		return 0;
	}
	return atoi(env);
}

void exit_error(const char *fmt, ...)
{
	fputs("F ", stdout);
	va_list args;
	va_start(args, fmt);
	vfprintf(stdout, fmt, args);
	va_end(args);
	fputc('\n', stdout);
	exit(1);
}

void dlog(const char *fmt, ...)
{
	if (envint("DEBUG") < 1) {
		return;
	}
	fputs("D ", stdout);
	va_list args;
	va_start(args, fmt);
	vfprintf(stdout, fmt, args);
	va_end(args);
	fputc('\n', stdout);
}

void ddlog(const char *fmt, ...)
{
	if (envint("DEBUG") < 2) {
		return;
	}
	fputs("DD ", stdout);
	va_list args;
	va_start(args, fmt);
	vfprintf(stdout, fmt, args);
	va_end(args);
	fputc('\n', stdout);
}

void ilog(const char *fmt, ...)
{
	fputs("I ", stdout);
	va_list args;
	va_start(args, fmt);
	vfprintf(stdout, fmt, args);
	va_end(args);
	fputc('\n', stdout);
}

void array_str_free(array_str *array)
{
	size_t i;
	for (i = 0; i < array->size; i++)
		free(array->d[i]);
	AFREE(*array);
}

array_str file_read_lines(char *fname)
{
	array_str lines;
	AINIT(lines);

	FILE *f = fopen(fname, "r");
	if (!f) {
		perror("fopen");
		exit(1);
	}

	char *line = NULL;
	size_t linecap = 0;
	ssize_t linelen;
	while ((linelen = getline(&line, &linecap, f)) > 0) {
		if (line[linelen - 1] == '\n')
			line[linelen - 1] = '\0';
		APUSH(lines, line);
		line = NULL;
		linecap = 0;
	}

	free(line);
	fclose(f);
	return lines;
}
