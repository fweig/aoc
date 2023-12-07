#ifndef AOC_H
#define AOC_H

#define _GNU_SOURCE
#include <ctype.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define AOC_MAYBE_UNUSED __attribute_maybe_unused__
#define AOC_NO_RETURN __attribute__((noreturn))

typedef int b32;
typedef long long int i64;

void task1(char *input_file);
void task2(char *input_file);

int main(int argc, char *argv[]);

#define MAX(a, b)                                                              \
	({                                                                     \
		__typeof__(a) _a = (a);                                        \
		__typeof__(b) _b = (b);                                        \
		_a > _b ? _a : _b;                                             \
	})

#define MIN(a, b)                                                              \
	({                                                                     \
		__typeof__(a) _a = (a);                                        \
		__typeof__(b) _b = (b);                                        \
		_a < _b ? _a : _b;                                             \
	})

#define ARRAY(name, type)                                                      \
	typedef struct {                                                       \
		type *d;                                                       \
		size_t size;                                                   \
		size_t capacity;                                               \
	} arr_##name

#define curry_finish(...) __VA_ARGS__)
#define curry(fn, arg1) fn(arg1, curry_finish

#define at(array, index) (array).d[(index)]
#define first(array) at(array, 0)
#define last(array) at(array, (array).size - 1)
#define empty(array) ((array).size == 0)

#define vtype(array) __typeof__((array).d[0]) /* value type of array */
#define vsize(array) (sizeof(vtype(array))) /* value size of array */

#define aforeach(array, iter)                                                  \
	for ((iter) = (array).d; (iter) < (array).d + (array).size; (iter)++)

#define aforeachr(array, iter, offset, xsize)                                  \
	for ((iter) = &at(array, offset);                                      \
	     (iter) < (array).d + (xsize < 0 ? (array).size : (size_t)xsize);  \
	     (iter)++)

#define amap(iarr, oarr, fn)                                                   \
	do {                                                                   \
		req(empty(oarr));                                              \
		vtype(iarr) *iter;                                             \
		aforeach(iarr, iter)                                           \
			apush(oarr, fn(*iter));                                \
	} while (0)

#define amap2(iarr1, iarr2, oarr, fn)                                          \
	do {                                                                   \
		req(empty(oarr));                                              \
		size_t i;                                                      \
		for (i = 0; i < MIN((iarr1).size, (iarr2).size); i++)          \
			apush(oarr, fn(at((iarr1), i), at((iarr2), i)));       \
	} while (0)

#define amapr(iarr, oarr, fn, offset, size)                                    \
	do {                                                                   \
		req(empty(oarr));                                              \
		vtype(iarr) *iter;                                             \
		aforeachr(iarr, iter, offset, size)                            \
			apush(oarr, fn(*iter));                                \
	} while (0)

void arr_adjust_capacity(size_t new_size, size_t vsizex, size_t *capacity,
			 void **data);

#define apush(array, val)                                                      \
	do {                                                                   \
		arr_adjust_capacity((array).size + 1, vsize(array),            \
				    &(array).capacity, (void **)&(array).d);   \
		(array).d[(array).size++] = (val);                             \
	} while (0)

#define aresize(array, new_size)                                               \
	do {                                                                   \
		arr_adjust_capacity((new_size), vsize(array),                  \
				    &(array).capacity, (void **)&(array).d);   \
		(array).size = (new_size);                                     \
	} while (0)

#define acontains(array, elem)                                                 \
	({                                                                     \
		__typeof__(elem) _elem = (elem);                               \
		b32 has_elem = 0;                                              \
		vtype(array) *it;                                              \
		aforeach(array, it) {                                          \
			if (*it == _elem) {                                    \
				has_elem = 1;                                  \
				break;                                         \
			}                                                      \
		}                                                              \
		has_elem;                                                      \
	})

#define areduce(array, op)                                                     \
	({                                                                     \
		__typeof__(array) _arr = (array);                              \
		req(!empty(_arr));                                             \
		vtype(_arr) result = first(_arr);                              \
		vtype(_arr) *it;                                               \
		aforeachr(_arr, it, 1, -1)                                     \
			result = op(result, *it);                              \
		result;                                                        \
	})

#define amin(array) areduce(array, MIN)
#define amax(array) areduce(array, MAX)

#define _add(a, b) (a) + (b)
#define asum(array) areduce(array, _add)
#define _mul(a, b) (a) * (b)
#define amul(array) areduce(array, _mul)

