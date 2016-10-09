#ifndef __OPENPTY_H
#define __OPENPTY_H

#include <HsFFI.h>

int
hs_openpty
    ( HsInt sx
    , HsInt sy
    , int *master_pty
    , int *slave_pty
    );
#endif
