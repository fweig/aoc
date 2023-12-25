#ifndef AOC_H
#define AOC_H

#include <omp.h>

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <ctype.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <new>

#define T_ template <class T>

// clang-format off
#define continue_if(b) if (b) continue
// clang-format on

T_ struct arr;
T_ struct arr2d;
T_ struct fv;

using i64 = int64_t;
using iarr = arr<i64>;
using sarr = arr<char *>;
using carr = arr<char>;
using carr2d = arr2d<char>;
using cfv = fv<char>;

int main(int argc, char *argv[]);
i64 task1(char *input_file);
i64 task2(char *input_file);

void require_fail(char *, int, char *);
#define req(test) ((test) ? (void)0 : require_fail(__FILE__, __LINE__, #test))

void *ymalloc(size_t); /* yolo malloc */
void yput(void *);
void *yrealloc(void *, size_t); /* yolo realloc */
void yfree(); /* Free entire heap */

bool testenv(char *env);
int envint(char *env);

T_ T max(T a, T b) { return a > b ? a : b; }
T_ T min(T a, T b) { return a < b ? a : b; }

T_ struct arr_ctx {
  i64 len = 0;
  i64 capacity = 0;
  T *data = nullptr;
};

T_ struct arr {
  using vtype = T;
  static constexpr i64 vsize = sizeof(T);

  arr_ctx<T> *ctx;

  arr()
  {
    ctx = (arr_ctx<T> *)ymalloc(sizeof(arr_ctx<T>));
    *ctx = { 0 };
  }

  T &operator[](i64 i)
  {
    if (i >= ctx->len || i < 0)
      abort();
    return ctx->data[i];
  }

  i64 capacity() { return ctx->capacity; }
  T *data() { return ctx->data; }
};

#define foreach(a) foreach_(a, it)
#define foreach_(a, iter) for (auto *iter = begin(a); iter != end(a); iter++)

#define foreachr(array, offset, xsize)                                         \
  for (auto *it = &array[offset];                                              \
       it !=                                                                   \
       begin(array) + (xsize < 0 ? len(array) : (i64)((xsize) + (offset)));    \
       it++)

T_ i64 len(arr<T> a) { return a.ctx->len; }
T_ T &first(arr<T> a) { return a[0]; }
T_ T &last(arr<T> a) { return a[len(a) - 1]; }
T_ bool empty(arr<T> a) { return len(a) == 0; }
T_ i64 index(arr<T> a, T *it) { return it - a.data(); }
T_ T *end(arr<T> a) { return a.data() + len(a); }
T_ T *begin(arr<T> a) { return a.data(); }
T_ bool operator==(arr<T> a, arr<T> b)
{
  if (len(a) != len(b))
    return false;
  for (i64 i = 0; i < len(a); i++) {
    if (a[i] == b[i])
      continue;
    return false;
  }
  return true;
}

T_ i64 indexof(arr<T> a, T x)
{
  for (i64 i = 0; i < len(a); i++) {
    if (a[i] == x)
      return i;
  }
  return -1;
}

void arr_adjust_capacity(i64 new_size, i64 vsizex, i64 *capacity, void **data);

T_ void put(arr<T> a, T val)
{
  arr_adjust_capacity(len(a) + 1, arr<T>::vsize, &a.ctx->capacity,
                      (void **)&a.ctx->data);
  a.ctx->data[a.ctx->len++] = val;
}

T_ void resize(arr<T> a, i64 new_size)
{
  arr_adjust_capacity(new_size, arr<T>::vsize, &a.ctx->capacity,
                      (void **)&a.ctx->data);
  a.ctx->len = new_size;
}

T_ void resize_init(arr<T> a, i64 new_size)
{
  auto oldlen = len(a);
  resize(a, new_size);
  if (new_size <= oldlen)
    return;

  foreachr(a, oldlen, -1) {
    new (it) T{}; // placement new, ugh
  }
}

T_ void remove(arr<T> a, T v)
{
  auto i = indexof(a, v);
  if (i == -1)
    return;
  if (i != len(a) - 1)
    memmove(&a[i], &a[i + 1], sizeof(T) * len(a) - i - 1);
  a.ctx->len--;
}

struct idx2 {
  i64 x = 0;
  i64 y = 0;

  idx2() = default;

  constexpr idx2(i64 _x, i64 _y)
      : x(_x)
      , y(_y)
  {
  }

  idx2 &operator+=(idx2 o)
  {
    x += o.x;
    y += o.y;
    return *this;
  }

