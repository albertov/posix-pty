#define _BSD_SOURCE
#include <sys/ioctl.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define TTYDEFCHARS
#include <termios.h>
#include <sys/ttydefaults.h>
#undef TTYDEFCHARS

#if defined(__APPLE__)
#include <util.h>
#elif defined(__linux__)
#include <pty.h>
#else /* bsd */
#include <libutil.h>
#endif

#include <HsFFI.h>

#include "open_pty.h"

#ifndef TTYDEFCHARS
static cc_t  ttydefchars[NCCS] = {
    CEOF, CEOL, CEOL, CERASE, CWERASE, CKILL, CREPRINT, 
      _POSIX_VDISABLE, CINTR, CQUIT,  CSUSP,  CDSUSP, CSTART, CSTOP,  CLNEXT,
        CDISCARD, CMIN, CTIME,  CSTATUS, _POSIX_VDISABLE
};
#endif

int
hs_openpty
    ( HsInt sx
    , HsInt sy
    , int *master_pty
    , int *slave_pty
    )
{
    int packet_mode = 1;
    struct winsize ws;
    struct termios tio;

    /* Set the terminal size and settings. */
    memset(&ws, 0, sizeof ws);
    ws.ws_col = sx;
    ws.ws_row = sy;

    memset(&tio, 0, sizeof tio);
    tio.c_iflag = TTYDEF_IFLAG;
    tio.c_oflag = TTYDEF_OFLAG;
    tio.c_lflag = TTYDEF_LFLAG;
    tio.c_cflag = TTYDEF_CFLAG;
    memcpy(&tio.c_cc, ttydefchars, sizeof tio.c_cc);
    cfsetspeed(&tio, TTYDEF_SPEED);

    int status = openpty(master_pty, slave_pty, NULL, &tio, &ws);
    if (!status) {
        /* Switch the pty to packet mode, we'll deal with packeting on the
           haskell side of things. */
        if (ioctl(*master_pty, TIOCPKT, &packet_mode) == -1) return 1;
        if (ioctl(*slave_pty, TIOCPKT, &packet_mode) == -1) return 1;
        //FIXME Close ptys
    }
    return status;
}
