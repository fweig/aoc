CC=gcc
CFLAGS=-Wall -Wextra -Werror -std=c89 -O2 -ggdb3

ALL_SOLUTIONS=$(patsubst day%.c,day%,$(wildcard day*.c))

day%: common.h common.c aoc.h day%.c
	$(CC) $(CFLAGS) -o $@ common.c $@.c

.PHONY: all
all: $(ALL_SOLUTIONS)

.PHONY: clean
clean:
	rm -f $(ALL_SOLUTIONS)