  idx2 operator+(idx2 o)
  {
    idx2 r = *this;
    return r += o;
  }

  idx2 &operator-=(idx2 o)
  {
    x -= o.x;
    y -= o.y;
    return *this;
  }

  idx2 operator-(idx2 o)
  {
    idx2 r = *this;
    return r -= o;
  }

  bool operator==(idx2 o) { return x == o.x && y == o.y; }
};

i64 index2d(idx2 i, idx2 dim) { return i.y * dim.x + i.x; }

bool inside(idx2 i, idx2 dim)
{
  return i.x >= 0 && i.x < dim.x && i.y >= 0 && i.y < dim.y;
}

T_ struct arr2d {
  using vtype = T;
  static constexpr i64 vsize = sizeof(T);

  T *data = nullptr;
  idx2 dim;

  arr2d(idx2 d)
      : dim(d)
  {
    data = (T *)ymalloc(dim.x * dim.y * vsize);
  }

  arr2d(i64 w, i64 h)
      : arr2d(idx2{ w, h })
  {
  }

  T &operator()(idx2 i)
  {
    if (!inside(i, dim))
      abort();
    return data[index2d(i, dim)];
  }

  T &operator()(i64 x, i64 y) { return (*this)(idx2{ x, y }); }

  bool operator==(arr2d<T> b)
  {
    if (dim != b.dim)
      return false;
    bool eq = true;
    for (i64 i = 0; i < dim.x * dim.y; i++)
      eq &= data[i] == b.data[i];
    return eq;
  }
};

// flat view
T_ struct fv {
  T *data = nullptr;
  i64 len = nullptr;
  i64 stride = nullptr;

  T &operator[](i64 i)
  {
    if (i < 0 || i >= len)
      abort();
    return data[i * stride];
  }
};

T_ i64 len(arr2d<T> a) { return a.dim.x * a.dim.y; }
T_ i64 len(fv<T> a) { return a.len; }
T_ i64 width(arr2d<T> a) { return a.dim.x; }
T_ i64 height(arr2d<T> a) { return a.dim.y; }
T_ void fill(arr2d<T> a, T v)
{
  for (i64 i = 0; i < width(a) * height(a); i++) {
    new (&a.data[i]) T{ v };
  }
}
T_ T last(fv<T> a) { return a[len(a) - 1]; }
T_ arr2d<T> copy(arr2d<T> a)
{
  arr2d<T> b(width(a), height(a));
  memcpy(b.data, a.data, len(a) * sizeof(T));
  return b;
}
T_ fv<T> row(arr2d<T> a, i64 i)
{
  if (i < 0 || i >= height(a))
    abort();
  return fv<T>{ a.data + width(a) * i, width(a), 1 };
}

T_ fv<T> col(arr2d<T> a, i64 i)
{
  if (i < 0 || i >= height(a))
    abort();
  return fv<T>{ a.data + i, height(a), width(a) };
}

#define repeat(n) for (i64 _i = 0; _i < n; _i++)

#define foreach2d(array)                                                       \
  for (idx2 _i{}; _i.y != height(array); _i.y++)                               \
    for (_i.x = 0; _i.x != width(array); _i.x++)

#define map(iarr, otype, block)                                                \
  ({                                                                           \
    arr<otype> oarr;                                                           \
    foreach(iarr) {                                                            \
      auto val = (block);                                                      \
      put(oarr, val);                                                          \
    }                                                                          \
    oarr;                                                                      \
  })

#define map2d(iarr, oarr, block)                                               \
  do {                                                                         \
    req(empty(oarr));                                                          \
    vtype(iarr) *_row;                                                         \
    aforeach(iarr, _row)                                                       \
    {                                                                          \
      vtype(oarr) _orow = { 0 };                                               \
      vtype(*_row) *it;                                                        \
      aforeach(*_row, it)                                                      \
      {                                                                        \
        vtype(_orow) val = (block);                                            \
        apush(_orow, val);                                                     \
      }                                                                        \
      apush(oarr, _orow);                                                      \
    }                                                                          \
  } while (0)

#define amap2(iarr1, iarr2, oarr, block)                                       \
  do {                                                                         \
    req(empty(oarr));                                                          \
    size_t i;                                                                  \
    for (i = 0; i < MIN((iarr1).size, (iarr2).size); i++) {                    \
      vtype(iarr1) *it1 = &at((iarr1), i);                                     \
      vtype(iarr2) *it2 = &at((iarr2), i);                                     \
      vtype(oarr) val = (block);                                               \
      apush(oarr, val);                                                        \
    }                                                                          \
  } while (0)

