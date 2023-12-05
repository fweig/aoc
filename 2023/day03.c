#define AOC_MAIN
#include "aoc.h"

typedef struct {
	int num;
	int xpos;
	int width;
} part_number;

typedef struct {
	char symbol;
	int xpos;
} symbol;

ARRAY(part_number, part_number);
ARRAY(part_number2d, array_part_number);
ARRAY(symbol, symbol);
ARRAY(symbol2d, array_symbol);


int try_parse_symbol(char **line, char *c)
{
	if (**line == '.')
		return 0;

	if (isalnum(**line))
		return 0;

	*c = **line;
	return scadv(line);
}

void parse_schematic_line(char *line, array_symbol *symbols,
			  array_part_number *parts)
{
	int xpos = 0;
	ddlog("Parsing line: '%s'", line);
	while (*line != '\0') {
		int chars_read = 0;

		int num;
		chars_read = scint(&line, &num);
		if (chars_read > 0) {
			part_number part = { .num = num,
					     .xpos = xpos,
					     .width = chars_read };
			APUSH(*parts, part);
			xpos += chars_read;
			ddlog("Part: { .num = %d, .xpos = %d, .width %d}",
			      part.num, part.xpos, part.width);
			continue;
		}

		chars_read = scuntil(&line, '.');
		if (chars_read > 0) {
			ddlog("Empty spaces: %d", chars_read);
			xpos += chars_read;
			continue;
		}

		char sym;
		chars_read = try_parse_symbol(&line, &sym);
		if (chars_read == 1) {
			symbol s = { .symbol = sym, .xpos = xpos };
			APUSH(*symbols, s);
			ddlog("Symbol '%c' at %d", sym, xpos);
			xpos++;
			continue;
		}

		if (chars_read == 0)
			exit_error("Encountered unknown character: '%s'", line);
		exit_error("Unreachable: This should never happen!!!");
	}
}

void parse_schematic(array_str schema, array_symbol2d *symbols,
		     array_part_number2d *parts)
{
	/* Add empty rows for padding at start and end.
	   Only required for symbols, but also add to parts for symmetry. */
	APUSH(*symbols, (array_symbol){ 0 });
	APUSH(*parts, (array_part_number){ 0 });

	char **line;
	AFOR_EACH(schema, line) {
		array_symbol symbols_row = { 0 };
		array_part_number parts_row = { 0 };
		parse_schematic_line(*line, &symbols_row, &parts_row);
		APUSH(*symbols, symbols_row);
		APUSH(*parts, parts_row);
	}

	APUSH(*symbols, (array_symbol){ 0 });
	APUSH(*parts, (array_part_number){ 0 });
}

b32 advance_symbols(array_symbol symbols, int xstart, int xend, size_t *pos)
{
	while (*pos < symbols.size) {
		int xpos = AT(symbols, *pos).xpos;

		if (xpos < xstart) {
			(*pos)++;
			continue;
		}

		if (xpos > xend) {
			break;
		}

		return 1; /* xstart <= xpos <= xend */
	}

	return 0;
}

int sum_part_numbers_row(array_symbol *symbols, array_part_number parts)
{
	int sum = 0;

	array_symbol symbols_above = symbols[-1];
	array_symbol symbols_row = symbols[0];
	array_symbol symbols_below = symbols[1];

	size_t pos[3] = { 0 };

	part_number *part;
	AFOR_EACH(parts, part) {
		b32 has_symbol = 0;

		int xstart = part->xpos - 1;
		int xend = part->xpos + part->width;

		has_symbol |=
			advance_symbols(symbols_above, xstart, xend, &pos[0]);
		has_symbol |=
			advance_symbols(symbols_row, xstart, xend, &pos[1]);
		has_symbol |=
			advance_symbols(symbols_below, xstart, xend, &pos[2]);

		if (has_symbol) {
			dlog("Part %d -> Has Symbol", part->num);
			sum += part->num;
		} else {
			dlog("Part %d -> No Symbol", part->num);
		}
	}

	return sum;
}

