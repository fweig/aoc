#define AOC_IMPL
#include "aoc.h"

bool col_eq(carr2d a, i64 x, i64 y)
{
  for (i64 i = 0; i < height(a); i++) {
    if (a(x, i) != a(y, i))
      return false;
  }
  return true;
}

bool symmetric_col(carr2d a, i64 c)
{
  bool b = true;

  i64 x = c, y = c + 1;
  while (x >= 0 && y < width(a)) {
    b &= col_eq(a, x, y);
    x--;
    y++;
  }

  return b;
}

bool row_eq(carr2d a, i64 x, i64 y)
{
  for (i64 i = 0; i < width(a); i++) {
    if (a(i, x) != a(i, y))
      return false;
  }
  return true;
}

bool symmetric_row(carr2d a, i64 r)
{
  bool b = true;

  i64 x = r, y = r + 1;
  while (x >= 0 && y < height(a)) {
    b &= row_eq(a, x, y);
    x--;
    y++;
  }

  return b;
}

i64 summarize(carr2d a)
{
  i64 s = 0;

  for (i64 r = 0; r < height(a) - 1; r++) {
    if (symmetric_row(a, r)) {
      dlog("Symmetric h line between %ld, %ld", r, r + 1);
      s += 100 * (r + 1);
    }
  }

  for (i64 c = 0; c < width(a) - 1; c++) {
    if (symmetric_col(a, c)) {
      dlog("Symmetric v line between %ld, %ld", c, c + 1);
      s += c + 1;
    }
  }

  return s;
}

i64 task1(char *infile)
{
  auto input0 = frl(infile);
  auto input1 = split_at_emtpy_lines(input0);
  auto i = map(input1, carr2d, { to_arr2d(*it); });
  auto v = map(i, i64, { summarize(*it); });
  return sum(v);
}

i64 col_eq2(carr2d a, i64 x, i64 y)
{
  i64 r = 0;
  for (i64 i = 0; i < height(a); i++) {
    r += (a(x, i) != a(y, i));
  }
  return r;
}

bool symmetric_col2(carr2d a, i64 c)
{
  i64 b = 0;

  i64 x = c, y = c + 1;
  while (x >= 0 && y < width(a)) {
    b += col_eq2(a, x, y);
    x--;
    y++;
  }

  return b == 1;
}

i64 row_eq2(carr2d a, i64 x, i64 y)
{
  i64 r = 0;
  for (i64 i = 0; i < width(a); i++) {
    r += (a(i, x) != a(i, y));
  }
  return r;
}

bool symmetric_row2(carr2d a, i64 r)
{
  i64 b = 0;

  i64 x = r, y = r + 1;
  while (x >= 0 && y < height(a)) {
    b += row_eq2(a, x, y);
    x--;
    y++;
  }

  return b == 1;
}

i64 summarize2(carr2d a)
{
  i64 s = 0;

  for (i64 r = 0; r < height(a) - 1; r++) {
    if (symmetric_row2(a, r)) {
      dlog("Symmetric h line between %ld, %ld", r, r + 1);
      s += 100 * (r + 1);
    }
  }

  for (i64 c = 0; c < width(a) - 1; c++) {
    if (symmetric_col2(a, c)) {
      dlog("Symmetric v line between %ld, %ld", c, c + 1);
      s += c + 1;
    }
  }

  return s;
}

i64 task2(char *infile)
{
  auto input0 = frl(infile);
  auto input1 = split_at_emtpy_lines(input0);
  auto i = map(input1, carr2d, { to_arr2d(*it); });
  auto v = map(i, i64, { summarize2(*it); });
  return sum(v);
}
