#define AOC_MAIN
#include "aoc.h"

typedef struct {
	i64 src_st;
	i64 dest_st;
	i64 len;
} Seedmap;
ARRAY(seedmap, Seedmap);
ARRAY(seedmap2d, array_seedmap);

i64 find_mapping(i64 pos, array_seedmap map)
{
	i64 dest = pos;
	Seedmap *r;
	AFOR_EACH(map, r) {
		if (pos >= r->src_st && pos < r->src_st + r->len)
			dest = pos - r->src_st + r->dest_st;
	}
	dlog("Mapping %lld -> %lld", pos, dest);
	return dest;
}

i64 find_location(i64 pos, array_seedmap2d maps)
{
	dlog("Find location of %lld", pos);
	array_seedmap *map;
	AFOR_EACH(maps, map) {
		pos = find_mapping(pos, *map);
	}
	return pos;
}

array_int parse_seeds(array_str header)
{
	REQUIRE(header.size == 1);
	char *line = FIRST(header);

	array_int seeds = { 0 };

	dlog("Parsing seeds");
	scstrf(&line, "seeds:");
	scw(&line);

	while (!isend(&line)) {
		scw(&line);

		i64 seed;
		scintf(&line, &seed);
		dlog("Seed %lld", seed);
		APUSH(seeds, seed);
	}

	return seeds;
}

Seedmap parse_range(char *input)
{
	Seedmap smap = { 0 };
	scintf(&input, &smap.dest_st);
	scw(&input);
	scintf(&input, &smap.src_st);
	scw(&input);
	scintf(&input, &smap.len);
	dlog("Read mapping { .dest_st = %lld, .src_str = %lld, .len = %lld}",
	     smap.dest_st, smap.src_st, smap.len);
	REQ(isend(&input));
	return smap;
}

array_seedmap parse_seedmap(array_str input)
{
	dlog("Read mapping");
	array_seedmap smap = { 0 };
	AMAPR(input, smap, parse_range, 1, -1);

	return smap;
}

void task1(char *infile)
{
	array_str input = file_read_lines(infile);
	array_str2d sections = split_at_emtpy_lines(input);

	array_int seeds = parse_seeds(FIRST(sections));

	array_seedmap2d map = { 0 };
	AMAPR(sections, map, parse_seedmap, 1, -1);

	array_int locs = { 0 };
	AMAP1(seeds, locs, find_location, map);

	int minloc = aminint(locs);
	ilog("Lowest location: %d", minloc);
}

typedef struct {
	i64 start;
	i64 len;
} SeedRange;
ARRAY(seedrange, SeedRange);

array_seedrange parse_seed_ranges(array_str header)
{
	REQUIRE(header.size == 1);
	char *line = FIRST(header);

	array_seedrange seeds = { 0 };

	dlog("Parsing seeds");
	scstrf(&line, "seeds:");
	scw(&line);

	while (!isend(&line)) {
		scw(&line);

		SeedRange range;
		scintf(&line, &range.start);
		scw(&line);
		scintf(&line, &range.len);
		dlog("Seed range (%lld, %lld)", range.start,
		     range.start + range.len);
		APUSH(seeds, range);
	}

	return seeds;
}

array_seedrange find_overlap(SeedRange range, array_seedmap map)
{
	array_seedrange overlap = { 0 };

	Seedmap *mapping;
	AFOR_EACH(map, mapping) {
		b32 has_overlap = range.start >= mapping->src_st || range.start + range.len <= mapping->src_st + mapping->len;
		SeedRange overlap = {
			.start = MAX(range.start, mapping->src_st),
			.len = MIN(range.start + range.len, mapping->)
		};
	}
}

i64 minloc_range(SeedRange range, array_seedmap2d maps)
{
	i64 loc = __INT64_MAX__;
	i64 i;
	#pragma omp parallel for schedule(static) reduction(min: loc)
	for (i = range.start; i < range.start + range.len; i++) {
		loc = MIN(loc, find_location(i, maps));
	}
	ilog("Min location for range (%lld, %lld): %lld", range.start, range.len, loc);
	return loc;
}

void task2(char *infile)
{
	array_str input = file_read_lines(infile);
	array_str2d sections = split_at_emtpy_lines(input);

	array_seedrange seeds = parse_seed_ranges(FIRST(sections));

	array_seedmap2d map = { 0 };
	AMAPR(sections, map, parse_seedmap, 1, -1);

	array_int locs = { 0 };
	AMAP1(seeds, locs, minloc_range, map);

	int minloc = aminint(locs);
	ilog("Lowest location: %d", minloc);
}
