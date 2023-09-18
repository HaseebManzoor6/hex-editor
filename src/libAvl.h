#ifndef LIBAVL_H
#define LIBAVL_H

#include "types.h"

/**
 * --- libAvl.h ---
 *  Simple AVL trees implementation
 *  Each Node has a KEYTYPE key and VALTYPE value. All searches of
 *   the tree are indexed by the key.
 *  Duplicate keys are not supported and may break these methods.
 */



// == TYPES ==

typedef addr_t KEYTYPE;
typedef char VALTYPE;

struct Node {
    KEYTYPE key;
    VALTYPE val;
    long height;
    struct Node *l, *r;
};

struct AVL {
    struct Node *root;
};



// == FUNCTIONS ==

// avlinit sets t->root to be NULL
void avlinit(struct AVL *t);
// Frees all nodes of the tree
void avlfree(struct AVL *t);

/**
 * avlnext returns the Successor of node n, or the
 *  Node with minimum key greater than/equal to n->key
 * avlnextk assumes there is a node with key k in tree t,
 *  and returns the Successor of that node
 *
 * These functions return NULL if n (or k) is not a node in
 *  t, and n (or the node with key k) itself if it has
 *  the greatest key in the tree
 */
struct Node *avlnext (struct AVL *t, struct Node *n);
struct Node *avlnextk(struct AVL *t, KEYTYPE k);

/**
 * Predecessor
 * Analogous to avlnext functions, but for maximum
 *  key less than/equal to n->key (or k)
 */
struct Node *avlpre (struct AVL *t, struct Node *n);
struct Node *avlprek(struct AVL *t, KEYTYPE k);

/**
 * Insert and Delete
 */
void avlinsert(struct AVL *t, KEYTYPE k, VALTYPE v);
void avldel (struct AVL *t, struct Node *n);
void avldelk(struct AVL *t, KEYTYPE k);
// TODO avlinsert(struct AVL *t, struct Node *n); ??
struct Node *avlfind(struct AVL *t, KEYTYPE k);

#endif
