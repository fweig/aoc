#define AOC_MAIN
#include "aoc.h"

typedef struct {
	int id;
	array_int nums;
	array_int winning_nums;
	int nwinners;
} Card;

ARRAY(card, Card);

Card parse_card(char *str)
{
	Card card = { 0 };
	scstrf(&str, "Card");
	scw(&str);
	scintf(&str, &card.id);
	ddlog("Card %d", card.id);
	scstrf(&str, ":");
	scw(&str);

	while (peek(&str) != '|') {
		int num;
		scintf(&str, &num);
		scw(&str);
		ddlog("> Winning %d", num);
		APUSH(card.winning_nums, num);
	}
	scstrf(&str, "|");
	scw(&str);

	while (!isend(&str)) {
		int num;
		scintf(&str, &num);
		scw(&str);
		ddlog("> Pulled %d", num);
		APUSH(card.nums, num);
	}
	APUSH(card.nums, 0);

	return card;
}

int count_points(Card card)
{
	int points = 0;

	int *it;
	AFOR_EACH(card.nums, it) {
		if (ACONTAINS(card.winning_nums, *it))
			points = MAX(2 * points, 1);
	}

	dlog("Card %d: %d points", card.id, points);

	return points;
}

void task1(char *infile)
{
	array_str input = file_read_lines(infile);

	array_card cards = { 0 };
	AMAP(input, cards, parse_card);

	array_int npoints_card = { 0 };
	AMAP(cards, npoints_card, count_points);

	int npoints_total = asumint(npoints_card);
	ilog("T1 result: %d", npoints_total);
}

void explode_card(Card card, array_int ncopies)
{
	int id = card.id - 1;
	int copies = AT(ncopies, id);

	int points = 0;
	int *num;
	AFOR_EACH(card.nums, num) {
		points += ACONTAINS(card.winning_nums, *num);
	}
	dlog("Card %d: %d points", card.id, points);

	while (points > 0) {
		AT(ncopies, id + points) += copies;
		points--;
	}
}

void explode_cards(array_card cards, array_int ncopies)
{
	Card *card;
	AFOR_EACH(cards, card) {
		explode_card(*card, ncopies);
	}
}

void task2(char *infile)
{
	array_str input = file_read_lines(infile);

	array_card cards = { 0 };
	AMAP(input, cards, parse_card);

	array_int ncopies = { 0 };
	ARESIZE(ncopies, cards.size);

	/* Start with 1 copy per card */
	int *n;
	AFOR_EACH(ncopies, n)
		*n = 1;

	explode_cards(cards, ncopies);

	int ncards_total = asumint(ncopies);
	ilog("T2 result: %d", ncards_total);
}
