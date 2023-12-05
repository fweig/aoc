#define AOC_MAIN
#include "aoc.h"
#include "common.h"
#include <ctype.h>

void task1(char *input)
{
	array_str lines = file_read_lines(input);

	int calibsum = 0;
	char **iter;
	AFOR_EACH(lines, iter) {
		char *line = *iter;
		int fdigit = -1;
		int ldigit = -1;
		char c = 0;
		for (c = *line; c != '\0'; c = *++line) {
			if (isdigit(c)) {
				if (fdigit == -1) {
					fdigit = c - '0';
				}
				ldigit = c - '0';
			}
		}
		if (fdigit == -1 || ldigit == -1) {
			exit_error("Invalid input: %s\n", line);
		}
		dlog("%s -> %d", *iter, fdigit * 10 + ldigit);
		calibsum += fdigit * 10 + ldigit;
	}
	ilog("Task1: %d", calibsum);
}

int t2_consume_digit(char **line)
{
	char c = **line;
	int v = -1;
	if (isdigit(c)) {
		v = c - '0';
	}
	if (strncmp(*line, "one", 3) == 0) {
		v = 1;
	}
	if (strncmp(*line, "two", 3) == 0) {
		v = 2;
	}
	if (strncmp(*line, "three", 5) == 0) {
		v = 3;
	}
	if (strncmp(*line, "four", 4) == 0) {
		v = 4;
	}
	if (strncmp(*line, "five", 4) == 0) {
		v = 5;
	}
	if (strncmp(*line, "six", 3) == 0) {
		v = 6;
	}
	if (strncmp(*line, "seven", 5) == 0) {
		v = 7;
	}
	if (strncmp(*line, "eight", 5) == 0) {
		v = 8;
	}
	if (strncmp(*line, "nine", 4) == 0) {
		v = 9;
	}
	if (**line != '\0') {
		(*line)++;
	}
	return v;
}

int t2_parse(char *line)
{
	int fdigit = -1;
	do {
		fdigit = t2_consume_digit(&line);
	} while (*line != '\0' && fdigit == -1);

	if (fdigit == -1) {
		exit_error("Invalid input: %s\n", line);
	}

	int ldigit = fdigit;
	do {
		int c = t2_consume_digit(&line);
		ddlog("c: %d", c);
		if (c != -1) {
			ldigit = c;
		}
	} while (*line != '\0');

	return fdigit * 10 + ldigit;
}

void task2(char *input)
{
	array_str lines = file_read_lines(input);

	int calibsum = 0;
	char **iter;
	AFOR_EACH(lines, iter) {
		char *line = *iter;
		int c = t2_parse(line);
		dlog("%s -> %d", line, c);
		calibsum += c;
	}
	ilog("Task2: %d", calibsum);
}
