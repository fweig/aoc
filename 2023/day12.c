#define AOC_IMPL
#if 1
#define NO_DLOG
#endif
#include "aoc12.h"

typedef struct {
	char *conditions;
	size_t conditions_len;
	arr_int groups;
} Record;
ARRAY(record, Record);

typedef struct {
	i64 *nrecords;
	i64 ngroups;
	i64 nconditions;
} Cache;

Cache make_cache(Record r)
{
	Cache c = { 0 };
	c.ngroups = r.groups.size + 2;
	c.nconditions = r.conditions_len + 2;

	size_t nentries = 2 * c.ngroups * c.nconditions;
	dlog("nentries = 2 * %lld * %lld = %zu", c.ngroups, c.nconditions,
	     nentries);

	c.nrecords = ymalloc(nentries * sizeof(*c.nrecords));

	size_t i;
	for (i = 0; i < nentries; i++)
		c.nrecords[i] = -1;

	return c;
}

i64 cache_get(Cache c, i64 group, i64 cond, char sym)
{
	i64 z = (sym == '.' ? 0 : 1);
	/* dlog("cache idx = %lld * %lld * 2 + %lld * 2 + %lld = %lld", group, c.nconditions, cond, z, group * c.nconditions * 2 + cond * 2 + z); */
	return c.nrecords[group * c.nconditions * 2 + cond * 2 + z];
}

void cache_set(Cache c, i64 group, i64 cond, char sym, i64 val)
{
	i64 z = (sym == '.' ? 0 : 1);
	/* dlog("cache idx = %lld", group * c.nconditions * 2 + cond * 2 + z); */
	c.nrecords[group * c.nconditions * 2 + cond * 2 + z] = val;
}

#ifdef NO_DLOG
#define dump_record(...) ((void)0)
#else
void dump_record(char *prefix, Record r)
{
	if (loglevel < L_DEBUG)
		return;
	fputs("D ", stdout);
	if (prefix != NULL)
		fputs(prefix, stdout);
	fputs(r.conditions, stdout);
	fputc(' ', stdout);
	i64 *it;
	aforeach(r.groups, it) {
		fprintf(stdout, "%lld,", *it);
	}
	fputc('\n', stdout);
}
#endif

Record parse_record(char *in)
{
	Record r = { 0 };

	int conditions_len = scuntil(&in, ' ') - 1;
	req(conditions_len > 0);
	r.conditions = ymalloc(conditions_len + 1);
	memcpy(r.conditions, &in[-conditions_len - 1], conditions_len);
	r.conditions[conditions_len] = '\0';
	r.conditions_len = strlen(r.conditions);

	while (*in != '\0') {
		i64 group;
		scintf(&in, &group);
		apush(r.groups, group);
		scchar(&in, ',');
	}

	return r;
}

i64 possible_records(Record r, size_t condoffset, size_t group, i64 seqlen,
		     Cache cache)
{
	(void)cache;
	dump_record("B: ", r);

#if 1
	if (r.conditions[condoffset] == '.') {
		i64 cached = cache_get(cache, group, condoffset,
				       r.conditions[condoffset]);
		if (cached > -1) {
			dlog("Cache Hit: g=%zu, c=%zu, x=%c, val=%lld", group,
			     condoffset, r.conditions[condoffset], cached);
			return cached;
		}
	}
#endif

	size_t c = condoffset;
	while (r.conditions[c] == '.') {
		seqlen = 0;
		c++;
	}

	if (group > r.groups.size)
		group = r.groups.size;

	size_t condlen = r.conditions_len;
	if (c >= condlen) {
		if (group >= r.groups.size) {
			dump_record("V: ", r);
			return 1;
		}
		dump_record("I: ", r);
		return 0;
	}

	i64 len = group < r.groups.size ? at(r.groups, group) : 0;

	i64 comb = 0;
	while (c + seqlen <= condlen) {
		char *x = r.conditions + c + seqlen;

		if (c + seqlen == condlen) {
			if (seqlen == len)
				comb += possible_records(r, c + seqlen,
							 group + 1, 0, cache);
			break;
		}

		if (seqlen == len) {
			if (*x == '.')
				comb += possible_records(r, c + seqlen,
							 group + 1, 0, cache);
			else if (*x == '?') {
				*x = '.';
				comb += possible_records(r, c + seqlen,
							 group + 1, 0, cache);
				*x = '?';
			} else { /* *x == '#' -> Sequence too long */
				dump_record("I (too long): ", r);
				comb = 0;
			}

			break;
		}

		if (*x == '?') {
			*x = '#';
			comb += possible_records(r, c, group, seqlen, cache);
			*x = '.';
			comb += possible_records(r, c, group, seqlen, cache);
			*x = '?';
			break;
		}
		if (*x == '.') {
			if (seqlen < len)
				break;
			c++;
			continue;
		}

		/* *x == '#' */
		seqlen++;
	}

	dump_record("E: ", r);
	dlog("Cached: g=%zu, c=%zu, x=%c, val=%lld", group, condoffset,
	     r.conditions[condoffset], comb);
	if (r.conditions[condoffset] == '.')
		cache_set(cache, group, condoffset, r.conditions[condoffset],
			  comb);
	return comb;
}

