#ifndef CHTRIE
#define CHTRIE

struct chtrie_edge {
	struct chtrie_edge *next;
	int from, sym, to;
};

typedef struct {
	struct chtrie_edge **etab;
	int *idxpool, *idxptr, idxmax;	/* pool of available node indexes */
	int maxn, alphsz, ecap;
} chtrie;

/* Allocate a trie with at most `n` nodes, and the alphabet size `m`.
 *
 * If `n` or `m` is less than 1, they will be regulated to 1.
 *
 * Nodes in the trie are indexed by non-negative integers less than `n`.
 * The root node is indexed by 0.
 * Symbols are non-negative integers less than `m`.
 *
 * Upon success, return a pointer to the trie.
 * Otherwise, return `NULL` and set `errno`.
 */
chtrie *chtrie_alloc(size_t n, size_t m);

/* Walk from one node to its child.
 *
 * If the child didn't exist and `creat` is non-zero,
 * a new node will be created.
 *
 * Upon the child is found or created, return the index of the child.
 * Otherwise, return -1.
 * If `creat` is non-zero and this routine fails, `errno` will be set.
 */
int chtrie_walk(chtrie *tr, int from, int sym, int creat);

/* Delete a child node.
 *
 * If the child doesn't exist, the trie shall be left unchanged.
 */
void chtrie_del(chtrie *tr, int from, int sym);

/* free the trie */
void chtrie_free(chtrie *tr);

#endif
