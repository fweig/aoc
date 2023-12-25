#define AOC_IMPL
#include "aoc.h"

static constexpr idx2 Up{ 0, -1 };
static constexpr idx2 Down{ 0, 1 };
static constexpr idx2 Left{ -1, 0 };
static constexpr idx2 Right{ 1, 0 };

struct Beam {
  idx2 pos;
  idx2 dir;
};

Beam step(Beam b, idx2 ndir) { return Beam{ b.pos + ndir, ndir }; }
Beam step(Beam b) { return step(b, b.dir); }

void trace_beam(Beam b, carr2d grid, arr2d<idx2> nrg)
{
  if (!inside(b.pos, grid.dim))
    return;
  if (nrg(b.pos) == b.dir)
    return;

  nrg(b.pos) = b.dir;

  switch (grid(b.pos)) {
  case '.':
    trace_beam(step(b), grid, nrg);
    break;
  case '|':
    if (b.dir.x == 0) // dir == Up or Down
      trace_beam(step(b), grid, nrg);
    else {
      trace_beam(step(b, Up), grid, nrg);
      trace_beam(step(b, Down), grid, nrg);
    }
    break;
  case '-':
    if (b.dir.y == 0) // dir == Left or right
      trace_beam(step(b), grid, nrg);
    else {
      trace_beam(step(b, Left), grid, nrg);
      trace_beam(step(b, Right), grid, nrg);
    }
    break;
  case '/': {
    idx2 ndir;
    if (b.dir == Up)
      ndir = Right;
    else if (b.dir == Down)
      ndir = Left;
    else if (b.dir == Right)
      ndir = Up;
    else if (b.dir == Left)
      ndir = Down;
    else
      exit_error("Unknown direction!");
    trace_beam(step(b, ndir), grid, nrg);
    break;
  }
  case '\\': {
    idx2 ndir;
    if (b.dir == Up)
      ndir = Left;
    else if (b.dir == Down)
      ndir = Right;
    else if (b.dir == Right)
      ndir = Down;
    else if (b.dir == Left)
      ndir = Up;
    else
      exit_error("Unknown direction!");
    trace_beam(step(b, ndir), grid, nrg);
    break;
  }
  default:
    exit_error("Unknown symbol '%c'", grid(b.pos));
  }
}

i64 count_energized(arr2d<idx2> nrg)
{
  i64 c = 0;
  for (i64 i = 0; i < len(nrg); i++) {
    c += (nrg.data[i] != idx2{});
  }
  return c;
}

i64 task1(char *infile)
{
  auto grid = to_arr2d(frl(infile));
  arr2d<idx2> energized(grid.dim);
  fill(energized, idx2{});
  trace_beam(Beam{ idx2{ 0, 0 }, Right }, grid, energized);
  return count_energized(energized);
}

i64 task2(char *infile)
{
  auto grid = to_arr2d(frl(infile));
  arr2d<idx2> energized(grid.dim);
  iarr ne;

  // Top -> Down
  for (idx2 i; i.x < width(grid); i.x++) {
    fill(energized, idx2{});
    trace_beam(Beam{ i, Down }, grid, energized);
    put(ne, count_energized(energized));
  }

  // Down -> Top
  for (idx2 i{ 0, height(grid) - 1 }; i.x < width(grid); i.x++) {
    fill(energized, idx2{});
    trace_beam(Beam{ i, Up }, grid, energized);
    put(ne, count_energized(energized));
  }

  // Left -> Right
  for (idx2 i{ 0, 0 }; i.y < height(grid); i.y++) {
    fill(energized, idx2{});
    trace_beam(Beam{ i, Right }, grid, energized);
    put(ne, count_energized(energized));
  }

  // Right -> Left
  for (idx2 i{ width(grid) - 1, 0 }; i.y < height(grid); i.y++) {
    fill(energized, idx2{});
    trace_beam(Beam{ i, Left }, grid, energized);
    put(ne, count_energized(energized));
  }

  return max(ne);
}
