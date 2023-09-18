#ifndef LIBCMD_H
#define LIBCMD_H

#include <string>

#include "hexView.h"



namespace CmdErr {
    enum Type {
        Ok = 0, // No error

        UNKNOWN, // Unknown command
    };
    
    const std::string getmsg(CmdErr::Type t);
};

/*
 *  Get a user typed command into buf
 *  Returns 0 unless command terminates with newline
 */
CmdErr::Type acceptCmd(struct HexView *h);

CmdErr::Type parseCmd(struct HexView *h);


#endif
