#define AOC_IMPL
#include "aoc10.h"

typedef enum {
	T_EMPTY = 0,
	T_INSIDE,
	T_OUTSIDE,
	T_START,
	T_NS,
	T_EW,
	T_NE,
	T_NW,
	T_SW,
	T_SE,
} Tile;
ARRAY(tile, Tile);
ARRAY(tile2d, arr_tile);

#define SOUTH ((idx2){ .x = 0, .y = 1 })
#define NORTH ((idx2){ .x = 0, .y = -1 })
#define WEST ((idx2){ .x = -1, .y = 0 })
#define EAST ((idx2){ .x = 1, .y = 0 })

#define WALK(arr2d, delta, pos)                                                \
	for (; inside(arr2d, pos); pos = idx2add(pos, delta))

char *dir2str(idx2 dir)
{
	if (idx2eq(dir, SOUTH))
		return "SOUTH";
	else if (idx2eq(dir, NORTH))
		return "NORTH";
	else if (idx2eq(dir, WEST))
		return "WEST";
	else if (idx2eq(dir, EAST))
		return "EAST";
	return "UNKNOWN";
}

char tile2char(Tile t)
{
	char c[] = ".IOS|-LJ7F";
	return c[t];
}

void dumpTiles(arr_tile2d tiles, idx2 pos)
{
	idx2 i;
	for (i.y = 0; (size_t)i.y < height(tiles); i.y++) {
		for (i.x = 0; (size_t)i.x < width(tiles); i.x++) {
			fputc(tile2char(at2(tiles, i)), stdout);
			if (idx2eq(i, pos))
				fputc('X', stdout);
			else
				fputc(' ', stdout);
		}
		fputc('\n', stdout);
	}
}

Tile char2tile(char c)
{
	switch (c) {
	case '.':
		return T_EMPTY;
	case 'S':
		return T_START;
	case '|':
		return T_NS;
	case '-':
		return T_EW;
	case 'L':
		return T_NE;
	case 'J':
		return T_NW;
	case '7':
		return T_SW;
	case 'F':
		return T_SE;
	default:
		exit_error("Unknown tile '%c'", c);
	}
	return T_EMPTY; /* Unreachable */
}

b32 connects_to(Tile t, idx2 dir)
{
	switch (t) {
	case T_EMPTY:
	case T_START:
	case T_INSIDE:
	case T_OUTSIDE:
		return 0;
	case T_NS:
		return idx2eq(dir, SOUTH) || idx2eq(dir, NORTH);
	case T_EW:
		return idx2eq(dir, EAST) || idx2eq(dir, WEST);
	case T_NE:
		return idx2eq(dir, NORTH) || idx2eq(dir, EAST);
	case T_NW:
		return idx2eq(dir, NORTH) || idx2eq(dir, WEST);
	case T_SW:
		return idx2eq(dir, SOUTH) || idx2eq(dir, WEST);
	case T_SE:
		return idx2eq(dir, SOUTH) || idx2eq(dir, EAST);
	}
	return 0;
}

idx2 tile_to(Tile t, idx2 dir)
{
	switch (t) {
	case T_EMPTY:
	case T_INSIDE:
	case T_OUTSIDE:
	case T_START:
		exit_error(
			"Error: Can't find connection on empty or start tile");
	case T_NS:
		if (idx2eq(dir, SOUTH))
			return SOUTH;
		else if (idx2eq(dir, NORTH))
			return NORTH;
		else
			req(0);
		break;
	case T_EW:
		if (idx2eq(dir, EAST))
			return EAST;
		else if (idx2eq(dir, WEST))
			return WEST;
		else
			req(0);
		break;
	case T_NE:
		if (idx2eq(dir, SOUTH))
			return EAST;
		else if (idx2eq(dir, WEST))
			return NORTH;
		else
			req(0);
		break;
	case T_NW:
		if (idx2eq(dir, SOUTH))
			return WEST;
		else if (idx2eq(dir, EAST))
			return NORTH;
		else
			req(0);
		break;
	case T_SW:
		if (idx2eq(dir, NORTH))
			return WEST;
		else if (idx2eq(dir, EAST))
			return SOUTH;
		else
			req(0);
		break;
	case T_SE:
		if (idx2eq(dir, NORTH))
			return EAST;
		else if (idx2eq(dir, WEST))
			return SOUTH;
		else
			req(0);
		break;
	}
	exit_error("Error: Unknown tile %d", t);
}

idx2 find_start(arr_tile2d tiles)
{
	idx2 i;
	for (i.y = 0; (size_t)i.y < height(tiles); i.y++) {
		for (i.x = 0; (size_t)i.x < width(tiles); i.x++) {
			if (at2(tiles, i) == T_START)
				return i;
		}
	}
	return (idx2){ 0, 0 };
}

