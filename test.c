#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "chtrie.h"
#define fatal(s) do { perror(s); exit(-1); } while (0)
#define N 65536
#define M 256

static char *dict1[] = { "", "the", "a", "an" };
static char *dict2[] = { "he", "she", "his", "hers" };
static char *dict3[] = { "this", "that" };
static char *stop[] = { "the", "an", "a" };

static chtrie *tr;

static int term[N];	/* is `i` a  termination node */
static int nchild[N];	/* number of children of `i` */

static void add(char *s);
static void del(char *s);
static int query(char *s);

int main(void)
{
	static char line[N];
	int i, j;

	if (!(tr = chtrie_alloc(N, M)))
		fatal("chtrie_alloc");
	for (i = 0; i < sizeof dict1 / sizeof dict1[0]; i++)
		add(dict1[i]);
	for (i = 0; i < sizeof dict2 / sizeof dict2[0]; i++)
		add(dict2[i]);
	for (i = 0; i < sizeof stop / sizeof stop[0]; i++)
		del(stop[i]);
	for (i = 0; i < sizeof dict3 / sizeof dict3[0]; i++)
		add(dict3[i]);

	while (fgets(line, sizeof line, stdin)) {
		line[strcspn(line, "\n")] = '\0';
		printf("%d\n", query(line) ? 1 : 0);
	}
	return 0;
}

static void add(char *s)
{
	int it;

	it = 0;
	while (*s) {
		if (chtrie_walk(tr, it, (unsigned char)*s, 0) == -1)
			nchild[it]++;
		if ((it = chtrie_walk(tr, it, (unsigned char)*s, 1)) == -1)
			fatal("chtrie_walk");
		s++;
	}
	term[it] = 1;
}

static void del(char *s)
{
	static int nodes[N], symbs[N], n;	/* trace the path */
	int it;

	n = 0;
	it = 0;
	while (it >= 0 && *s) {
		nodes[n] = it;
		symbs[n] = *s;
		n++;
		it = chtrie_walk(tr, it, (unsigned char)*s++, 0);
	}
	if (it < 0 || !term[it])
		return;
	term[it] = 0;
	while (it > 0 && !term[it] && nchild[it] == 0) {
		n--;
		chtrie_del(tr, nodes[n], symbs[n]);
		it = nodes[n];
		nchild[it]--;
	}
}

static int query(char *s)
{
	int it;

	it = 0;
	while (it >= 0 && *s)
		it = chtrie_walk(tr, it, (unsigned char)*s++, 0);
	return it >= 0 && term[it];
}
