#ifndef CHTRIE
#define CHTRIE

struct chtrie_edge {
	struct chtrie_edge *next;
	int from, to;
	char sym;
};

typedef struct {
	struct chtrie_edge **etab;
	int *idxpool, *idxptr, idxmax;	/* pool of available node indexes */
	int maxn, alphsz, ecap;
} chtrie;

/* allocate a trie with at most `n` nodes, and the alphabet with size `m`
 *
 * Upon success, return a pointer to the trie.
 * Otherwise, return `NULL` and set `errno`.
 */
chtrie *chtrie_alloc(int n, int m);

/* walk from one node to its child
 *
 * If the child didn't exist and `creat` is non-zero,
 * a new node will be created.
 *
 * Upon success, return the index of the child.
 * Otherwise, return -1.
 *
 * If `creat` is non-zero and this routine fails, `errno` will be set.
 */
int chtrie_walk(chtrie *tr, int from, int sym, int creat);

/* delete a child node
 *
 * If the child doesn't exist, the trie shall be left unchanged.
 *
 * The chile node must be a leaf, or the behavior is undefined.
 */
void chtrie_del(chtrie *tr, int from, int sym);

/* free the trie */
void chtrie_free(chtrie *tr);

#endif
