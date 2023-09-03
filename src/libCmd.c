#include CURSES_LIB

#include <ctype.h>
#include <string.h>

#include "libCmd.h"
#include "hexView.h"

#define PASS 1
#define FAIL 0

int acceptCmd(struct HexView *h) {
    int i=0;
    char c, *buf = h->cmdbuf;
    nocbreak();
    echo();
    while(i<CMD_BUFSIZE-1) {
        c = getch();
        buf[i] = c;
        if(c == '\n' || c == '\r') break;
        i++;
    }
    noecho();
    cbreak();
    h->cmdLen=i;
    buf[i] = '\0';

    return PASS;
}

// HexToDec solution from: https://stackoverflow.com/a/11068850
static const long hextable[] = { 
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1, 0,1,2,3,4,5,6,7,8,9,-1,-1,-1,-1,-1,-1,-1,10,11,12,13,14,15,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,10,11,12,13,14,15,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1
};

unsigned long hexToDec(char *buf, int size) {
    unsigned long ret=0;
    for(int i=0; i<size; i++) {
        ret = (ret << 4) | hextable[buf[i]];
    }
    return ret;
}

// TODO response messages
#define MIN(x,y) x=(y<x)? y:x
int parseCmd(struct HexView *h) {
    char *buf = h->cmdbuf;
    int isAddr=1; // Is hex line number

    // Check if command is a hex line number
    for(int i=0; i<h->cmdLen; i++) {
        if(buf[i] == '\0') break;
        if(!isxdigit(buf[i])) {isAddr=0;}
    }
    if(h->cmdLen==0) isAddr=0;

    // Jump to line
    // TODO Always jump to 1 screen above, as vim does
    if(isAddr) {h->startLine = hexToDec(buf,h->cmdLen);}
    // Jump to end shortcut
    else if(0==strcmp(buf,"$")) {
        isAddr=1; h->startLine = h->cs.fsize;
    }
    if(isAddr) {
        h->curx=0;
        h->cury=MIN(h->startLine,NLINES((*h))-1);
        h->startLine = MIN(h->startLine, CEILDIVIDE(h->cs.fsize,LINELENGTH) - h->settings.textLines);
        h->cury-=h->startLine;
        if(h->startLine<0) h->startLine=0;
    }
    // Quit
    else if(!strcmp(buf,"q")) {
        h->quit=1;
    }

    return FAIL;
}
