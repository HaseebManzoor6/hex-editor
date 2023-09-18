#ifndef LIBCHUNKS_H
#define LIBCHUNKS_H

#include <stdio.h>

#include "types.h"


struct FileChunk {
    char *chunk;
    unsigned int size;
};


struct ChunkSet {
    FILE *fp;
    struct FileChunk *low, *high;
    unsigned int chunkSize;
    addr_t start; // Where low[0] is in the file
    addr_t fsize;
    char eof; // 1 iff last byte read was EOF
};

void initChunkSet(struct ChunkSet *p, FILE *fp, unsigned int chunkSize);
void delChunkSet(struct ChunkSet *p);

void scrollDownChunks(struct ChunkSet *p,int x);

char chunksGet(struct ChunkSet *p, addr_t i);

#define chunkSetSize(p) p->low->size + p->high->size


#endif
