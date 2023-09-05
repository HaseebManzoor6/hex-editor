#include CURSES_LIB

#include <stdio.h>
#include <stdlib.h>

#include "libChunks.h"
#include "libCmd.h"
#include "hexView.h"



#define MIN(x,y) (y<x)?y:x
void handleKey(int c, struct HexView *h, CmdErr::Type *err) {
    switch(c) {
        case KEY_BACKSPACE:
            if(h->curx==0 && h->cury+h->startLine>0) {handleKey('k',h,err); h->curx=LINELENGTH*2;}
            // Then case 'h' runs
        case 'h':
            if(h->curx>0) h->curx--;
            break;
        case ' ':
            if(h->curx==LINELENGTH*2-1 && CURSORLINE(h)<NLINES((*h))-1) {
                handleKey('j',h,err);
                h->curx=-1;
            }
        case 'l':
            // If not at rightmost byte and not at end of file, then move cursor left
            if(h->curx<LINELENGTH*2-1 && FILEPOS(h)<h->cs.fsize) h->curx++;
            break;
        case 'k':
            if(h->cury>0) h->cury--;
            else if(h->startLine > 0) {
                h->startLine--;
            }
            break;
        case 'j':
            if(CURSORLINE(h) < NLINES((*h))-1) {
                if(h->cury<h->settings.textLines-1) h->cury++;
                else h->startLine++;
            }
            if(CURSORLINE(h)==NLINES((*h))-1) h->curx=MIN(h->curx,1+2*((h->cs.fsize-1)%LINELENGTH));
            break;
        case 'H':
            h->curx = h->cury = 0;
            break;
        case 'L':
            h->cury = MIN(h->settings.textLines-1,NLINES((*h))-1);
            h->curx=0;
            break;
        // Commands
        case ':':
            move(h->settings.textLines+2, 0);
            addch(c);

            if(*err = acceptCmd(h))
                return;

            if(*err = parseCmd(h))
                return;

            break;
    }
}

#define BADUSAGE(x) fprintf(stderr, "Usage: %s filename\n",argv[0]); exit(x)
int main(int argc, char *argv[]) {
    if(argc != 2)
        {BADUSAGE(123);}

    FILE* fp;
    if((fp = fopen(argv[1],"r")) == NULL) {
        fprintf(stderr,"Couldn't open file %s\n",argv[1]);
        exit(1);
    }

    unsigned int width, height;
    struct HexView h;
    char in, *cmd; // user input
    CmdErr::Type cmderr;

    cmd = (char*)malloc(CMD_BUFSIZE*sizeof(char));
    cmderr = CmdErr::Ok;


    // nCurses setup
    initscr();
    noecho(); // Don't show keys user types
    keypad(stdscr,TRUE); // Enable arrow/function/other keys
    getmaxyx(stdscr, height, width); // Get screen bounds

    initHexView(&h, fp, height-3, (has_colors() == TRUE));

    // Init colors
    if(h.settings.termColors) {
        start_color();
        use_default_colors();
        init_pair(1, -1, -1); // term colors
        init_pair(2, COLOR_RED, -1);
        init_pair(3, COLOR_RED, -1);
    }

    // Main loop
    while(!h.quit) {
        // Render
        erase();
        render(&h);

        // error message line
        if(h.settings.termColors) {
            attron(COLOR_PAIR(3)); attron(A_REVERSE); attron(A_BOLD);}
        CmdErr::printmsg(cmderr);
        if(h.settings.termColors) {
            attron(COLOR_PAIR(1)); attroff(A_REVERSE); attroff(A_BOLD);}
        cmderr = CmdErr::Ok;

        move(1+h.cury, 1+h.addrsize+3*(h.curx/2)+(h.curx%2));
        refresh();

        // Controls
        handleKey(getch(), &h, &cmderr);
    }

    endwin();
    delHexView(&h);
    free(cmd);

    return 0;
}

