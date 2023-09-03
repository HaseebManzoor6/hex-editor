#ifndef LIBCMD_H
#define LIBCMD_H

#include "hexView.h"

/*
 *  Get a user typed command into buf
 *  Returns 0 unless command terminates with newline
 */
int acceptCmd(struct HexView *h);

int parseCmd(struct HexView *h);


#endif
