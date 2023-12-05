#include "common.h"
#include <stdarg.h>

ARRAY(ptr, void *);

static array_ptr H = { 0 };

void *ymalloc(size_t bytes)
{
	void *ptr = malloc(bytes);
	yput(ptr);
	return ptr;
}

void yput(void *ptr)
{
	APUSH(H, ptr);
}

void *yrealloc(void *oldptr, size_t bytes)
{
	ddlog("yrealloc: %zu", bytes);
	void *newptr = realloc(oldptr, bytes);
	void **it;
	AFOR_EACH(H, it) {
		if (*it == oldptr)
			*it = newptr;
	}
	return newptr;
}

void yfree()
{
	void **it;
	AFOR_EACH(H, it)
		free(*it);
	free(H.d);
	H = (array_ptr){ 0 };
}

void arr_adjust_capacity(size_t new_size, size_t vsize, size_t *capacity, void **data)
{
	if (new_size <= *capacity)
		return;
	new_size = next_power_of_two(new_size);
	*capacity = MAX(new_size, 8ul);
	*data = yrealloc(*data, *capacity * vsize);
}

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

void require_fail(char *file, int line, char *test)
{
	exit_error("%s:%d: Precondition '%s' failed!", file, line, test);
}

void exit_error(const char *fmt, ...)
{
	fputs("F ", stdout);
	va_list args;
	va_start(args, fmt);
	vfprintf(stdout, fmt, args);
	va_end(args);
	fputc('\n', stdout);
	abort();
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

int asumint(array_int arr)
{
	int sum = 0;
	int *it;
	AFOR_EACH(arr, it)
		sum += *it;
	return sum;
}

array_str file_read_lines(char *fname)
{
	array_str lines = { 0 };

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
		yput(line);
		APUSH(lines, line);
		line = NULL;
		linecap = 0;
	}

	fclose(f);
	return lines;
}

int scint(char **line, int *v)
{
	*v = 0;
	int advanced = 0;
	for (; **line != '\0' && isdigit(**line); ++*line) {
		*v = *v * 10 + (**line - '0');
		advanced++;
	}
	return advanced;
}

int scintf(char **line, int *v)
{
	int consumed = scint(line, v);
	if (consumed == 0)
		exit_error("Failed to parse int: '%s'", *line);
	return consumed;
}

int scuntil(char **line, char c)
{
	int consumed = 0;
	while (peek(line) != c && !isend(line)) {
		consumed += scadv(line);
	}
	consumed += scadv(line); /* Read past c */
	return consumed;
}

int scw(char **line)
{
	int consumed = 0;
	while (isspace(peek(line)) && !isend(line)) {
		consumed += scadv(line);
	}
	return consumed;
}

int scchar(char **line, char c)
{
	if (peek(line) == c)
		return scadv(line);
	return 0;
}

int scstr(char **line, char *str)
{
	size_t len_prefix = strlen(str);
	if (strncmp(*line, str, len_prefix) != 0)
		return 0;
	*line += len_prefix;
	return len_prefix;
}

int scstrf(char **line, char *str)
{
	int consumed = scstr(line, str);
	if (consumed == 0)
		exit_error("Failed to read prefix '%s': '%s'", str, *line);
	return consumed;
}
