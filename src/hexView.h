#ifndef HEXVIEW_H
#define HEXVIEW_H

#include "libChunks.h"

#define LINELENGTH 16
// TODO link the stackOverflow where i got this awesome macro
#define CEILDIVIDE(x,y) x/y + (x%y != 0)

// Lots of macros for hexview-related calculations
#define CURSORLINE(h) h->cury+h->startLine
#define NLINES(h) CEILDIVIDE(h.cs.fsize,LINELENGTH)
// Byte index in the file
#define FILEPOS(h) LINELENGTH*(h->startLine+h->cury)+(h->curx+1)/2

// Size of buffer for commands
#define CMD_BUFSIZE 128

struct Settings {
    unsigned int textLines;
    int termColors;
};

struct HexView {
    struct ChunkSet cs;
    struct Settings settings;
    
    
    long addrsize,  // Number of hex digits for addresses
         startLine; // Top line of file to show
    int curx, cury;

    char *cmdbuf;
    int cmdLen;
    int quit;
};

void initHexView(struct HexView *p, FILE *fp, unsigned int textLines, int hasColors);
void delHexView(struct HexView *p);

//#define canScrollUp(h) h.startLine > 0
//#define canScrollDown(h) h.startLine+h.settings.textLines < CEILDIVIDE(h.cs.fsize,LINELENGTH)

void render(struct HexView *p);

#endif
