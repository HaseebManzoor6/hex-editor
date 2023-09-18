#ifndef REPLTREE_H
#define REPLTREE_H

#include "types.h"
#include "libAvl.h"

// Tree for bytes replaced in the file
typedef struct {
    struct AVL *tree;
    struct Node *prev, *next;
} repltree;

#endif
