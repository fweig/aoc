#define AOC_IMPL
#define AOC_NOMAIN
#include "aoc.h"

void test_arr()
{
  iarr a;
  req(a.size() == 0);
  req(a.capacity() == 0);
  req(a.data() == 0);

  put(a, 7l);
  req(a.size() == 1);
  req(a.capacity() == 8);
  req(a[0] == 7);
}

void test_map()
{
  iarr a;
  put(a, 7l);
  put(a, 8l);
  put(a, 9l);

  auto b = map(a, i64, { (*it) * (*it); });
  req(b.size() == 3);
  req(b[0] == 49);
  req(b[1] == 64);
  req(b[2] == 81);
}

int main(int /*argc*/, char * /*argv*/[])
{
  initlog();
  test_arr();
  test_map();
  ilog("OK");
  return 0;
}