#define amapr(iarr, oarr, offset, size, block)                                 \
  do {                                                                         \
    req(empty(oarr));                                                          \
    vtype(iarr) *it;                                                           \
    aforeachr(iarr, it, offset, size)                                          \
    {                                                                          \
      vtype(oarr) val = (block);                                               \
      apush(oarr, val);                                                        \
    }                                                                          \
  } while (0)

#define reduce(arr, block)                                                     \
  ({                                                                           \
    req(!empty(arr));                                                          \
    auto _result = first(arr);                                                 \
    foreachr(arr, 1, -1) {                                                     \
      auto a = _result;                                                        \
      auto b = *it;                                                            \
      _result = (block);                                                       \
    }                                                                          \
    _result;                                                                   \
  })

#define reducer(array, offset, size, neutral, block)                           \
  ({                                                                           \
    __typeof__(array) _arr = (array);                                          \
    req(!empty(_arr));                                                         \
    vtype(_arr) _result = neutral;                                             \
    vtype(_arr) *_it;                                                          \
    aforeachr(_arr, _it, (offset), (size))                                     \
    {                                                                          \
      vtype(_arr) a = _result;                                                 \
      vtype(_arr) b = *_it;                                                    \
      _result = (block);                                                       \
    }                                                                          \
    _result;                                                                   \
  })

T_ T min(arr<T> a_)
{
  return reduce(a_, { min(a, b); });
}

T_ T max(arr<T> a_)
{
  return reduce(a_, { max(a, b); });
}

T_ T sum(arr<T> a_)
{
  return reduce(a_, { a + b; });
}

/* clang-format off */
#define mul(array) reduce(array, { a * b; })
/* clang-format on */

#define afilter(iarr, oarr, block)                                             \
  do {                                                                         \
    vtype(iarr) *it;                                                           \
    aforeach(iarr, it)                                                         \
    {                                                                          \
      b32 _test = (block);                                                     \
      if (_test)                                                               \
        apush(oarr, *it);                                                      \
    }                                                                          \
  } while (0)

#define afold(iarr, oarr, block)                                               \
  do {                                                                         \
    size_t _idx;                                                               \
    for (_idx = 1; _idx < iarr.size; _idx++) {                                 \
      vtype(iarr) a = at(iarr, _idx - 1);                                      \
      vtype(iarr) b = at(iarr, _idx);                                          \
      vtype(iarr) r = (block);                                                 \
      apush(oarr, r);                                                          \
    }                                                                          \
  } while (0)

#define aall(arr, block)                                                       \
  ({                                                                           \
    b32 _result = 1;                                                           \
    vtype(arr) *it;                                                            \
    aforeach(arr, it)                                                          \
    {                                                                          \
      b32 _test = (block);                                                     \
      _result &= _test;                                                        \
    }                                                                          \
    _result;                                                                   \
  })

#define aresize(array, new_size)                                               \
  do {                                                                         \
    arr_adjust_capacity((new_size), vsize(array), &(array).capacity,           \
                        (void **)&(array).d);                                  \
    (array).size = (new_size);                                                 \
  } while (0)

#define aattach(arr, arr2)                                                     \
  do {                                                                         \
    size_t oldsize = arr.size;                                                 \
    aresize(arr, arr.size + arr2.size);                                        \
    memcpy(arr.d + oldsize, arr2.d, vsize(arr) * arr2.size);                   \
  } while (0)

#define acontains(array, elem)                                                 \
  ({                                                                           \
    __typeof__(elem) _elem = (elem);                                           \
    b32 has_elem = 0;                                                          \
    vtype(array) *it;                                                          \
    aforeach(array, it)                                                        \
    {                                                                          \
      if (*it == _elem) {                                                      \
        has_elem = 1;                                                          \
        break;                                                                 \
      }                                                                        \
    }                                                                          \
    has_elem;                                                                  \
  })

#define acount(array, test)                                                    \
  ({                                                                           \
    int cnt = 0;                                                               \
    vtype(array) *it;                                                          \
    aforeach(array, it)                                                        \
    {                                                                          \
      b32 b = (test);                                                          \
      if (b)                                                                   \
        cnt++;                                                                 \
    }                                                                          \
    cnt;                                                                       \
  })

struct timer {
  timespec t0, t1;

  void start() { clock_gettime(CLOCK_MONOTONIC, &t0); }
  void stop() { clock_gettime(CLOCK_MONOTONIC, &t1); }

