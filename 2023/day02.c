#define AOC_MAIN
#include "aoc.h"
#include <ctype.h>

typedef struct {
	int nred;
	int ngreen;
	int nblue;
} cubecnt;

typedef enum {
	RED,
	GREEN,
	BLUE,
} color;

void parse_whitespace(char **line)
{
	for (; **line != '\0' && isspace(**line); ++*line) {
	}
}

b32 try_parse_char(char **line, char c)
{
	if (**line == c) {
		++*line;
		return 1;
	}
	return 0;
}

int parse_int(char **line)
{
	if (!isdigit(**line))
		exit_error("Couldn't parse int: \"%s\"\n", *line);

	int v = 0;
	for (; **line != '\0' && isdigit(**line); ++*line) {
		v = v * 10 + (**line - '0');
	}
	return v;
}

color parse_color(char **line)
{
	if (strncmp(*line, "red", 3) == 0) {
		*line += 3;
		return RED;
	}
	if (strncmp(*line, "green", 5) == 0) {
		*line += 5;
		return GREEN;
	}
	if (strncmp(*line, "blue", 4) == 0) {
		*line += 4;
		return BLUE;
	}
	exit_error("Couldn't parse color: %s\n", *line);
	return RED; /* unreachable */
}

void parse_prefix(char **line)
{
	for (; !isend(line) && **line != ':'; ++*line) {
	}
	if (isend(line))
		exit_error("Couldn't parse prefix: %s\n", *line);
	++*line; /* consume ':' */
}

cubecnt parse_round(char **line)
{
	cubecnt cubes = { 0 };

	while (!isend(line)) {
		parse_whitespace(line);
		int cnt = parse_int(line);
		parse_whitespace(line);
		color c = parse_color(line);

		switch (c) {
		case RED:
			cubes.nred += cnt;
			break;
		case GREEN:
			cubes.ngreen += cnt;
			break;
		case BLUE:
			cubes.nblue += cnt;
			break;
		}

		parse_whitespace(line);
		b32 round_cont = try_parse_char(line, ',');
		b32 round_fin = try_parse_char(line, ';');

		if (round_fin)
			break;

		if (!round_cont && !isend(line))
			exit_error(
				"Unexpected char (expected ',' or ';'): %s\n",
				*line);
	}

	return cubes;
}

cubecnt parse_game(char *line)
{
	parse_prefix(&line);

	cubecnt maxcubes = { 0 };
	while (*line != '\0') {
		cubecnt cnt = parse_round(&line);
		maxcubes.nred = MAX(maxcubes.nred, cnt.nred);
		maxcubes.ngreen = MAX(maxcubes.ngreen, cnt.ngreen);
		maxcubes.nblue = MAX(maxcubes.nblue, cnt.nblue);
	}
	return maxcubes;
}

void task1(char *file)
{
	array_str lines = file_read_lines(file);
	cubecnt target = { .nred = 12, .ngreen = 13, .nblue = 14 };

	int gameid = 1;
	int sum_possible_games = 0;

	char **game;
	AFOR_EACH(lines, game) {
		cubecnt maxcubes = parse_game(*game);

		b32 is_possible = maxcubes.nred <= target.nred &&
				  maxcubes.ngreen <= target.ngreen &&
				  maxcubes.nblue <= target.nblue;

		if (is_possible)
			sum_possible_games += gameid;

		dlog("%s -> %d %d %d (%d)", *game, maxcubes.nred,
		     maxcubes.ngreen, maxcubes.nblue, is_possible);

		gameid++;
	}

	ilog("T1 Result: %d", sum_possible_games);
}

void task2(char *file)
{
	array_str lines = file_read_lines(file);

	int setpower_sum = 0;

	char **game;
	AFOR_EACH(lines, game) {
		cubecnt maxcubes = parse_game(*game);
		dlog("%s -> %d %d %d", *game, maxcubes.nred, maxcubes.ngreen,
		     maxcubes.nblue);

		int setpower = maxcubes.nred * maxcubes.ngreen * maxcubes.nblue;
		setpower_sum += setpower;
	}

	ilog("T2 Result: %d", setpower_sum);
}
