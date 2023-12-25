CC=g++
CFLAGS=-Wall -Wextra -Werror -Wno-write-strings -Wno-error=unused-parameter -std=c++20 -O3 -ggdb3 -fopenmp
# CFLAGS=-Wall -Wextra -Werror -std=c89 -Og -ggdb3 -fopenmp

.PHONY: all
all: solution aoc_test

aoc_test: aoc.h aoc_test.cpp
	$(CC) $(CFLAGS) -o $@ $@.cpp

solution: aoc.h solution.cpp
	$(CC) $(CFLAGS) -o $@ $@.cpp

.PHONY: clean
clean:
	rm -f solution aoc_test
