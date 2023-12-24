#define AOC_IMPL
#include "aoc.h"

void dump(carr2d a)
{
  foreach2d(a) {
    putc(a(_i), stdout);
    if (_i.x == width(a) - 1)
      putc('\n', stdout);
  }
  putc('\n', stdout);
}

void roll_left(cfv c)
{
  i64 j = c[0] != '.';
  for (i64 i = 1; i < len(c); i++) {
    if (c[i] == '#')
      j = i + 1;
    continue_if(c[i] != 'O');
    c[i] = '.';
    c[j] = 'O';
    j++;
  }
}

i64 total_load(carr2d a)
{
  i64 s = 0;
  foreach2d(a) {
    continue_if(a(_i) != 'O');
    s += height(a) - _i.y;
  }
  return s;
}

i64 task1(char *infile)
{
  auto input = to_arr2d(frl(infile));
  dump(input);
  for (i64 i = 0; i < width(input); i++) {
    roll_left(col(input, i));
  }
  putc('\n', stdout);
  dump(input);
  return total_load(input);
}

void roll_right(cfv c)
{
  i64 j = len(c) - 1 - (last(c) != '.');
  for (i64 i = len(c) - 2; i >= 0; i--) {
    if (c[i] == '#')
      j = i - 1;
    continue_if(c[i] != 'O');
    c[i] = '.';
    c[j] = 'O';
    j--;
  }
}

i64 task2(char *infile)
{
  auto input = to_arr2d(frl(infile));

  iarr hash;
  arr<carr2d> state;

  i64 cycle = -1;

  while (1) {
    for (i64 i = 0; i < width(input); i++) {
      roll_left(col(input, i));
    }

    for (i64 i = 0; i < height(input); i++) {
      roll_left(row(input, i));
    }

    for (i64 i = 0; i < width(input); i++) {
      roll_right(col(input, i));
    }

    for (i64 i = 0; i < height(input); i++) {
      roll_right(row(input, i));
    }

    auto h = total_load(input);
    auto st = indexof(hash, h);

    if (st != -1) {
      if (indexof(state, input) == st) {
        cycle = st;
        break;
      }
    }

    put(hash, h);
    put(state, copy(input));
  }

  dlog("cycle from %ld to %ld", cycle, len(hash));

  i64 cycle_len = len(hash) - cycle;

  i64 i = (1000000000 - cycle) % cycle_len;
  return hash[cycle + i - 1]; // Why -1???
}
