CC=gcc
CFLAGS=-Wall -Wextra -Werror -std=c89 -O3 -ggdb3 -fopenmp
# CFLAGS=-Wall -Wextra -Werror -std=c89 -Og -ggdb3 -fopenmp


ALL_SOLUTIONS=$(patsubst day%.c,day%,$(wildcard day*.c))

day%: aoc%.h day%.c
	$(CC) $(CFLAGS) -o $@ $@.c

.PHONY: all
all: $(ALL_SOLUTIONS)

.PHONY: clean
clean:
	rm -f $(ALL_SOLUTIONS)
