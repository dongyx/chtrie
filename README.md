CHTrie
======

*CHTrie* is the official C library of the *coordinate hash trie*.
See [the *Coordinate Hash Trie*](#coordinate-hash-trie) section.

CHTrie is designed for arbitrary symbols, not just characters.
Instead of providing high-level string set/dictionary interfaces,
CHTrie provides low-level interfaces making you implement your own trie-based data structures.
See [the *Usage* section](#usage).


**Table of Contents**

* [Coordinate Hash Trie](#coordinate-hash-trie)
* [Installation](#installation)
* [Usage](#usage)
* [Example: Implementing a String Set](#example-implementing-a-string-set)
* [Following News](#following-news)
* [Related Projects](#related-projects)

Coordinate Hash Trie
--------------------

The coordinate hash trie is a trie variant balancing between time, space, and simplicity.

The basic idea is very simple.
We use a global hash table to store all edges in a Trie.
Each edge is stored as a dictionary item `(from, symb)->to`.

We use a special hash function:

	h(from, symb) = (from*m + symb) mod H

, where `m` is the size of the alphabet, and `H` is the number of slots in the hash table. For a trie with `n` nodes, we take `H = (n-1)/alpha` where `alpha` is the constant load factor. **No rehashing, resizing, or reallocation is required.**

We could prove that **the time complexity of transition from one node to another, is O(1) for the average case and O(m) for the worst case**. **The space complexity is O(n), unrelated to** `m`.

See [my paper](https://arxiv.org/abs/2302.03690) for the analysis and proof.

Compared to other compressed trie variants like *double array trie*, or using `n` hash tables each for a node, this variant provides **stable space consumption** and is very **easy to implement**.

Installation
------------

CHTrie requires a C89 compiler and POSIX `errno` definitions.

To install CHTrie as header files and a static library into the system:

	make
	sudo make install

By default, CHTrie is installed to `/usr/local`.
To link CHTrie, use the `-l chtrie` option on your compiler.

It's also simple to embed CHTrie into your project.
Just include `chtrie.h`.
Compile and link `chtrie.c`.

Usage
-----

CHTrie provides low-level interfaces.
It's not designed in the concepts of strings and trees, but symbols and nodes.

Each node is indexed by a non-negative integer less than the maximum number of nodes specified by the `chtrie_alloc()` call.
The root node is indexed by 0.

Each symbol is a non-negative integer less than the alphabet size specified by the `chtrie_alloc()` call.

You could use the `chtrie_walk()` function to walk from one node to its child.
The `chtrie_walk()` function also provides an argument that determines whether to create a new node if the child didn't exist.

Because nodes are indexed by non-negative integers,
it's easy to associate additional information with nodes, by using external data structures.
For example, we could define an array `nchild[]` where `nchild[i]` stores the number of children of node `i`,
or you could maintain whether a node is a termination node, which node is its parent node, and so on.

The low-level nature brings both flexibility and simplicity to CHTrie.
There are only one public type and four functions of CHTrie.

- `typedef ... chtrie`

	A `chtrie` instance represents a trie.


- `chtrie *chtrie_alloc(int n, int m)`

	Allocate a trie with at most `n` nodes, and the alphabet size `m`.

	If `n` or `m` is less than 1, they will be regulated to 1.

	Nodes in the trie are indexed by non-negative integers less than `n`.
	The root node is indexed by 0.
	Symbols are non-negative integers less than `m`.

	Upon success, return a pointer to the trie.
	Otherwise, return `NULL` and set `errno`.

- `int chtrie_walk(chtrie *tr, int from, int sym, int creat)`

	Walk from one node to its child.

	If the child didn't exist and `creat` is non-zero,
	a new node will be created.
	
	`from` must be a valid node index,
	and `sym` must be a non-negative integer less than the alphabet size,
	or the behavior is undefined.
	
	Upon success, return the index of the child
	which is a non-negative integer less than the maximum number of nodes
	specified by the `chtrie_alloc()` call.

	Otherwise, return -1.
	
	If `creat` is non-zero and this routine fails, `errno` will be set.
	
- `void chtrie_del(chtrie *tr, int from, int sym)`

	Delete a child node.

	The child node must be a leaf if it exists,
	or the behavior is undefined.
	
	If the child doesn't exist, the trie shall be left unchanged.
	
	`from` must be a valid node index,
	and `sym` must be a non-negative integer less than the alphabet size,
	or the behavior is undefined.
	

- `void chtrie_free(chtrie *tr)`

	Free the trie.

Example: Implementing a String Set
-------------------------------

The following program builds a trie with words from the `dict` array excluding those that are also in the `stop` array.

After the trie is built, it reads lines from the standard input.
For each line, the program prints `1` if the line is in the trie.
Otherwise, the program prints `0`.

This example illustrates the common usage of CHTrie, e.g. how to associate additional information with a node.

	#include <string.h>
	#include <stdlib.h>
	#include <stdio.h>
	#include "chtrie.h"
	#define fatal(s) do { perror(s); exit(-1); } while (0)
	#define N 65536
	#define M 256

	static char *dict[] = {
		"", "he", "the", "she", "his", "hers", "a", "an", "this", "that"
	};
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
		for (i = 0; i < sizeof dict / sizeof dict[0]; i++)
			add(dict[i]);
		for (i = 0; i < sizeof stop / sizeof stop[0]; i++)
			del(stop[i]);

		while (fgets(line, sizeof line, stdin)) {
			line[strcspn(line, "\n")] = '\0';
			printf("%d\n", query(line) ? 1 : 0);
		}
		chtrie_free(tr);
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

Following News
--------------

- <https://www.dyx.name>
- [Twitter](https://twitter.com/dongyx2)

Related Projects
----------------

- [LibACA](https://github.com/dongyx/libaca): A C Library of Aho-Corasick Algorithm Based on the Coordinate Hash Trie
