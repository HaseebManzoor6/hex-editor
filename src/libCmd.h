#ifndef LIBCMD_H
#define LIBCMD_H

#include "hexView.h"



class CmdErr {
    public:
        enum Type {
            Ok = 0, // No error

            UNKNOWN, // Unknown command
        };
        
        static void printmsg(CmdErr::Type t);
};

/*
 *  Get a user typed command into buf
 *  Returns 0 unless command terminates with newline
 */
CmdErr::Type acceptCmd(struct HexView *h);

CmdErr::Type parseCmd(struct HexView *h);


#endif
