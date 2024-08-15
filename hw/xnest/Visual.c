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

#ifdef HAVE_XNEST_CONFIG_H
#include <xnest-config.h>
#endif

#include <X11/X.h>
#include <X11/Xproto.h>
#include "scrnintstr.h"
#include "dix.h"
#include "mi.h"
#include "Xnest.h"

#include "Display.h"
#include "Visual.h"

Visual *
xnestVisual(VisualPtr pVisual)
{
    int i;

    fprintf(stderr, "xnestVisual: class=%d bits=%d cm=%d depth=%d red=%ld green=%ld blue=%ld\n",
        pVisual->class, pVisual->bitsPerRGBValue, pVisual->ColormapEntries,
        pVisual->nplanes, pVisual->redMask, pVisual->greenMask, pVisual->blueMask);

    for (i = 0; i < xnestNumVisuals; i++)
        if (pVisual->class == xnestVisuals[i].class &&
            pVisual->bitsPerRGBValue == xnestVisuals[i].bits_per_rgb &&
            pVisual->ColormapEntries == xnestVisuals[i].colormap_size &&
            pVisual->nplanes == xnestVisuals[i].depth &&
            pVisual->redMask == xnestVisuals[i].red_mask &&
            pVisual->greenMask == xnestVisuals[i].green_mask &&
            pVisual->blueMask == xnestVisuals[i].blue_mask) {
                fprintf(stderr, "found visual object\n");
            return xnestVisuals[i].visual;
        }

    return NULL;
}

VisualPtr ScreenGetVisual(ScreenPtr pScreen, VisualID visual)
{
    for (int i = 0; i < pScreen->numVisuals; i++) {
        fprintf(stderr, "comparing #%d got %d want %d\n", i, pScreen->visuals[i].vid, visual);
        if (pScreen->visuals[i].vid == visual) {
            fprintf(stderr, "found it by id %d\n", visual);
            return &pScreen->visuals[i];
        }
    }

    fprintf(stderr, "cant find visual by id %d\n", visual);
    return NULL;
}

Visual *
xnestVisualFromID(ScreenPtr pScreen, VisualID visual)
{
    VisualPtr v = ScreenGetVisual(pScreen, visual);
    if (v)
        return xnestVisual(v);
    return NULL;
}

Colormap
xnestDefaultVisualColormap(Visual * visual)
{
    int i;

    for (i = 0; i < xnestNumVisuals; i++)
        if (xnestVisuals[i].visual == visual)
            return xnestDefaultColormaps[i];

    return XCB_WINDOW_NONE;
}
