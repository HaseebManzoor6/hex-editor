//#define CURSES_LIB "../lib/pdcurses/curses.h"
#include CURSES_LIB
#include <string.h>
#include <stdlib.h>

#include "libChunks.h"

#define CHUNKS_ACCESS_ERR endwin(); fprintf(stderr,"Chunks access error\n");

#define MEM_ERR fprintf(stderr,"malloc error"); exit(66);

struct FileChunk {
    char *chunk;
    unsigned int size;
};

void initFileChunk(struct FileChunk *p, unsigned int capacity) {
    p->chunk = malloc(capacity*sizeof(char));
    p->size=0;
}

// TODO remove or make this a macro
void delFileChunk(struct FileChunk *p) {
    free(p->chunk);
}

// TODO remove or make this a macro
void fetchFileChunk(struct FileChunk *p, FILE *fp, unsigned int nmemb) {
    p->size = fread(p->chunk, sizeof(char), nmemb, fp);
}






void initChunkSet(struct ChunkSet *p, FILE *fp, unsigned int chunkSize) {

    p->fp = fp;

    if(NULL == (p->low  = malloc(sizeof(struct FileChunk)))) {MEM_ERR}
    if(NULL == (p->high = malloc(sizeof(struct FileChunk)))) {MEM_ERR}

    initFileChunk(p->low, chunkSize);
    initFileChunk(p->high, chunkSize);

    fetchFileChunk(p->low, fp, chunkSize);
    fetchFileChunk(p->high, fp, chunkSize);

    p->start=0;
    p->chunkSize = chunkSize;
    
    // Get file size
    fseek(fp, 0L, SEEK_END);
    p->fsize = ftell(fp);
    rewind(fp);
}

void delChunkSet(struct ChunkSet *p) {
    delFileChunk(p->low);
    delFileChunk(p->high);
    free(p->low);
    free(p->high);
}


char chunksGet(struct ChunkSet *p, unsigned long i) {

    p->eof = (i == p->fsize-1);

    if(i < 0 || i > p->fsize) {CHUNKS_ACCESS_ERR exit(1);}

    if(i < p->start) {
        // shift up
        p->start = i < p->chunkSize ? 0 : i-(p->chunkSize);

        if(i == p->start-1) {
            fseek(p->fp, -1*(p->chunkSize), SEEK_CUR);
            memcpy(p->high->chunk, p->low->chunk, p->low->size);
            p->high->size = p->low->size;
            i -= p->chunkSize; 
        }
        else {
            fseek(p->fp, p->start, SEEK_SET);
            fetchFileChunk(p->low, p->fp, p->chunkSize);
        }
        fetchFileChunk(p->high, p->fp, p->chunkSize);

    }
    else if(i >= p->start+chunkSetSize(p)) {
        // shift down
        p->start = i < p->chunkSize ? 0 : i-(p->chunkSize);

        // For just scrolling down, use memcpy to swap high/low chunk
        if(i == p->start+chunkSetSize(p)) {
            fseek(p->fp, p->chunkSize, SEEK_CUR);
            memcpy(p->low->chunk, p->high->chunk, p->high->size);
            p->low->size = p->high->size;
            i -= p->chunkSize;
        }
        // For jumping, get low chunk from the file
        else {
            fseek(p->fp, p->start, SEEK_SET);
            fetchFileChunk(p->low, p->fp, p->chunkSize);
        }
        fetchFileChunk(p->high, p->fp, p->chunkSize);

    }

    i -= p->start;
    if(i < p->chunkSize)
        return p->low->chunk[i];
    else 
        return p->high->chunk[i - p->chunkSize];
}

