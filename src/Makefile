CC = gcc

ifeq ($(OS),Windows_NT)
	CURSES_LIB_DEF = -DCURSES_LIB='"..\lib\pdcurses\curses.h"'
	CURSES_LNK = -L..\bin -lpdcurses
else
	CURSES_LIB_DEF = -DCURSES_LIB='<ncurses.h>'
	CURSES_LNK = -lncurses
endif

all: libChunks hexView libCmd main.c
	$(CC) $(CURSES_LIB_DEF) -o ..\bin\a main.c libCmd.o libChunks.o hexView.o $(CURSES_LNK) -lm 

libChunks: libChunks.c
	$(CC) $(CURSES_LIB_DEF) -c -o libChunks.o libChunks.c

hexView: hexView.c libChunks.o
	$(CC) $(CURSES_LIB_DEF) -c -o hexView.o hexView.c

libCmd: libCmd.c hexView.o
	$(CC) $(CURSES_LIB_DEF) -c -o libCmd.o libCmd.c 