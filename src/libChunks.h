#ifndef LIBCHUNKS_H
#define LIBCHUNKS_H

#include <stdio.h>







struct ChunkSet {
    FILE *fp;
    struct FileChunk *low, *high;
    unsigned int chunkSize;
    unsigned long start; // Where low[0] is in the file
    long fsize;
    char eof; // 1 iff last byte read was EOF
};

void initChunkSet(struct ChunkSet *p, FILE *fp, unsigned int chunkSize);
void delChunkSet(struct ChunkSet *p);

void scrollDownChunks(struct ChunkSet *p,int x);

char chunksGet(struct ChunkSet *p, unsigned long i);

//char popChunksDown(struct ChunkSet *p);

#define chunkSetSize(p) p->low->size + p->high->size


#endif