b32 is_possible(arr_int groups, char *config)
{
	size_t g;
	for (g = 0; g < groups.size; g++) {
		i64 len = at(groups, g);
		i64 seqlen = 0;
		while (*config == '.')
			config++;
		while (*config == '#') {
			seqlen++;
			config++;
		}
		if (seqlen == len)
			continue;
		return 0;
	}
	while (*config == '.')
		config++;
	return *config == '\0';
}

void all_records(char *c, size_t x, arr_str *out)
{
	while (c[x] == '.' || c[x] == '#')
		x++;

	if (c[x] == '\0') {
		size_t len = strlen(c);
		char *cp = ymalloc(len + 1);
		memcpy(cp, c, len + 1);
		apush(*out, cp);
		return;
	}

	if (c[x] == '?') {
		c[x] = '.';
		all_records(c, x + 1, out);
		c[x] = '#';
		all_records(c, x + 1, out);
		c[x] = '?';
	}
}

i64 possible_records_bruteforce(Record r)
{
	arr_str setups = { 0 };
	all_records(r.conditions, 0, &setups);
	return acount(setups, { is_possible(r.groups, *it); });
}

void task1(char *infile)
{
	arr_str input = frl(infile);

	arr_record records = { 0 };
	amap(input, records, { parse_record(*it); });

	arr_int n = { 0 };
	amapr(records, n, 0, -1, {
		Cache c = make_cache(*it);
		possible_records(*it, 0, 0, 0, c);
	});
	ilog("Task 1 result: %lld", asum(n));
}

Record parse_record_unfold(char *in)
{
	Record r = { 0 };

	int conditions_len = scuntil(&in, ' ') - 1;
	req(conditions_len > 0);
	r.conditions = ymalloc((conditions_len + 1) * 5);
	size_t f;
	for (f = 0; f < 5; f++) {
		memcpy(r.conditions + (conditions_len + 1) * f,
		       &in[-conditions_len - 1], conditions_len);
		r.conditions[(conditions_len + 1) * f + conditions_len] = '?';
	}
	r.conditions[5 * (conditions_len + 1) - 1] = '\0';
	r.conditions_len = strlen(r.conditions);

	while (*in != '\0') {
		i64 group;
		scintf(&in, &group);
		apush(r.groups, group);
		scchar(&in, ',');
	}

	size_t osize = r.groups.size;
	for (f = 0; f < 4; f++) {
		size_t i;
		for (i = 0; i < osize; i++) {
			apush(r.groups, at(r.groups, i));
		}
	}

	dump_record("", r);

	return r;
}

void task2(char *infile)
{
	arr_str input = frl(infile);

	arr_record records = { 0 };
	amap(input, records, { parse_record_unfold(*it); });

	ilog("N records: %zu", records.size);

	arr_int n = { 0 };
	amap_parallel(records, n, {
		Cache c = make_cache(*it);
		i64 x = possible_records(*it, 0, 0, 0, c);
		ilog("%zu(%d): %lld, groups = %zu", _i, omp_get_thread_num(), x,
		     it->groups.size);
		x;
	});
	ilog("Task 2 result: %lld", asum(n));
}
