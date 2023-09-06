#include CURSES_LIB

#include <math.h>
#include <stdlib.h>

#include "hexView.h"
#include "libChunks.h"

#define HEX "0123456789ABCDEF"
#define BYTETOHEX(x) HEX[x & 0xF]


void initHexView(struct HexView *p, FILE *fp, unsigned int textLines, int hasColors) {
    p->startLine=0;
    p->settings.textLines = textLines;
    initChunkSet(&(p->cs), fp, textLines * LINELENGTH);
    p->addrsize = (logl((double)(p->cs.fsize))/logl(16.0)); // Number of hex digits for addresses
    p->settings.termColors = hasColors;

    p->cmdbuf = (char*)malloc(CMD_BUFSIZE*sizeof(char));
    p->curx=0; p->cury=0;
    p->quit = 0;
}

void delHexView(struct HexView *p) {
    delChunkSet(&(p->cs));
    free(p->cmdbuf);
}

#define DUMP(x) \
    if (x <= 0x20 || x==0x7F || x==0xFF) addch('.'); \
    else addch(x);

void render(struct HexView *hv) {
    unsigned int nLines = hv->settings.textLines;
    unsigned long startLine = hv->startLine;
    char c='x';

    hv->cs.eof = 0;

    // Print header
    for(int i=0; i<hv->addrsize; i++) addch(' '); // print [addrsize] spaces
    printw("  0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F | Dump\n");

    // Print file
    for(int y=0; y<nLines && !(hv->cs.eof); y++) {
        // Print address
        for(int i=hv->addrsize-1; i>=0; i--)
            addch(BYTETOHEX((startLine+y) >> (i*4)));
        printw("                                                 | ");

        for(int x=0; x<LINELENGTH && !(hv->cs.eof); x++) {
            c = chunksGet(&(hv->cs),(y+hv->startLine)*LINELENGTH+x);
            if(hv->cs.eof && hv->settings.termColors) attron(COLOR_PAIR(2));
            // Print hex
            move(y+1, x*3+(hv->addrsize)+1);
            addch(BYTETOHEX(c>>4)); addch(BYTETOHEX(c));
            // Print dump
            move(y+1, LINELENGTH*3+(hv->addrsize)+x+3);
            DUMP(c)

            if(hv->settings.termColors) attron(COLOR_PAIR(1));
        }

        addch('\n');
    }

    // command error msg line
    clrtoeol();
}
