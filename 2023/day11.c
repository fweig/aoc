#define AOC_IMPL
#include "aoc11.h"

typedef enum {
	T_EMPTY,
	T_GALAXY,
} Tile;
ARRAY(tile, Tile);
ARRAY(tile2d, arr_tile);

ARRAY(idx2, idx2);

arr_int expansion_factor_row(arr_tile2d tiles, i64 factor)
{
	arr_int exp = { 0 };

	dlog("ROW");

	size_t i;
	for (i = 0; i < height(tiles); i++) {
		b32 hasgalaxy = 0;
		Tile *t;
		aforeach(at(tiles, i), t) {
			hasgalaxy |= (*t == T_GALAXY);
		}

		apush(exp, hasgalaxy ? 1 : factor);
		dlog("%lld", last(exp));
	}

	return exp;
}

arr_int expansion_factor_col(arr_tile2d tiles, i64 factor)
{
	arr_int exp = { 0 };

	dlog("COL");

	size_t i;
	for (i = 0; i < width(tiles); i++) {
		b32 hasgalaxy = 0;
		size_t j;
		for (j = 0; j < height(tiles); j++) {
			idx2 pos = { i, j };
			hasgalaxy |= (at2(tiles, pos) == T_GALAXY);
		}

		apush(exp, hasgalaxy ? 1 : factor);
		dlog("%lld", last(exp));
	}

	return exp;
}

arr_idx2 find_galaxies(arr_tile2d tiles)
{
	arr_idx2 galaxies = { 0 };

	idx2 p;
	aforeach2d_idx(tiles, p) {
		Tile t = at2(tiles, p);
		if (t == T_GALAXY)
			apush(galaxies, p);
	}

	return galaxies;
}

arr_int path_lens(arr_tile2d tiles, arr_int exp_row, arr_int exp_col)
{
	arr_int dist = { 0 };

	arr_idx2 galaxies = find_galaxies(tiles);

	size_t i, j;
	for (i = 0; i < galaxies.size; i++) {
		idx2 p1 = at(galaxies, i);
		for (j = i + 1; j < galaxies.size; j++) {
			idx2 p2 = at(galaxies, j);
			i64 offset_h = MIN(p1.x, p2.x);
			i64 size_h = llabs(p1.x - p2.x);
			i64 dist_h = areducer(exp_col, (offset_h), (size_h), 0, {
				ddlog("a = %lld, b = %lld", a, b);
				a + b;
			});
			i64 offset_v = MIN(p1.y, p2.y);
			i64 size_v = llabs(p1.y - p2.y);
			i64 dist_v = asumr(exp_row, offset_v, size_v);

			dlog("%zu(%lld, %lld) @ (%lld, %lld), %zu(%lld, %lld) @ (%lld, %lld):  %lld + %lld = %lld",
			     i, p1.x, p1.y, offset_h, size_h, j, p2.x, p2.y,
			     offset_v, size_v, dist_h, dist_v, dist_h + dist_v);

			apush(dist, dist_h + dist_v);
		}
	}

	return dist;
}

void task1(char *infile)
{
	arr_str input = frl(infile);
	arr_tile2d tiles = { 0 };
	amap_str(input, tiles, { (*it == '.') ? T_EMPTY : T_GALAXY; });

	arr_int exp_row = expansion_factor_row(tiles, 1);
	arr_int exp_col = expansion_factor_col(tiles, 1);

	arr_int dists = path_lens(tiles, exp_row, exp_col);

	i64 r = asum(dists);
	ilog("Task 1 result: %lld", r);
}

void task2(char *infile)
{
	arr_str input = frl(infile);
	arr_tile2d tiles = { 0 };
	amap_str(input, tiles, { (*it == '.') ? T_EMPTY : T_GALAXY; });

	arr_int exp_row = expansion_factor_row(tiles, 1000000);
	arr_int exp_col = expansion_factor_col(tiles, 1000000);

	arr_int dists = path_lens(tiles, exp_row, exp_col);

	i64 r = asum(dists);
	ilog("Task 2 result: %lld", r);
}
