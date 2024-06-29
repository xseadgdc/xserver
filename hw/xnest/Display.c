/*

Copyright 1993 by Davor Matic

Permission to use, copy, modify, distribute, and sell this software
and its documentation for any purpose is hereby granted without fee,
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in
supporting documentation.  Davor Matic makes no representations about
the suitability of this software for any purpose.  It is provided "as
is" without express or implied warranty.

*/

#include <dix-config.h>

#include <string.h>
#include <errno.h>

#include <X11/X.h>
#include <X11/Xproto.h>

#include "os/osdep.h"

#include "screenint.h"
#include "input.h"
#include "misc.h"
#include "scrnintstr.h"
#include "servermd.h"

#include "Display.h"
#include "Init.h"
#include "Args.h"

#include "icon"
#include "screensaver"

Display *xnestDisplay = NULL;
XVisualInfo *xnestVisuals;
int xnestNumVisuals;
int xnestDefaultVisualIndex;
Colormap *xnestDefaultColormaps;
static uint16_t xnestNumDefaultColormaps;
int *xnestDepths;
int xnestNumDepths;
XPixmapFormatValues *xnestPixmapFormats;
int xnestNumPixmapFormats;
Pixel xnestBlackPixel;
Pixel xnestWhitePixel;
Drawable xnestDefaultDrawables[MAXDEPTH + 1];
Pixmap xnestIconBitmap;
Pixmap xnestScreenSaverPixmap;
GC xnestBitmapGC;
unsigned long xnestEventMask;

static int _X_NORETURN
x_io_error_handler(Display * dpy)
{
    ErrorF("Lost connection to X server: %s\n", strerror(errno));
    CloseWellKnownConnections();
    OsCleanup(1);
    exit(1);
}

