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

/* allocate a trie with at most `n` nodes, and the alphabet size `m`
 *
 * Symbols are defined to be non-negative integers less than `m`.
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
 * `from` must be a valid node index,
 * and `sym` must be a non-negative integer less than the alphabet size,
 * or the behavior is undefined.
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
 * `from` must be a valid node index,
 * and `sym` must be a non-negative integer less than the alphabet size,
 * and the child node must be a leaf if it exists,
 * or the behavior is undefined.
 */
void chtrie_del(chtrie *tr, int from, int sym);

/* free the trie */
void chtrie_free(chtrie *tr);

#endif
