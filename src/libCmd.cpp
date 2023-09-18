#include CURSES_LIB

#include <ctype.h>
#include <string.h>

#include "libCmd.h"
#include "hexView.h"

const std::string CmdErr::getmsg(CmdErr::Type t) {
    switch(t) {
        case CmdErr::Ok:
            return "\n";

        case CmdErr::UNKNOWN:
            return "Unknown command\n";

        default:
            return "[Internal] Unknown error code\n";
    }
}

CmdErr::Type acceptCmd(struct HexView *h) {
    int i=0, c;
    char *buf = h->cmdbuf;
    clrtoeol();
    echo();
    while(i<CMD_BUFSIZE-1) {
        c = getch();
        buf[i] = c;
        if(c == '\n' || c == '\r') break;
        if(c == 27) {
            i=0; break;
        }
        i++;
    }
    noecho();
    h->cmdLen=i;
    buf[i] = '\0';

    return CmdErr::Ok;
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

unsigned long hexToDec(const char *buf, int size) {
    unsigned long ret=0;

    for(int i=0; i<size; i++) {
        ret = (ret << 4) | hextable[buf[i]];
    }
    return ret;
}

#define MIN(x,y) (((y)<(x))? (y):(x))
#define MAX(x,y) (((y)<(x))? (x):(y))
CmdErr::Type parseCmd(struct HexView *h) {
    const char *buf = h->cmdbuf;
    int isAddr=1; // Is hex line number

    // Check if command is a hex line number
    for(int i=0; i<h->cmdLen; i++) {
        if(buf[i] == '\0') break;
        if(!isxdigit(buf[i])) {isAddr=0;}
    }
    if(h->cmdLen==0) isAddr=0;

    // TODO Always jump to 1 screen above, as vim does
    if(isAddr) 
        h->startLine = hexToDec(buf,h->cmdLen);
    // Jump to end shortcut
    else if(0==strcmp(buf,"$")) {
        isAddr=1; h->startLine = h->cs.fsize;
    }

    // Jump to line
    if(isAddr) {
        // TODO fix cursor can go off the end of last line using :$
        h->cury=h->startLine/LINELENGTH;
        h->curx = 2*(h->startLine % LINELENGTH);
        h->startLine = MIN(h->startLine, h->cs.fsize - (LINELENGTH*(h->settings.textLines-1)));
        h->startLine/=LINELENGTH;
        h->cury-=h->startLine;

        return CmdErr::Ok;
    }

    // Quit
    else if(!strcmp(buf,"q")) {
        h->quit=1;

        return CmdErr::Ok;
    }
    // no command
    else if(!strcmp(buf,"")) return CmdErr::Ok;

    return CmdErr::UNKNOWN;
}