int sum_part_numbers(array_symbol2d symbols, array_part_number2d parts)
{
	int sum = 0;

	size_t row; /* Skip padding at end and start */
	for (row = 1; row < symbols.size - 1; row++) {
		int sum_row =
			sum_part_numbers_row(symbols.d + row, parts.d[row]);
		dlog("Row %zu: sum = %d", row - 1, sum_row);
		sum += sum_row;
	}

	return sum;
}

void task1(char *fname)
{
	array_str schematic = file_read_lines(fname);

	array_symbol2d symbols = { 0 };
	array_part_number2d parts = { 0 };
	parse_schematic(schematic, &symbols, &parts);

	int sum = sum_part_numbers(symbols, parts);
	ilog("Result = %d", sum);
}

void advance_parts(array_part_number parts, int xpos, array_int *adjacent,
		   size_t *pos)
{
	while (*pos < parts.size) {
		part_number part = AT(parts, *pos);
		int xstart = part.xpos - 1;
		int xend = part.xpos + part.width;

		ddlog("Check part %d: xstart = %d, xpos = %d, xend = %d",
		      part.num, xstart, xpos, xend);

		if (xpos > xend) {
			ddlog("Advance pos");
			(*pos)++;
			continue;
		}

		if (xpos < xstart) {
			break;
		}

		ddlog("pos = %zu: found adjacent part %d", *pos, part.num);
		APUSH(*adjacent, part.num);

		/* Peak at next part if both neighbors are on the same line */
		size_t next_pos = *pos + 1;
		if (next_pos < parts.size) {
			part = AT(parts, next_pos);
			xstart = part.xpos - 1;
			xend = part.xpos + part.width;
			if (xpos >= xstart && xpos <= xend) {
				ddlog("pos = %zu: found adjacent while peeking part %d",
				      *pos, part.num);
				APUSH(*adjacent, part.num);
			}
		}
		break;
	}
}

int sum_gear_ratios_row(array_symbol symbols, array_part_number *parts)
{
	int sum = 0;

	size_t pos[3] = { 0 };

	array_part_number parts_above = parts[-1];
	array_part_number parts_row = parts[0];
	array_part_number parts_below = parts[1];

	symbol *sym;
	AFOR_EACH(symbols, sym) {
		if (sym->symbol != '*')
			continue;

		ddlog("Check gear @ %d", sym->xpos);

		array_int adjacent_parts = { 0 };

		ddlog("Row above");
		advance_parts(parts_above, sym->xpos, &adjacent_parts, &pos[0]);
		ddlog("Row current");
		advance_parts(parts_row, sym->xpos, &adjacent_parts, &pos[1]);
		ddlog("Row below: %zu parts", parts_below.size);
		advance_parts(parts_below, sym->xpos, &adjacent_parts, &pos[2]);

		if (adjacent_parts.size == 2) {
			int gear_ratio =
				AT(adjacent_parts, 0) * AT(adjacent_parts, 1);
			sum += gear_ratio;
		}
	}

	return sum;
}

int sum_gear_ratios(array_symbol2d symbols, array_part_number2d parts)
{
	int sum = 0;

	size_t row; /* Skip padding at end and start */
	for (row = 1; row < symbols.size - 1; row++) {
		dlog("Row %zu", row - 1);
		int sum_row =
			sum_gear_ratios_row(AT(symbols, row), &parts.d[row]);
		sum += sum_row;
	}

	return sum;
}

void task2(char *fname)
{
	array_str schematic = file_read_lines(fname);

	array_symbol2d symbols = { 0 };
	array_part_number2d parts = { 0 };
	parse_schematic(schematic, &symbols, &parts);

	int sum = sum_gear_ratios(symbols, parts);
	ilog("T2 Result = %d", sum);
}
