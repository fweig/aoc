#ifndef AOC_H
#define AOC_H

#include "common.h"

void task1(char *input_file);
void task2(char *input_file);

int main(int argc, char *argv[]);

#ifdef AOC_MAIN
int main(int argc, char *argv[])
{
	if (argc != 2) {
		printf("Usage: %s <input_file>\n", argv[0]);
		return 1;
	}
	char *input_file = argv[1];
	if (!testenv("NOTASK1")) {
		ilog(">>> Task1");
		task1(input_file);
	}
	yfree();
	if (!testenv("NOTASK2")) {
		ilog(">>> Task2");
		task2(input_file);
	}
	yfree();
	return 0;
}
#endif

#endif
