#define AOC_IMPL
#include "aoc08.h"

typedef struct {
	char name[4];
	int left;
	int right;
} Node;
ARRAY(node, Node);

void dlog_node(arr_node nodes, int i)
{
	Node *n = &at(nodes, i);
	char *lname = at(nodes, n->left).name;
	char *rname = at(nodes, n->right).name;
	dlog("%s[%d] = (%s[%d], %s[%d])", n->name, i, lname, n->left, rname,
	     n->right);
}

b32 is_end_node(arr_node nodes, i64 *i)
{
	Node *n = &at(nodes, *i);
	return n->name[2] == 'Z';
}

int find(arr_node *nodes, char *name)
{
	Node *n;
	aforeach(*nodes, n) {
		if (strncmp(name, n->name, 3) == 0)
			return n - nodes->d;
	}
	return -1;
}

int find_or_emplace(arr_node *nodes, char *name)
{
	int idx = find(nodes, name);
	if (idx > -1)
		return idx;

	Node n = { 0 };
	memcpy(n.name, name, sizeof(char) * 3);
	apush(*nodes, n);
	return nodes->size - 1;
}

arr_node parse_nodes(arr_str input)
{
	arr_node graph = { 0 };

	char **ln;
	aforeachr(input, ln, 2, -1) {
		char name[4];
		scn(ln, name, 3);
		scstr(ln, " = (");

		char lname[4];
		scn(ln, lname, 3);
		scstr(ln, ", ");

		char rname[4];
		scn(ln, rname, 3);
		scstr(ln, ")");
		req(isend(ln));

		int i = find_or_emplace(&graph, name);
		Node *n = &at(graph, i);
		n->left = find_or_emplace(&graph, lname);
		n->right = find_or_emplace(&graph, rname);

		dlog_node(graph, i);
	}

	return graph;
}

int walk(arr_node graph, char *steps)
{
	int n = 0;
	size_t steps_len = strlen(steps);

	int i = find(&graph, "AAA");
	req(i > -1);

	while (1) {
		size_t s;
		for (s = 0; s < steps_len; s++) {
			Node *node = &at(graph, i);
			if (strcmp(node->name, "ZZZ") == 0)
				return n;
			n++;
			i = steps[s] == 'L' ? node->left : node->right;
		}
	}
}

void task1(char *inf)
{
	arr_str input = frl(inf);
	char *steps = first(input);
	req(isend(&at(input, 1)));
	arr_node graph = parse_nodes(input);

	int n = walk(graph, steps);
	ilog("Result: %d", n);
}

arr_int start_nodes(arr_node graph)
{
	arr_int idx = { 0 };
	i64 i;
	for (i = 0; (size_t)i < graph.size; i++) {
		if (at(graph, i).name[2] == 'A')
			apush(idx, i);
	}
	return idx;
}

/*
u64 walk2(arr_node graph, char *steps)
{
	u64 n = 0;
	size_t steps_len = strlen(steps);

	arr_int idx = { 0 };
	i64 i;
	for (i = 0; (size_t)i < graph.size; i++) {
		if (at(graph, i).name[2] == 'A')
			apush(idx, i);
	}
	dlog("got %zu start nodes", idx.size);

	while (1) {
		size_t s;
		for (s = 0; s < steps_len; s++) {
			if (aall(idx, curry(is_end_node, graph)))
				return n;

			i64 *j;
			if (steps[s] == 'L') {
				aforeach(idx, j) {
					Node *node = &at(graph, *j);
					*j = node->left;
				}
			} else {
				aforeach(idx, j) {
					Node *node = &at(graph, *j);
					*j = node->right;
				}
			}

			if (n % (1ul << 31) == 0)
				dlog("%llu", n);
			n++;
		}
	}
}
*/

u64 walk_end(arr_node graph, char *steps, i64 st)
{
	int n = 0;
	size_t steps_len = strlen(steps);

	int i = st;
	req(i > -1);

	while (1) {
		size_t s;
		for (s = 0; s < steps_len; s++) {
			Node *node = &at(graph, i);
			if (node->name[2] == 'Z')
				return n;
			n++;
			i = steps[s] == 'L' ? node->left : node->right;
		}
	}
}

u64 walk_cycle(arr_node graph, char *steps, i64 st)
{
	int n = 0;
	size_t steps_len = strlen(steps);

	int i = st;
	req(i > -1);

	arr_int node_visited = { 0 };
	aresize(node_visited, graph.size);
	memset(node_visited.d, 0, vsize(node_visited) * node_visited.size);

	while (1) {
		if (at(node_visited, i)) {
			return n;
		}
		at(node_visited, i) = 1;

		size_t s;
		for (s = 0; s < steps_len; s++) {
			Node *node = &at(graph, i);
			n++;
			i = steps[s] == 'L' ? node->left : node->right;
		}
	}
}

void task2(char *inf)
{
	arr_str input = frl(inf);
	char *steps = first(input);
	req(isend(&at(input, 1)));
	arr_node graph = parse_nodes(input);

	arr_int st = start_nodes(graph);
	dlog("got %zu start nodes", st.size);

	arr_int steps_goal = { 0 };
	amapb(st, steps_goal, { walk_end(graph, steps, *it); });

	size_t slen = strlen(steps);
	dlog("nsteps = %zu", slen);

	i64 *it;
	aforeach(steps_goal, it) {
		i64 x = *it / strlen(steps);
		i64 y = *it % 2;
		dlog("goal: %llu + %llu = %llu steps", x, y, *it);
	}

	i64 x = amul(steps_goal);
	dlog("Result? %lld", x);

	aforeach(steps_goal, it) {
		*it /= slen;
	}

	x = amul(steps_goal);
	dlog("Result 2? %lld", x * slen);

	arr_int steps_cycle = { 0 };
	amapb(st, steps_cycle, { walk_cycle(graph, steps, *it); });
	aforeach(steps_cycle, it) {
		dlog("cycle: %llu steps", *it);
	}

	/*
	int n = walk2(graph, steps);
	ilog("Result: %d", n);
	*/
}
