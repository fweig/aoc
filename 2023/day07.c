#define AOC_IMPL
#include "aoc07.h"

typedef enum {
	T_HIGH = 0,
	T_1PAIR = 1,
	T_2PAIR = 2,
	T_THRUPLE = 3,
	T_FULLHOUSE = 4,
	T_FOUR = 5,
	T_FIVE = 6,
} Type;

char *type2str(Type t)
{
	switch (t) {
	case T_HIGH:
		return "High card";
	case T_1PAIR:
		return "One pair";
	case T_2PAIR:
		return "Two pair";
	case T_THRUPLE:
		return "Three of a kind";
	case T_FULLHOUSE:
		return "Full house";
	case T_FOUR:
		return "Four of a kind";
	case T_FIVE:
		return "Five of a kind";
	}
	return "Unknown";
}

typedef struct {
	Type type;
	i64 power;
	i64 bid;
} Hand;
ARRAY(hand, Hand);

void power2str(i64 power, char *out)
{
	int i;
	for (i = 4; i >= 0; i--) {
		i64 x = power % 13;
		char c = 0;
		switch (x) {
		case 0:
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
		case 7:
			c = x + '2';
			break;
		case 8:
			c = 'T';
			break;
		case 9:
			c = 'J';
			break;
		case 10:
			c = 'Q';
			break;

		case 11:
			c = 'K';
			break;

		case 12:
			c = 'A';
			break;
		}
		dlog("c = %c", c);
		out[i] = c;
		power /= 13;
	}
	out[5] = '\0';
}

void dlog_hand(Hand h)
{
	char cards[6] = { 0 };
	power2str(h.power, cards);
	dlog("Hand: .type = %s, .power = %s, .bid = %lld", type2str(h.type),
	     cards, h.bid);
}

int comp_hands(const void *a1, const void *a2)
{
	const Hand *c1 = a1;
	const Hand *c2 = a2;

	if (c1->type < c2->type)
		return -1;

	if (c1->type > c2->type)
		return 1;

	/* c1->type == c2->type */
	return (c1->power > c2->power) - (c1->power < c2->power);
}

i64 card2power(char c)
{
	switch (c) {
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
		return c - '2';
	case 'T':
		return 8;
	case 'J':
		return 9;
	case 'Q':
		return 10;
	case 'K':
		return 11;
	case 'A':
		return 12;
	}
	exit_error("Unreachable");
}

Type find_type(char *cards)
{
	int n[13] = { 0 };
	int i;
	for (i = 0; i < 5; i++) {
		n[card2power(cards[i])]++;
	}
	int npairs = 0;
	int nthrouple = 0;
	int nquad = 0;
	int nfives = 0;
	for (i = 0; i < 13; i++) {
		switch (n[i]) {
		case 2:
			npairs++;
			break;
		case 3:
			nthrouple++;
			break;
		case 4:
			nquad++;
			break;
		case 5:
			nfives++;
			break;
		default:
			break;
		}
	}
	if (nfives == 1)
		return T_FIVE;
	if (nquad == 1)
		return T_FOUR;
	if (nthrouple == 1 && npairs == 1)
		return T_FULLHOUSE;
	if (nthrouple == 1 && npairs == 0)
		return T_THRUPLE;
	if (npairs == 2)
		return T_2PAIR;
	if (npairs == 1)
		return T_1PAIR;
	return T_HIGH;
}

Hand parse_hand(char *ln)
{
	Hand h = { 0 };
	h.type = find_type(ln);
	int i;
	for (i = 0; i < 5; i++) {
		h.power = h.power * 13 + card2power(ln[i]);
	}
	ln += 5;
	scw(&ln);
	scintf(&ln, &h.bid);
	dlog_hand(h);
	return h;
}

i64 total_winnings(arr_hand hands)
{
	dlog("Determine winnings");
	qsort(hands.d, hands.size, vsize(hands), comp_hands);

	i64 winnings = 0;

	size_t i;
	for (i = 0; i < hands.size; i++) {
		dlog_hand(at(hands, i));
		winnings += (i + 1) * at(hands, i).bid;
	}

	return winnings;
}

void task1(char *infile)
{
	arr_str input = file_read_lines(infile);

	arr_hand hands = { 0 };
	amap(input, hands, parse_hand);

	i64 winnings = total_winnings(hands);
	ilog("Result: %lld", winnings);
}

char power2card(i64 power)
{
	char c = 0;
	switch (power) {
	case 0:
		c = 'J';
		break;
	case 1:
	case 2:
	case 3:
	case 4:
	case 5:
	case 6:
	case 7:
	case 8:
		c = power + '1';
		break;
	case 9:
		c = 'T';
		break;
	case 10:
		c = 'Q';
		break;
	case 11:
		c = 'K';
		break;
	case 12:
		c = 'A';
		break;
	}
	return c;
}

void power2str2(i64 power, char *out)
{
	int i;
	for (i = 4; i >= 0; i--) {
		i64 x = power % 13;
		char c = power2card(x);
		dlog("c = %c", c);
		out[i] = c;
		power /= 13;
	}
	out[5] = '\0';
}

i64 card2power2(char c)
{
	switch (c) {
	case 'J':
		return 0;
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
		return c - '1';
	case 'T':
		return 9;
	case 'Q':
		return 10;
	case 'K':
		return 11;
	case 'A':
		return 12;
	}
	exit_error("Unreachable");
}

Type find_type2(char *cards)
{
	int n[13] = { 0 };
	int i;
	for (i = 0; i < 5; i++) {
		n[card2power2(cards[i])]++;
	}
	int npairs = 0;
	int nthrouple = 0;
	int nquad = 0;
	int nfives = 0;
	for (i = 0; i < 13; i++) {
		switch (n[i]) {
		case 2:
			npairs++;
			break;
		case 3:
			nthrouple++;
			break;
		case 4:
			nquad++;
			break;
		case 5:
			nfives++;
			break;
		default:
			break;
		}
	}
	if (nfives == 1)
		return T_FIVE;
	if (nquad == 1)
		return T_FOUR;
	if (nthrouple == 1 && npairs == 1)
		return T_FULLHOUSE;
	if (nthrouple == 1 && npairs == 0)
		return T_THRUPLE;
	if (npairs == 2)
		return T_2PAIR;
	if (npairs == 1)
		return T_1PAIR;
	return T_HIGH;
}

Type find_type_wildcards(char *ln, int st)
{
	if (st == 5)
		return find_type2(ln);

	Type best = T_HIGH;
	char old[5];
	memcpy(old, ln, 5 * sizeof(char));

	if (ln[st] != 'J') {
		best = find_type_wildcards(ln, st+1);
	} else {
		int i;
		for (i = 1; i < 13; i++) {
			ln[st] = power2card(i);
			best = MAX(best, find_type_wildcards(ln, st+1));
		}
	}

	memcpy(ln, old, 5 * sizeof(char));
	return best;
}

Hand parse_hand2(char *ln)
{
	Hand h = { 0 };
	h.type = find_type_wildcards(ln, 0);
	int i;
	for (i = 0; i < 5; i++) {
		h.power = h.power * 13 + card2power2(ln[i]);
	}
	ln += 5;
	scw(&ln);
	scintf(&ln, &h.bid);
	dlog_hand(h);
	return h;
}

void task2(char *infile)
{
	arr_str input = file_read_lines(infile);

	arr_hand hands = { 0 };
	amap(input, hands, parse_hand2);

	i64 winnings = total_winnings(hands);
	ilog("Result: %lld", winnings);
}
