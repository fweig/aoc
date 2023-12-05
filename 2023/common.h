#ifndef COMMON_H
#define COMMON_H

#define _GNU_SOURCE
#include <ctype.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define AOC_MAYBE_UNUSED __attribute_maybe_unused__
#define AOC_NO_RETURN __attribute__((noreturn))

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

#define AT(array, index) (array).d[(index)]
#define EMPTY(array) ((array).size == 0)

#define VTYPE(array) __typeof__((array).d[0]) /* value type of array */
#define VSIZE(array) (sizeof(VTYPE(array))) /* value size of array */

#define AFOR_EACH(array, iter)                                                 \
	for ((iter) = (array).d; (iter) < (array).d + (array).size; (iter)++)

#define AMAP(iarr, oarr, fn)                                                   \
	do {                                                                   \
		REQUIRE(EMPTY(oarr));                                          \
		VTYPE(iarr) *iter;                                             \
		AFOR_EACH(iarr, iter)                                          \
			APUSH(oarr, fn(*iter));                                \
	} while (0)

void arr_adjust_capacity(size_t new_size, size_t vsize, size_t *capacity,
			 void **data);

#define APUSH(array, val)                                                      \
	do {                                                                   \
		arr_adjust_capacity((array).size + 1, VSIZE(array),            \
				    &(array).capacity, (void **)&(array).d);   \
		(array).d[(array).size++] = (val);                             \
	} while (0)

#define ARESIZE(array, new_size)                                               \
	do {                                                                   \
		arr_adjust_capacity((new_size), VSIZE(array),                  \
				    &(array).capacity, (void **)&(array).d);   \
		(array).size = (new_size);                                     \
	} while (0)

#define ACONTAINS(array, elem)                                                 \
	({                                                                     \
		__typeof__(elem) _elem = (elem);                               \
		b32 has_elem = 0;                                              \
		VTYPE(array) *it;                                              \
		AFOR_EACH(array, it) {                                         \
			if (*it == _elem) {                                    \
				has_elem = 1;                                  \
				break;                                         \
			}                                                      \
		}                                                              \
		has_elem;                                                      \
	})

#define ACOUNT(array, test)                                                    \
	({                                                                     \
		int cnt = 0;                                                   \
		VTYPE(array) *it;                                              \
		AFOR_EACH(array, it) {                                         \
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
#define REQUIRE(test)                                                          \
	((test) ? (void)0 : require_fail(__FILE__, __LINE__, #test))

AOC_NO_RETURN void exit_error(const char *, ...)
	__attribute__((format(printf, 1, 2)));

void dlog(const char *, ...) __attribute__((format(printf, 1, 2)));
void ddlog(const char *, ...) __attribute__((format(printf, 1, 2)));
void ilog(const char *, ...) __attribute__((format(printf, 1, 2)));

ARRAY(str, char *);
ARRAY(int, int);

int asumint(array_int);

/* Read file and split by lines */
array_str file_read_lines(char *);

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

int scint(char **line, int *v); /* string try consume int */
int scintf(char **line, int *v); /* string consume int or fail */
int scuntil(char **line, char c); /* string consume until character */
int scw(char **line); /* string consume whitespace */
int scchar(char **line, char c); /* string try consume char */
int scstr(char **line, char *str); /* string try consume substring */
int scstrf(char **line, char *str); /* string consume substring or fail */

#endif
