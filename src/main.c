#include CURSES_LIB

#include <stdio.h>
#include <stdlib.h>

#include "libChunks.h"
#include "libCmd.h"
#include "hexView.h"



#define MODE_NORMAL 0
#define MODE_INSERT 1
#define MODE_REPLACE 2
struct HexViewMeta {
    CmdErr::Type err;
    char mode;
};



#define MIN(x,y) (y<x)?y:x
#define ESC 27
void handleKey(int c, struct HexView *h, struct HexViewMeta *m) {
#define LAST_X ((CURSORLINE(h)==NLINES(*h)-1)?  1+2*((h->cs.fsize-1)%LINELENGTH) : LINELENGTH*2 - 1)
    switch(c) {
        case KEY_LEFT:
        case KEY_BACKSPACE:
            if(h->curx==0 && h->cury+h->startLine>0) {handleKey('k',h,m); h->curx=LINELENGTH*2;}
            // Then case 'h' runs
        case 'h':
            if(h->curx>0) h->curx--;
            break;
        case KEY_RIGHT:
        case ' ':
            if(h->curx==LINELENGTH*2-1 && CURSORLINE(h)<NLINES((*h))-1) {
                handleKey('j',h,m);
                h->curx=-1;
            }
        case 'l':
            // If not at rightmost byte and not at end of file, then move cursor left
            if(h->curx<LINELENGTH*2-1 && FILEPOS(h)<h->cs.fsize) h->curx++;
            break;
        case KEY_UP:
        case 'k':
            if(h->cury>0) h->cury--;
            else if(h->startLine > 0) {
                h->startLine--;
            }
            break;
        case KEY_DOWN:
        case 'j':
            if(CURSORLINE(h) < NLINES((*h))-1) {
                if(h->cury<h->settings.textLines-1) h->cury++;
                else h->startLine++;
            }
            if(CURSORLINE(h)==NLINES((*h))-1) h->curx=MIN(h->curx,1+2*((h->cs.fsize-1)%LINELENGTH));
            break;

        case 'H':
            h->cury = 0;
            break;
        case 'L':
            h->cury = MIN(h->settings.textLines-1,NLINES((*h))-1);
            h->curx = MIN(h->curx,LAST_X);
            break;
        case '^':
        case '0':
            h->curx=0;
            break;
        case '$':
            h->curx = LAST_X;
            break;
        case KEY_IC:
        case 'i':
            m->mode = MODE_INSERT;
            break;
        // Commands
        case ':':
            move(h->settings.textLines+2, 0);
            addch(c);

            if(m->err = acceptCmd(h))
                return;

            if(m->err = parseCmd(h))
                return;

            break;
    }
}

void handleKeyInsert(int c, struct HexView *h, struct HexViewMeta *m) {
    switch(c) {
        case KEY_LEFT:
        case KEY_RIGHT:
        case KEY_UP:
        case KEY_DOWN:
            handleKey(c,h,m);
            break;
        case ESC:
            m->mode=MODE_NORMAL;
            break;
        case KEY_IC:
            m->mode=MODE_REPLACE;
            break;
    }
}

void handleKeyReplace(int c, struct HexView *h, struct HexViewMeta *m) {
    switch(c) {
        case KEY_LEFT:
        case KEY_RIGHT:
        case KEY_UP:
        case KEY_DOWN:
            handleKey(c,h,m);
            break;
        case ESC:
            m->mode=MODE_NORMAL;
            break;
        case KEY_IC:
            m->mode=MODE_INSERT;
            break;
    }
}

void renderMode(HexViewMeta *m) {
    if(m->mode==MODE_INSERT) {
        attron(COLOR_PAIR(4));
        printw("\n-- INSERT --");
    }
    else if(m->mode==MODE_REPLACE) {
        attron(COLOR_PAIR(4));
        printw("\n-- REPLACE --");
    }
    else{clrtoeol();}
}

#define BADUSAGE(x) fprintf(stderr, "Usage: %s filename\n",argv[0]); exit(x)
int main(int argc, char *argv[]) {
    if(argc != 2)
        {BADUSAGE(123);}

    FILE* fp;
    if((fp = fopen(argv[1],"rb")) == NULL) {
        fprintf(stderr,"Couldn't open file %s\n",argv[1]);
        exit(1);
    }

    unsigned int width, height;
    struct HexView h;
    char in, *cmd; // user input
    struct HexViewMeta m;

    cmd = (char*)malloc(CMD_BUFSIZE*sizeof(char));
    m.err = CmdErr::Ok;


    // nCurses setup
    initscr();
    noecho(); // Don't show keys user types
    keypad(stdscr,TRUE); // Enable arrow/function/other keys
    getmaxyx(stdscr, height, width); // Get screen bounds
    ESCDELAY = 10; // 10ms delay for escape codes; allows handling ESC presses

    initHexView(&h, fp, height-3, (has_colors() == TRUE));

    // Init colors
    if(h.settings.termColors) {
        start_color();
        use_default_colors();
        init_pair(1, -1, -1); // term colors
        init_pair(2, COLOR_RED, -1);
        init_pair(3, COLOR_RED, -1);
        init_pair(4, COLOR_YELLOW, -1);
    }

    // Main loop
    while(!h.quit) {
        // Render
        erase();
        render(&h);
        renderMode(&m);

        // error message line
        if(h.settings.termColors) {
            attron(COLOR_PAIR(3)); attron(A_REVERSE); attron(A_BOLD);}
        CmdErr::printmsg(m.err);
        if(h.settings.termColors) {
            attron(COLOR_PAIR(1)); attroff(A_REVERSE); attroff(A_BOLD);}
        m.err = CmdErr::Ok;

        move(1+h.cury, 1+h.addrsize+3*(h.curx/2)+(h.curx%2));
        refresh();

        // Controls
        switch(m.mode) {
            case MODE_NORMAL:
                handleKey(getch(), &h, &m);
                break;
            case MODE_INSERT:
                handleKeyInsert(getch(), &h, &m);
                break;
            case MODE_REPLACE:
                handleKeyReplace(getch(), &h, &m);
                break;
        }
    }

    endwin();
    delHexView(&h);
    free(cmd);

    return 0;
}

