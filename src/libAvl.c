#include <stdlib.h>
#include <stdio.h>

#include "libAvl.h"


#define MAX(x,y) ((x>y)?x:y)
#define HEIGHT(n) ((n==NULL)? 0:n->height)
#define BALANCEFACTOR(n) (HEIGHT(n->r)-HEIGHT(n->l))
#define UPDATEHEIGHT(n) n->height = 1 + MAX( HEIGHT(n->r),HEIGHT(n->l) )

// where n a struct Node*
#define BSTRotate(root,from,to,n) {\
    if(root!=NULL) {             \
        n = root->from;          \
        if(n != NULL) {          \
            root->from = n->to;  \
            n->to = root;        \
            UPDATEHEIGHT(n);     \
        }                        \
        UPDATEHEIGHT(root);      \
        root = n;                \
    }                            \
}
#define REBALANCE(p,n) {                         \
    if(BALANCEFACTOR(p) > 1) {                   \
        if(p->r && (BALANCEFACTOR(p->r) <= -1))  \
            BSTRotate(p->r,l,r,n);               \
        BSTRotate(p,r,l,n);                      \
    }                                            \
    else if(BALANCEFACTOR(p) < -1) {             \
        if(p->l && (BALANCEFACTOR(p->l) >= 1))   \
            BSTRotate(p->l,r,l,n);               \
        BSTRotate(p,l,r,n);                      \
    }                                            \
}




void avlinit(struct AVL *t) {
    t->root=NULL;
}

/**
 * RECURSIVE
 * Free node root and all it's children
 */
void freeSubtree(struct Node *root) {
    if(root!=NULL) {
        freeSubtree(root->l);
        freeSubtree(root->r);
        free(root);
    }
}
void avlfree(struct AVL *t) {
    freeSubtree(t->root);
    // free(t); // No other data
}




/**
 * RECURSIVE
 * Insert into (sub)tree and track height
 */
void subtreeinsert(struct Node *p, struct Node *n) {
    if(n->key>p->key) {
        if(p->r==NULL) p->r=n;
        else subtreeinsert(p->r,n);
    }
    else {
        if(p->l==NULL) p->l=n;
        else subtreeinsert(p->l,n);
    }
    UPDATEHEIGHT(p);
}
void avlinsert(struct AVL *t, KEYTYPE k, VALTYPE v) {
    // make new node
    struct Node *n = malloc(sizeof(struct Node)); \
    if(n==NULL) {perror("malloc"); exit(127);}
    n->l=NULL; n->r=NULL;
    n->height=1;
    n->key=k; n->val=v;

    // Find place in tree
    if(t->root==NULL) {t->root=n; return;}
    subtreeinsert(t->root,n);

    // Rebalance
    REBALANCE(t->root,n);
}

/**
 * RECURSIVE
 * PRECONDITION: p has 2 children
 *
 * Replace p's key and value with those of it's successor
 *
 * RETURN: a pointer to the successor node, which is no longer
 * connected to the tree, and should be free()d at some point
 */
struct Node *replacewithsuccessor(struct Node *origin, struct Node *parent, struct Node *p) {
    // Find successor and it's parent
    if(p->l) {
        #define ret parent
        ret=replacewithsuccessor(origin, p,p->l);
        UPDATEHEIGHT(p);
        REBALANCE(p,origin);
        return ret;
    }

    // replace
    origin->key=p->key; origin->val=p->val;
    // delete successor
    if(p==parent->r) parent->r=p->r;
    else             parent->l=p->r;
    p->r=NULL;

    UPDATEHEIGHT(p);
    REBALANCE(p,origin);

    return p;
}

/**
 * RECURSIVE
 * Delete and update heights
 */
void subtreedel(struct AVL *t, struct Node *parent, struct Node *p, KEYTYPE k) {

    if(!p) return;

    if(p->key != k) {
        subtreedel(t, p, (k>p->key)? p->r : p->l,k);
        UPDATEHEIGHT(p);
        REBALANCE(p,parent);
        return;
    }
    // 2 child
    if(p->l && p->r) {
        p=replacewithsuccessor(p,p,p->r);
    }
    // 1 child
    else if(p->l || p->r) {
        if(!parent)             t->root=(p->l)? p->l:p->r;
        else if(parent->l==p) parent->l=(p->l)? p->l:p->r;
        else                  parent->r=(p->l)? p->l:p->r;
    }
    // no child
    else {
        if(!parent)           t->root=NULL;
        else if(parent->l==p) parent->l=NULL;
        else                  parent->r=NULL;
    }
    free(p);
}
void avldelk(struct AVL *t, KEYTYPE k) {
    if(t->root) subtreedel(t, NULL, t->root, k);
}

struct Node *avlfind(struct AVL *t, KEYTYPE k) {
    struct Node *p = t->root;
    while(1) {
        if(p==NULL) return p;
        else if(k>p->key) p=p->r;
        else if(k<p->key) p=p->l;
        else return p;
    }
}

struct Node *avlnext (struct AVL *t, struct Node *n) {
    struct Node *p, *parent;
    // If exists, return the minimum of the right subtree
    if(n->r) {
        p=n->r;
        while(p->l) p=p->l;
        return p;
    }
    // Else find lowest parent for which n is in the left subtree
    p=t->root;
    parent=t->root;
    while(p!=n) {
        if(p==NULL) return p;
        else if(n->key>p->key) p=p->r;
        else if(n->key<p->key) {parent=p; p=p->l;}
    }
    // If no such parent, n must be tree maximum
    // TODO better way to check this
    if(parent==t->root && n->key>parent->key) return n;
    // Else return successor
    return parent;
}
struct Node *avlnextk(struct AVL *t, KEYTYPE k) {/*
    struct Node *p, *parent;
    p=t->root;
    parent=t->root;
    while(p->key!=k) {
        if(p==NULL) return p;
        else if(k>p->key) p=p->r;
        else if(k<p->key) {parent=p; p=p->l;}
    }
    // Check if right subtree exists
    if(p->r) {
        p=p->r;
        while(p->l) p=p->l;
        return p;
    }
    // Maximum
    // TODO better way to check this
    if(parent==t->root && k>parent->key) return p;
    // Else return parent
    return parent;
*/}


struct Node *avlpre (struct AVL *t, struct Node *n) {
    struct Node *p, *parent;
    // If exists, return the maximum of the left subtree
    if(n->l) {
        p=n->l;
        while(p->r) p=p->r;
        return p;
    }
    // Else find greatest parent for which n is in the right subtree
    p=t->root;
    parent=t->root;
    while(p!=n) {
        if(p==NULL) return p;
        else if(n->key>p->key) {parent=p; p=p->r;}
        else if(n->key<p->key) p=p->l;
    }
    // If no such parent, n must be tree minimum
    // TODO better way to check this
    if(parent==t->root && n->key<parent->key) return n;
    // Else return successor
    return parent;
}
struct Node *avlprek(struct AVL *t, KEYTYPE k) {}