void
xnestOpenDisplay(int argc, char *argv[])
{
    XVisualInfo vi;
    long mask;
    int i, j;

    /* fill out the first screen and later copy it to all others */
    XnestScreenPtr xnscr = XnestScreenByIdx(0);
    xnscr->clonedFrom = -1;

    if (!xnestDoFullGeneration)
        return;

    XSetIOErrorHandler(x_io_error_handler);

    xnestCloseDisplay();

    xnscr->upstreamDisplay = XOpenDisplay(xnestDisplayName);
    if (xnscr->upstreamDisplay == NULL)
        FatalError("Unable to open display \"%s\".\n",
                   XDisplayName(xnestDisplayName));

    if (xnestSynchronize)
        XSynchronize(xnscr->upstreamDisplay, True);

    mask = VisualScreenMask;
    vi.screen = DefaultScreen(xnscr->upstreamDisplay);
    xnestVisuals = XGetVisualInfo(xnestDisplay, mask, &vi, &xnestNumVisuals);
    if (xnestNumVisuals == 0 || xnestVisuals == NULL)
        FatalError("Unable to find any visuals.\n");

    if (xnestUserDefaultClass || xnestUserDefaultDepth) {
        xnestDefaultVisualIndex = UNDEFINED;
        for (i = 0; i < xnestNumVisuals; i++)
            if ((!xnestUserDefaultClass ||
                 xnestVisuals[i].class == xnestDefaultClass)
                &&
                (!xnestUserDefaultDepth ||
                 xnestVisuals[i].depth == xnestDefaultDepth)) {
                xnestDefaultVisualIndex = i;
                break;
            }
        if (xnestDefaultVisualIndex == UNDEFINED)
            FatalError("Unable to find desired default visual.\n");
    }
    else {
        vi.visualid = XVisualIDFromVisual(DefaultVisual(xnestDisplay,
                                                        DefaultScreen
                                                        (xnestDisplay)));
        xnestDefaultVisualIndex = 0;
        for (i = 0; i < xnestNumVisuals; i++)
            if (vi.visualid == xnestVisuals[i].visualid)
                xnestDefaultVisualIndex = i;
    }

    xnestNumDefaultColormaps = xnestNumVisuals;
    xnestDefaultColormaps = xallocarray(xnestNumDefaultColormaps,
                                        sizeof(Colormap));
    for (i = 0; i < xnestNumDefaultColormaps; i++)
        xnestDefaultColormaps[i] = XCreateColormap(xnestDisplay,
                                                   DefaultRootWindow
                                                   (xnestDisplay),
                                                   xnestVisuals[i].visual,
                                                   AllocNone);

    xnestDepths = XListDepths(xnestDisplay, DefaultScreen(xnscr->upstreamDisplay),
                              &xnestNumDepths);

    xnestPixmapFormats = XListPixmapFormats(xnscr->upstreamDisplay,
                                            &xnestNumPixmapFormats);

    xnestBlackPixel = BlackPixel(xnscr->upstreamDisplay, DefaultScreen(xnscr->upstreamDisplay));
    xnestWhitePixel = WhitePixel(xnscr->upstreamDisplay, DefaultScreen(xnscr->upstreamDisplay));

    if (xnestParentWindow != (Window) 0)
        xnestEventMask = StructureNotifyMask;
    else
        xnestEventMask = 0L;

    for (i = 0; i <= MAXDEPTH; i++)
        xnestDefaultDrawables[i] = None;

    for (i = 0; i < xnestNumPixmapFormats; i++)
        for (j = 0; j < xnestNumDepths; j++)
            if (xnestPixmapFormats[i].depth == 1 ||
                xnestPixmapFormats[i].depth == xnestDepths[j]) {
                xnestDefaultDrawables[xnestPixmapFormats[i].depth] =
                    XCreatePixmap(xnscr->upstreamDisplay, DefaultRootWindow(xnscr->upstreamDisplay),
                                  1, 1, xnestPixmapFormats[i].depth);
            }

    xnestBitmapGC = XCreateGC(xnscr->upstreamDisplay, xnestDefaultDrawables[1], 0L, NULL);

    if (!(xnestUserGeometry & XValue))
        xnestX = 0;

    if (!(xnestUserGeometry & YValue))
        xnestY = 0;

    if (xnestParentWindow == 0) {
        if (!(xnestUserGeometry & WidthValue))
            xnestWidth = 3 * DisplayWidth(xnscr->upstreamDisplay,
                                          DefaultScreen(xnscr->upstreamDisplay)) / 4;

        if (!(xnestUserGeometry & HeightValue))
            xnestHeight = 3 * DisplayHeight(xnscr->upstreamDisplay,
                                            DefaultScreen(xnscr->upstreamDisplay)) / 4;
    }

    if (!xnestUserBorderWidth)
        xnestBorderWidth = 1;

    xnestIconBitmap =
        XCreateBitmapFromData(xnscr->upstreamDisplay,
                              DefaultRootWindow(xnscr->upstreamDisplay),
                              (char *) icon_bits, icon_width, icon_height);

    xnestScreenSaverPixmap =
        XCreatePixmapFromBitmapData(xnscr->upstreamDisplay,
                                    DefaultRootWindow(xnscr->upstreamDisplay),
                                    (char *) screensaver_bits,
                                    screensaver_width,
                                    screensaver_height,
                                    xnestWhitePixel,
                                    xnestBlackPixel,
                                    DefaultDepth(xnscr->upstreamDisplay,
                                                 DefaultScreen(xnscr->upstreamDisplay)));

    /* now copy over the remaining screens */
    for (i=1; i<xnestNumScreens; i++) {
        XnestScreenPtr s2 = XnestScreenByIdx(i);
        s2->upstreamDisplay = xnscr->upstreamDisplay;
        s2->clonedFrom = 0;
    }
}

void
xnestCloseDisplay(void)
{
    XnestScreenPtr xnscr = xnestScreenByIdx(0);

    if (!xnestDoFullGeneration || !xnscr->upstreamDisplay)
        return;

    /*
       If xnestDoFullGeneration all x resources will be destroyed upon closing
       the display connection.  There is no need to generate extra protocol.
     */

    free(xnestDefaultColormaps);
    XFree(xnestVisuals);
    XFree(xnestDepths);
    XFree(xnestPixmapFormats);
    XCloseDisplay(xnscr->upstreamDisplay);
}