  i64 elapsed_ms()
  {
    return (t1.tv_sec - t0.tv_sec) * 1000.0 + (t1.tv_nsec - t0.tv_nsec) / 1.0e6;
  }
};

[[noreturn]] void exit_error(const char *, ...)
    __attribute__((format(printf, 1, 2)));

#ifdef NO_DLOG
#define dlogb() ;
#define loge() ;
#define dlog(...)
#define ddlog(...)
#define dlog_ints(...)
#else
void dlogb();
void loge();
void dlog(const char *, ...) __attribute__((format(printf, 1, 2)));
void ddlog(const char *, ...) __attribute__((format(printf, 1, 2)));
void dlog_ints(char *prefix, iarr &);
#endif
void ilog(const char *, ...) __attribute__((format(printf, 1, 2)));
void initlog();

/* Read file and split by lines */
sarr frl(char *); /* file read lines */
arr<sarr> split_at_emtpy_lines(sarr);
carr2d to_arr2d(sarr lines);
arr<carr> split_at(char *str, char delim);

[[maybe_unused]] static size_t next_power_of_two(size_t n)
{
  return 1 << (sizeof(size_t) * 8 - __builtin_clzl(n));
}

/* String parsing */

[[maybe_unused]] static bool isend(char *line) { return *line == '\0'; }
[[maybe_unused]] static char peek(char *line) { return *line; }

[[maybe_unused]] static int scadv(char *&line)
{
  if (isend(line))
    return 0;
  line++;
  return 1;
}

int scint(char *&line, i64 *v); /* string try consume int */
int scintf(char *&line, i64 *v); /* string consume int or fail */
int scints(char *&line, iarr &); /* consume list of integers */
int scuntil(char *&line, char c); /* string consume until character */
int scw(char *&line); /* string consume whitespace */
int scn(char *&line, char *out, size_t n); /* string consume n chars */
int scchar(char *&line, char c); /* string try consume char */
int scstr(char *&line, char *str); /* string try consume substring */
int scstrf(char *&line, char *str); /* string consume substring or fail */

#ifdef AOC_IMPL
// #if 1

typedef enum {
  L_DDEBUG = 2,
  L_DEBUG = 1,
  L_INFO = 0,
  L_FATAL = -1,
} log_level_t;

log_level_t loglevel;

#ifndef AOC_NOMAIN
int main(int argc, char *argv[])
{
  if (argc != 2) {
    printf("Usage: %s <input_file>\n", argv[0]);
    return 1;
  }
  initlog();
  char *input_file = argv[1];
  if (!testenv("NOTASK1")) {
    ilog(">>> Task1");
    timer t;
    t.start();
    i64 r = task1(input_file);
    t.stop();
    ilog(">>> Task1 Result: %ld (%ld ms)", r, t.elapsed_ms());
  }
  yfree();
  if (!testenv("NOTASK2")) {
    ilog(">>> Task2");
    timer t;
    t.start();
    i64 r = task2(input_file);
    t.stop();
    ilog(">>> Task2 Result: %ld (%ld ms)", r, t.elapsed_ms());
  }
  yfree();
  return 0;
}
#endif

static arr_ctx<void *> H = { 0 };

void *ymalloc(size_t bytes)
{
  void *ptr = malloc(bytes);
  if (ptr == NULL)
    exit_error("Out of memory, LOL");
  yput(ptr);
  return ptr;
}

void yput(void *ptr)
{
  if (H.len + 1 > H.capacity) {
    H.capacity = next_power_of_two(H.capacity + 1);
    H.capacity = max(H.capacity, 8l);
    H.data = (void **)realloc(H.data, H.capacity * sizeof(void *));
  }
  H.data[H.len++] = ptr;
}

void *yrealloc(void *oldptr, size_t bytes)
{
  void *newptr = realloc(oldptr, bytes);
  for (auto *it = H.data; it != H.data + H.len; it++) {
    if (*it == oldptr)
      *it = newptr;
  }
  return newptr;
}

void yfree()
{
  for (auto *it = H.data; it != H.data + H.len; it++) {
    free(*it);
  }
  free(H.data);
  H = { 0 };
}

void arr_adjust_capacity(i64 new_size, i64 vsize, i64 *capacity, void **data)
{
  if (new_size <= *capacity)
    return;
  new_size = next_power_of_two(new_size);
  *capacity = max(new_size, 8l);
  *data = yrealloc(*data, *capacity * vsize);
}

