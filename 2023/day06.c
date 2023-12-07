#define AOC_IMPL
#include "aoc06.h"

arr_int parse_line(char *line)
{
	arr_int out = { 0 };
	scuntil(&line, ':');
	scints(&line, &out);
	return out;
}

i64 count_ways_to_beat_record(i64 time, i64 dist)
{
	i64 n = 0;
	i64 x;
	for (x = 1; x < time; x++) {
		n += (time - x) * x > dist;
	}
	dlog("time = %lld, dist = %lld, n = %lld", time, dist, n);
	return n;
}

void task1(char *infile)
{
	arr_str input = file_read_lines(infile);
	req(input.size == 2);
	arr_int times = parse_line(first(input));
	arr_int dists = parse_line(last(input));
	req(times.size == dists.size);

	arr_int num_beat_rec = { 0 };
	amap2(times, dists, num_beat_rec, count_ways_to_beat_record);

	i64 result = amul(num_beat_rec);
	ilog("Result t1: %lld", result);
}

void task2(char *infile)
{
	ilog("Reading file '%s'", infile);
}
