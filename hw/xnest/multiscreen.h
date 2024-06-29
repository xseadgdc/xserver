/* SPDX-License-Identifier: MIT OR X11
 *
 * Copyright © 2024 Enrico Weigelt, metux IT consult <info@metux.net>
 */
#ifndef _XNEST_MULTISCREEN_H
#define _XNEST_MULTISCREEN_H

#include <X11/X.h>
#include <X11/Xdefs.h>
#include <X11/Xlib.h>

#include "screenint.h"

typedef struct xnest_screen {
    Window rootWindow;
    Window screenSaverWindow;
    ScreenPtr pScreen;
} XnestScreenRec, *XnestScreenPtr;

/* retrieve private screen structure by ScreenPtr */
XnestScreenPtr xnestScreenPriv(ScreenPtr pScreen);

/* retrieve private screen structure by global index */
XnestScreenPtr xnestScreenByIdx(int idx);

/* allocate new private screen structure for ScreenPtr */
XnestScreenPtr xnestAllocScreenPriv(ScreenPtr pScreen);

#endif /* _XSERVER_CALLBACK_PRIV_H */
