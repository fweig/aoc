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

#ifndef NO_DLOG
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
#endif

void ilog(const char *fmt, ...)
{
	fputs("I ", stdout);
	va_list args;
	va_start(args, fmt);
	vfprintf(stdout, fmt, args);
	va_end(args);
	fputc('\n', stdout);
}

i64 asumint(array_int arr)
{
	i64 sum = 0;
	i64 *it;
	AFOR_EACH(arr, it)
		sum += *it;
	return sum;
}

i64 aminint(array_int arr)
{
	REQ(arr.size > 0);
	i64 min = FIRST(arr);
	i64 *it;
	AFOR_EACHR(arr, it, 1, -1)
		min = MIN(*it, min);
	return min;
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

array_str2d split_at_emtpy_lines(array_str lines)
{
	array_str2d section = { 0 };
	APUSH(section, (array_str){ 0 });

	char **ln;
	AFOR_EACH(lines, ln)
	{
		if (strlen(*ln) == 0) {
			APUSH(section, (array_str){ 0 });
			continue;
		}
		APUSH(LAST(section), *ln);
	}

	return section;
}

int scint(char **line, i64 *v)
{
	*v = 0;
	int advanced = 0;
	for (; **line != '\0' && isdigit(**line); ++*line) {
		i64 x = *v * 10 + (**line - '0');
		if (x < *v) {
			exit_error("Overflow parsing %lld: %lld", *v, x);
		}
		*v = x;
		advanced++;
	}
	return advanced;
}

int scintf(char **line, i64 *v)
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