size_t len_path(arr_tile2d tiles)
{
	size_t len = 0;

	idx2 pos = find_start(tiles);

	idx2 northp = idx2add(pos, NORTH);
	idx2 westp = idx2add(pos, WEST);
	idx2 southp = idx2add(pos, SOUTH);
	idx2 eastp = idx2add(pos, EAST);

	idx2 dir;

	if (connects_to(at2(tiles, northp), SOUTH))
		dir = NORTH;
	else if (connects_to(at2(tiles, westp), EAST))
		dir = WEST;
	else if (connects_to(at2(tiles, southp), NORTH))
		dir = SOUTH;
	else if (connects_to(at2(tiles, eastp), WEST))
		dir = EAST;
	else
		exit_error("No connection found");

	while (1) {
		dlog("Go %s, pos = (%lld, %lld)", dir2str(dir), pos.x, pos.y);
		dlog("dir = (%lld, %lld)", dir.x, dir.y);
		/* dumpTiles(tiles, pos); */
		len++;
		pos = idx2add(pos, dir);

		Tile t = at2(tiles, pos);
		dlog("new tile = %d", t);
		if (t == T_START)
			break;

		dir = tile_to(t, dir);
	}

	return len;
}

void task1(char *infile)
{
	arr_str input = frl(infile);

	arr_tile2d tiles = { 0 };
	amap(input, tiles, {
		arr_tile row = { 0 };
		while (**it != '\0') {
			apush(row, char2tile(**it));
			scadv(it);
		}
		row;
	});
	dlog("width = %zu, height = %zu", width(tiles), height(tiles));

	idx2 st = find_start(tiles);
	dlog("Start @ %lld, %lld", st.x, st.y);

	size_t len = len_path(tiles);
	ilog("Result: %zu (%zu)", len / 2, len);
}

Tile start2tile(arr_tile2d tiles, idx2 pos)
{
	idx2 northp = idx2add(pos, NORTH);
	idx2 westp = idx2add(pos, WEST);
	idx2 southp = idx2add(pos, SOUTH);
	idx2 eastp = idx2add(pos, EAST);

	Tile t;

	if (connects_to(at2(tiles, northp), SOUTH) &&
	    connects_to(at2(tiles, southp), NORTH))
		t = T_NS;
	else if (connects_to(at2(tiles, westp), EAST) &&
		 connects_to(at2(tiles, eastp), WEST))
		t = T_EW;
	else if (connects_to(at2(tiles, northp), SOUTH) &&
		 connects_to(at2(tiles, eastp), WEST))
		t = T_NE;
	else if (connects_to(at2(tiles, northp), SOUTH) &&
		 connects_to(at2(tiles, westp), EAST))
		t = T_NW;
	else if (connects_to(at2(tiles, southp), NORTH) &&
		 connects_to(at2(tiles, westp), EAST))
		t = T_SW;
	else if (connects_to(at2(tiles, southp), NORTH) &&
		 connects_to(at2(tiles, eastp), WEST))
		t = T_SE;
	else
		exit_error("This shouldn't happen");

	return t;
}

arr_tile2d cutout_loop(arr_tile2d tiles)
{
	size_t w = width(tiles), h = height(tiles);

	arr_tile2d loop = { 0 };
	size_t i;
	for (i = 0; i < h; i++) {
		apush(loop, (arr_tile){ 0 });
		aresize(last(loop), w);
		memset(last(loop).d, 0, sizeof(Tile) * w);
	}

	idx2 pos = find_start(tiles);

	idx2 northp = idx2add(pos, NORTH);
	idx2 westp = idx2add(pos, WEST);
	idx2 southp = idx2add(pos, SOUTH);
	idx2 eastp = idx2add(pos, EAST);

	idx2 dir;

	if (connects_to(at2(tiles, northp), SOUTH))
		dir = NORTH;
	else if (connects_to(at2(tiles, westp), EAST))
		dir = WEST;
	else if (connects_to(at2(tiles, southp), NORTH))
		dir = SOUTH;
	else if (connects_to(at2(tiles, eastp), WEST))
		dir = EAST;
	else
		exit_error("No connection found");

	while (1) {
		dlog("Go %s, pos = (%lld, %lld)", dir2str(dir), pos.x, pos.y);
		dlog("dir = (%lld, %lld)", dir.x, dir.y);

		at2(loop, pos) = at2(tiles, pos);

		/* dumpTiles(tiles, pos); */
		pos = idx2add(pos, dir);

		Tile t = at2(tiles, pos);
		dlog("new tile = %d", t);
		if (t == T_START)
			break;

		dir = tile_to(t, dir);
	}

	return loop;
}

