#define AOC_IMPL
#include "aoc09.h"

i64 predict(arr_int seq)
{
	dlog_ints("Got seq:", seq);
	b32 allzero = aall(seq, { *it == 0; });
    if (allzero) {
        dlog("Got zero seq");
        return 0;
    }

	arr_int seqdiff = { 0 };
	afold(seq, seqdiff, { b - a; });

    i64 lst = last(seq);
	i64 offset = predict(seqdiff);
    i64 res = lst + offset;
    dlog("Returning %lld + %lld = %lld", lst, offset, res);
	return res;
}

void task1(char *infile)
{
	arr_str input = frl(infile);
	arr_int2d seqs = { 0 };
	amap(input, seqs, {
		arr_int seq = { 0 };
		scints(it, &seq);
		req(isend(it));
		seq;
	});

    arr_int predictions = { 0 };
    amap(seqs, predictions, { predict(*it); });
    dlog_ints("Extrapolated:", predictions);

    i64 res = asum(predictions);
    ilog("Result: %lld", res);
}

i64 predict_prev(arr_int seq)
{
	dlog_ints("Got seq:", seq);
	b32 allzero = aall(seq, { *it == 0; });
	if (allzero) {
		dlog("Got zero seq");
		return 0;
	}

	arr_int seqdiff = { 0 };
	afold(seq, seqdiff, { b - a; });

	i64 lst = first(seq);
	i64 offset = predict_prev(seqdiff);
	i64 res = lst - offset;
	dlog("Returning %lld - %lld = %lld", lst, offset, res);
	return res;
}

void task2(char *infile)
{
	arr_str input = frl(infile);
	arr_int2d seqs = { 0 };
	amap(input, seqs, {
		arr_int seq = { 0 };
		scints(it, &seq);
		req(isend(it));
		seq;
	});

	arr_int predictions = { 0 };
	amap(seqs, predictions, { predict_prev(*it); });
	dlog_ints("Extrapolated:", predictions);

	i64 res = asum(predictions);
	ilog("Result: %lld", res);
}