bool testenv(char *name)
{
  char *env = getenv(name);
  if (!env)
    return 0;
  return strcmp(env, "0") != 0 && strcmp(env, "false") != 0;
}

int envint(char *name)
{
  char *env = getenv(name);
  if (!env)
    return 0;
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
  if (loglevel < L_DEBUG) {
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
  if (loglevel < L_DDEBUG) {
    return;
  }
  fputs("DD ", stdout);
  va_list args;
  va_start(args, fmt);
  vfprintf(stdout, fmt, args);
  va_end(args);
  fputc('\n', stdout);
}

void dlog_ints(char *prefix, iarr a)
{
  if (loglevel < L_DEBUG)
    return;
  fputs("D ", stdout);
  fputs(prefix, stdout);
  foreach(a) {
    fprintf(stdout, "%ld ", *it);
  }
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

void initlog()
{
  int level = envint("DEBUG");
  if (level >= 2)
    loglevel = L_DDEBUG;
  else
    loglevel = max<log_level_t>(log_level_t(level), L_FATAL);
}

sarr frl(char *fname)
{
  sarr lines;

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
    put(lines, line);
    line = NULL;
    linecap = 0;
  }

  fclose(f);
  return lines;
}

arr<sarr> split_at_emtpy_lines(sarr lines)
{
  arr<sarr> section;
  put(section, sarr{});

  foreach(lines) {
    if (isend(*it)) {
      put(section, sarr{});
      continue;
    }
    put(last(section), *it);
  }

  return section;
}

carr2d to_arr2d(sarr lines)
{
  i64 w = strlen(first(lines));
  i64 h = len(lines);

  i64 o = 0;
  carr2d out(w, h);
  foreach(lines) {
    req(strlen(*it) == size_t(w));
    memcpy(out.data + o, *it, w);
    o += w;
  }

  return out;
}

arr<carr> split_at(char *str, char delim)
{
  arr<carr> o;
  put(o, {});
  for (; !isend(str); str++) {
    if (peek(str) == delim) {
      put(o, {});
      continue;
    }
    put(last(o), peek(str));
  }
  return o;
}

int scint(char *&line, i64 *v)
{
  int advanced = 0;

  i64 sign = 1;
  if (peek(line) == '-') {
    sign = -1;
    advanced += scadv(line);
  }

  *v = 0;
  for (; *line != '\0' && isdigit(*line); line++) {
    i64 x = *v * 10 + (*line - '0');
    if (x < *v)
      exit_error("Overflow parsing %ld: %ld", *v, x);
    *v = x;
    advanced++;
  }

  *v *= sign;

  return advanced;
}

int scintf(char *&line, i64 *v)
{
  int consumed = scint(line, v);
  if (consumed == 0)
    exit_error("Failed to parse int: '%s'", line);
  return consumed;
}

int scints(char *&line, iarr &out)
{
  int consumed = 0;
  int consumed_int = 0;
  do {
    consumed += scw(line);
    i64 val;
    consumed_int = scint(line, &val);
    consumed += consumed_int;
    if (consumed_int > 0) {
      put(out, val);
    }
  } while (consumed_int > 0);
  return consumed;
}

int scuntil(char *&line, char c)
{
  int consumed = 0;
  while (peek(line) != c && !isend(line)) {
    consumed += scadv(line);
  }
  consumed += scadv(line); /* Read past c */
  return consumed;
}

int scw(char *&line)
{
  int consumed = 0;
  while (isspace(peek(line)) && !isend(line)) {
    consumed += scadv(line);
  }
  return consumed;
}

int scn(char *&line, char *out, size_t n)
{
  int consumed = 0;

  size_t i = 0;
  for (i = 0; !isend(line) && i < n; i++) {
    out[i] = peek(line);
    consumed += scadv(line);
  }
  out[n] = '\0';
  return consumed;
}

int scchar(char *&line, char c)
{
  if (peek(line) == c)
    return scadv(line);
  return 0;
}

int scstr(char *&line, char *str)
{
  size_t len_prefix = strlen(str);
  if (strncmp(line, str, len_prefix) != 0)
    return 0;
  *line += len_prefix;
  return len_prefix;
}

int scstrf(char *&line, char *str)
{
  int consumed = scstr(line, str);
  if (consumed == 0)
    exit_error("Failed to read prefix '%s': '%s'", str, line);
  return consumed;
}

#endif /* defined AOC_IMPL */
#endif /* defined AOC_H */