void mark_obviously_outside(arr_tile2d loop, idx2 st)
{
	if (!inside(loop, st))
		return;

	Tile t = at2(loop, st);
	if (t != T_EMPTY)
		return;

	b32 is_outside = 0;

	idx2 pos = st;
	WALK(loop, NORTH, pos) {
		Tile t = at2(loop, pos);
		if (t == T_OUTSIDE)
			is_outside = 1;
		if (t != T_EMPTY)
			break;
	}
	if (!inside(loop, pos))
		is_outside = 1;

	pos = st;
	WALK(loop, WEST, pos) {
		Tile t = at2(loop, pos);
		if (t == T_OUTSIDE)
			is_outside = 1;
		if (t != T_EMPTY)
			break;
	}
	if (!inside(loop, pos))
		is_outside = 1;

	pos = st;
	WALK(loop, SOUTH, pos) {
		Tile t = at2(loop, pos);
		if (t == T_OUTSIDE)
			is_outside = 1;
		if (t != T_EMPTY)
			break;
	}
	if (!inside(loop, pos))
		is_outside = 1;

	pos = st;
	WALK(loop, EAST, pos) {
		Tile t = at2(loop, pos);
		if (t == T_OUTSIDE)
			is_outside = 1;
		if (t != T_EMPTY)
			break;
	}
	if (!inside(loop, pos))
		is_outside = 1;

	if (is_outside)
		at2(loop, st) = T_OUTSIDE;
}

void mark_all_outside(arr_tile2d loop)
{
	size_t w = width(loop), h = height(loop);

	size_t i, j;
	for (i = 0; i < h; i++) {
		for (j = 0; j < w; j++) {
			mark_obviously_outside(loop, (idx2){ j, i });
		}
	}
}

idx2 rotate90deg_clockwise(idx2 i)
{
	return (idx2){ i.y, -i.x };
}

idx2 rotate90deg_counterclockwise(idx2 i)
{
	return (idx2){ -i.y, i.x };
}

idx2 tile_to2(Tile t, idx2 dir, idx2 *normal)
{
	switch (t) {
	case T_EMPTY:
	case T_INSIDE:
	case T_OUTSIDE:
	case T_START:
		exit_error(
			"Error: Can't find connection on empty or start tile");
	case T_NS:
		/* Don't change normal */
		if (idx2eq(dir, SOUTH))
			return SOUTH;
		else if (idx2eq(dir, NORTH))
			return NORTH;
		else
			req(0);
		break;
	case T_EW:
		/* Don't change normal */
		if (idx2eq(dir, EAST))
			return EAST;
		else if (idx2eq(dir, WEST))
			return WEST;
		else
			req(0);
		break;
	case T_NE:
		if (idx2eq(dir, SOUTH)) {
			*normal = rotate90deg_clockwise(*normal);
			return EAST;
		} else if (idx2eq(dir, WEST)) {
			*normal = rotate90deg_counterclockwise(*normal);
			return NORTH;
		} else
			req(0);
		break;
	case T_NW:
		if (idx2eq(dir, SOUTH)) {
			*normal = rotate90deg_counterclockwise(*normal);
			return WEST;
		} else if (idx2eq(dir, EAST)) {
			*normal = rotate90deg_clockwise(*normal);
			return NORTH;
		} else
			req(0);
		break;
	case T_SW:
		if (idx2eq(dir, NORTH)) {
			*normal = rotate90deg_clockwise(*normal);
			return WEST;
		} else if (idx2eq(dir, EAST)) {
			*normal = rotate90deg_counterclockwise(*normal);
			return SOUTH;
		} else
			req(0);
		break;
	case T_SE:
		if (idx2eq(dir, NORTH)) {
			*normal = rotate90deg_counterclockwise(*normal);
			return EAST;
		} else if (idx2eq(dir, WEST)) {
			*normal = rotate90deg_clockwise(*normal);
			return SOUTH;
		} else
			req(0);
		break;
	}
	exit_error("Error: Unknown tile %d", t);
}

void mark_from_neighbor(arr_tile2d loop, idx2 st)
{
	Tile t = at2(loop, st);
	if (t != T_EMPTY)
		return;

	b32 is_outside = 0;
	b32 is_inside = 0;

	idx2 pos = st;
	WALK(loop, NORTH, pos) {
		Tile t = at2(loop, pos);
		if (t == T_OUTSIDE)
			is_outside = 1;
		if (t == T_INSIDE)
			is_inside = 1;
		if (t != T_EMPTY)
			break;
	}
	if (!inside(loop, pos))
		is_outside = 1;

	pos = st;
	WALK(loop, WEST, pos) {
		Tile t = at2(loop, pos);
		if (t == T_OUTSIDE)
			is_outside = 1;
		if (t == T_INSIDE)
			is_inside = 1;
		if (t != T_EMPTY)
			break;
	}
	if (!inside(loop, pos))
		is_outside = 1;

	pos = st;
	WALK(loop, SOUTH, pos) {
		Tile t = at2(loop, pos);
		if (t == T_OUTSIDE)
			is_outside = 1;
		if (t == T_INSIDE)
			is_inside = 1;
		if (t != T_EMPTY)
			break;
	}
	if (!inside(loop, pos))
		is_outside = 1;

	pos = st;
	WALK(loop, EAST, pos) {
		Tile t = at2(loop, pos);
		if (t == T_OUTSIDE)
			is_outside = 1;
		if (t == T_INSIDE)
			is_inside = 1;
		if (t != T_EMPTY)
			break;
	}
	if (!inside(loop, pos))
		is_outside = 1;

	if (is_outside)
		at2(loop, st) = T_OUTSIDE;
	if (is_inside)
		at2(loop, st) = T_INSIDE;
}