#define acount(array, test)                                                    \
	({                                                                     \
		int cnt = 0;                                                   \
		vtype(array) *it;                                              \
		aforeach(array, it) {                                          \
			if (test(*it))                                         \
				cnt++;                                         \
		}                                                              \
		cnt;                                                           \
	})

void *ymalloc(size_t); /* yolo malloc */
void yput(void *);
void *yrealloc(void *, size_t); /* yolo realloc */
void yfree(); /* Free entire heap */

b32 testenv(char *env);
int envint(char *env);

void require_fail(char *, int, char *);
#define req(test) ((test) ? (void)0 : require_fail(__FILE__, __LINE__, #test))

AOC_NO_RETURN void exit_error(const char *, ...)
	__attribute__((format(printf, 1, 2)));

#ifdef NO_DLOG
#define dlog(...)
#define ddlog(...)
#else
void dlog(const char *, ...) __attribute__((format(printf, 1, 2)));
void ddlog(const char *, ...) __attribute__((format(printf, 1, 2)));
#endif
void ilog(const char *, ...) __attribute__((format(printf, 1, 2)));

ARRAY(str, char *);
ARRAY(str2d, arr_str);
ARRAY(int, i64);

/* Read file and split by lines */
arr_str file_read_lines(char *);
arr_str2d split_at_emtpy_lines(arr_str);

AOC_MAYBE_UNUSED static size_t next_power_of_two(size_t n)
{
	return 1 << (sizeof(size_t) * 8 - __builtin_clzl(n));
}

/* String parsing */

AOC_MAYBE_UNUSED static b32 isend(char **line)
{
	return **line == '\0';
}

AOC_MAYBE_UNUSED static char peek(char **line)
{
	return **line;
}

AOC_MAYBE_UNUSED static int scadv(char **line)
{
	if (isend(line))
		return 0;
	(*line)++;
	return 1;
}

int scint(char **line, i64 *v); /* string try consume int */
int scintf(char **line, i64 *v); /* string consume int or fail */
int scints(char **line, arr_int *); /* consume list of integers */
int scuntil(char **line, char c); /* string consume until character */
int scw(char **line); /* string consume whitespace */
int scchar(char **line, char c); /* string try consume char */
int scstr(char **line, char *str); /* string try consume substring */
int scstrf(char **line, char *str); /* string consume substring or fail */

#ifdef AOC_IMPL
int main(int argc, char *argv[])
{
	if (argc != 2) {
		printf("Usage: %s <input_file>\n", argv[0]);
		return 1;
	}
	char *input_file = argv[1];
	if (!testenv("NOTASK1")) {
		ilog(">>> Task1");
		task1(input_file);
	}
	yfree();
	if (!testenv("NOTASK2")) {
		ilog(">>> Task2");
		task2(input_file);
	}
	yfree();
	return 0;
}

ARRAY(ptr, void *);

static arr_ptr H = { 0 };

void *ymalloc(size_t bytes)
{
	void *ptr = malloc(bytes);
	yput(ptr);
	return ptr;
}

void yput(void *ptr)
{
	apush(H, ptr);
}

void *yrealloc(void *oldptr, size_t bytes)
{
	ddlog("yrealloc: %zu", bytes);
	void *newptr = realloc(oldptr, bytes);
	void **it;
	aforeach(H, it) {
		if (*it == oldptr)
			*it = newptr;
	}
	return newptr;
}

void yfree()
{
	void **it;
	aforeach(H, it)
		free(*it);
	free(H.d);
	H = (arr_ptr){ 0 };
}

void arr_adjust_capacity(size_t new_size, size_t vsize, size_t *capacity,
			 void **data)
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

arr_str file_read_lines(char *fname)
{
	arr_str lines = { 0 };

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
		apush(lines, line);
		line = NULL;
		linecap = 0;
	}

	fclose(f);
	return lines;
}

arr_str2d split_at_emtpy_lines(arr_str lines)
{
	arr_str2d section = { 0 };
	apush(section, (arr_str){ 0 });

	char **ln;
	aforeach(lines, ln) {
		if (isend(ln)) {
			apush(section, (arr_str){ 0 });
			continue;
		}
		apush(last(section), *ln);
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

int scints(char **line, arr_int *out)
{
	int consumed = 0;
	int consumed_int = 0;
	do {
		consumed += scw(line);
		i64 val;
		consumed_int = scint(line, &val);
		consumed += consumed_int;
		if (consumed_int > 0) {
			apush(*out, val);
		}
	} while (consumed_int > 0);
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

#endif /* defined AOC_IMPL */
#endif /* defined AOC_H */
