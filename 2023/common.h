#ifndef COMMON_H
#define COMMON_H

#define _GNU_SOURCE
#include <ctype.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef int b32;

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
	} array_##name

#define AINIT(array)                                                           \
	do {                                                                   \
		(array).d = NULL;                                              \
		(array).size = 0;                                              \
		(array).capacity = 0;                                          \
	} while (0)

#define AFREE(array)                                                           \
	do {                                                                   \
		free((array).d);                                               \
		(array).d = NULL;                                              \
		(array).size = 0;                                              \
		(array).capacity = 0;                                          \
	} while (0)

#define AFREE2D(array2d)                                                       \
	do {                                                                   \
		__typeof__((array2d).d[0]) *iter;                              \
		AFOR_EACH(array2d, iter) {                                     \
			AFREE(*iter);                                          \
		}                                                              \
		AFREE(array2d);                                                \
	} while (0)

#define AT(array, index) (array).d[(index)]

#define AFOR_EACH(array, iter)                                                 \
	for ((iter) = (array).d; (iter) < (array).d + (array).size; (iter)++)

#define APUSH(array, val)                                                      \
	do {                                                                   \
		if ((array).size >= (array).capacity) {                        \
			(array).capacity = MAX((array).capacity * 2, 8ul);     \
			(array).d = realloc(                                   \
				(array).d,                                     \
				(array).capacity *                             \
					sizeof(__typeof__((array).d[0])));     \
		}                                                              \
		(array).d[(array).size++] = (val);                             \
	} while (0)

#define ARESIZE(array, new_size)                                               \
	do {                                                                   \
		(array).size = (new_size);                                     \
		if ((array).size > (array).capacity) {                         \
			(array).capacity = next_power_of_two((array).size);    \
			(array).d = realloc(                                   \
				(array).d,                                     \
				(array).capacity *                             \
					sizeof(__typeof__((array).d[0])));     \
		}                                                              \
	} while (0)

b32 testenv(char *env);
int envint(char *env);

void exit_error(const char *, ...) __attribute__((format(printf, 1, 2)));

void dlog(const char *, ...) __attribute__((format(printf, 1, 2)));
void ddlog(const char *, ...) __attribute__((format(printf, 1, 2)));
void ilog(const char *, ...) __attribute__((format(printf, 1, 2)));

ARRAY(str, char *);
void array_str_free(array_str *);

ARRAY(int, int);

/* Read file and split by lines */
array_str file_read_lines(char *);

__attribute_maybe_unused__ static size_t next_power_of_two(size_t n)
{
	return 1 << (sizeof(size_t) * 8 - __builtin_clzl(n) - 1);
}

/* String parsing */

__attribute_maybe_unused__ static b32 isend(char **line)
{
	return **line == '\0';
}

__attribute_maybe_unused__ static int advance(char **line)
{
	(*line)++;
	return 1;
}

int try_parse_int(char **line, int *v);

#endif