b32 is_pipe(Tile t)
{
	switch (t) {
	case T_EMPTY:
	case T_INSIDE:
	case T_OUTSIDE:
		return 0;
	default:
		return 1;
	}
	return 0; /* Unreachable */
}

void walk_loop_and_mark(arr_tile2d loop, idx2 st)
{
	idx2 pos = st;
	WALK(loop, NORTH, pos) {
		Tile t = at2(loop, pos);
		if (is_pipe(t))
			break;
	}
	req(is_pipe(at2(loop, pos)));

	dlog("Starting with pipe at {%lld, %lld}", pos.x, pos.y);

	idx2 normal = SOUTH;

	/* Get start dir */
	idx2 dir = EAST;
	switch (at2(loop, pos)) {
	case T_NS:
		dir = NORTH; /* this case shouldn't be possible? */
		break;
	case T_EW:
		dir = EAST;
		break;
	case T_NE:
		dir = WEST;
		break;
	case T_NW:
		dir = EAST;
		break;
	case T_SW:
		dir = EAST;
		break;
	case T_SE:
		dir = WEST;
		break;
	default:
		break;
	}

	b32 is_inside = 1;

	dlog("pos = {%lld, %lld}, dir = {%lld, %lld}, normal = {%lld, %lld}",
	     pos.x, pos.y, dir.x, dir.y, normal.x, normal.y);

	idx2 loop_st = pos;
	do {
		Tile t = at2(loop, pos);
		dir = tile_to2(t, dir, &normal);
		pos = idx2add(pos, dir);

		idx2 upos = idx2add(pos, normal);
		Tile u = at2(loop, upos);

		dlog("pos = {%lld, %lld}, dir = {%lld, %lld}, normal = {%lld, %lld}",
		     pos.x, pos.y, dir.x, dir.y, normal.x, normal.y);

		if (u == T_INSIDE)
			break; /* is_inside already 1 */

		if (u == T_OUTSIDE) {
			is_inside = 0;
			break;
		}
	} while (!idx2eq(pos, loop_st));

	at2(loop, st) = is_inside ? T_INSIDE : T_OUTSIDE;
}

void mark_all(arr_tile2d loop)
{
	size_t w = width(loop), h = height(loop);

	idx2 i;
	for (i.x = 0; i.x < (i64)w; i.x++) {
		for (i.y = 0; i.y < (i64)h; i.y++) {
			Tile t = at2(loop, i);
			if (t != T_EMPTY)
				continue;
			mark_from_neighbor(loop, i);
			t = at2(loop, i);
			if (t != T_EMPTY) {
				continue;
				dlog("Didn't mark from neighbor");
			}
			dlog("Walk loop to mark");
			walk_loop_and_mark(loop, i);
		}
	}
}

int count_inside(arr_tile2d loop)
{
	size_t w = width(loop), h = height(loop);

	int n = 0;

	size_t i, j;
	for (i = 0; i < h; i++) {
		for (j = 0; j < w; j++) {
			idx2 pos = { j, i };
			n += at2(loop, pos) == T_INSIDE;
		}
	}
	return n;
}

void task2(char *infile)
{
	arr_str input = frl(infile);

	arr_tile2d tiles = { 0 };
	amap(input, tiles, {
		arr_tile row = { 0 };
		while (**it != '\0') {
			apush(row, char2tile(**it));
			scadv(it);
		}
		row;
	});
	dlog("width = %zu, height = %zu", width(tiles), height(tiles));

	idx2 st = find_start(tiles);
	dlog("Start @ %lld, %lld", st.x, st.y);

	arr_tile2d loop = cutout_loop(tiles);
	mark_all_outside(loop);
	Tile t = start2tile(loop, st);
	at2(loop, st) = t;
	dumpTiles(loop, st);
	mark_all(loop);
	ilog(" ");
	dumpTiles(loop, st);
	int r = count_inside(loop);
	ilog("Task2 result = %d", r);
}
