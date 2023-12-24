#define AOC_IMPL
#include "aoc.h"

struct Lens {
  carr label;
  i64 focal_length;

  bool operator==(Lens o) { return label == o.label; }
};

void print(carr a) {
  foreach(a) {
    putc(*it, stdout);
  }
}

void print(Lens l)
{
  putc('[', stdout);
  print(l.label);
  putc(' ', stdout);
  putc(l.focal_length + '0', stdout);
  putc(']', stdout);
}

i64 hash(carr a)
{
  i64 h = 0;
  foreach(a) {
    h += *it;
    h *= 17;
    h %= 256;
  }
  dlog("Hash is %ld", h);
  return h;
}

i64 task1(char *infile)
{
  sarr x = frl(infile);
  req(len(x) == 1);
  auto input = split_at(first(x), ',');
  auto hs = map(input, i64, hash(*it));
  return sum(hs);
}

void print_boxes(arr<arr<Lens>> boxes)
{
  foreach(boxes) {
    continue_if(empty(*it));
    printf("Box %ld: ", index(boxes, it));
    foreach_(*it, l) { print(*l); }
    putc('\n', stdout);
  }
}

carr label(carr op)
{
  carr o;
  foreach(op) {
    if (*it == '-' || *it == '=')
      break;
    put(o, *it);
  }
  return o;
}

void do_action(carr op, arr<arr<Lens>> boxes)
{
  print(op);
  printf(":\n");

  auto l = label(op);

  auto b = hash(l);
  auto box = boxes[b];

  char c = op[len(l)];
  if (c == '-') {
    remove(box, Lens{ l, 0 });
    // print_boxes(boxes);
    return;
  }
  req(c == '=');
  i64 v = last(op) - '0';

  Lens lens{ l, v };

  i64 i = indexof(box, lens);

  if (i == -1)
    put(box, lens);
  else
    box[i] = lens;

  // print_boxes(boxes);
}

i64 focusing_power(i64 i, arr<Lens> a)
{
  i64 p = 0;
  foreach(a) {
    p += (i + 1) * (index(a, it) + 1) * it->focal_length;
  }
  return p;
}

i64 task2(char *infile)
{
  sarr x = frl(infile);
  req(len(x) == 1);
  auto input = split_at(first(x), ',');

  arr<arr<Lens>> boxes;
  resize_init(boxes, 256);

  foreach(input) {
    do_action(*it, boxes);
  }

  iarr p =  map(boxes, i64, focusing_power(index(boxes, it), *it));

  return sum(p);
}
