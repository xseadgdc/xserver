/* SPDX-License-Identifier: MIT OR X11
 *
 * Copyright © 2024 Enrico Weigelt, metux IT consult <info@metux.net>
 */
#ifndef __XSERVER_DIX_VISUAL_PRIV_H
#define __XSERVER_DIX_VISUAL_PRIV_H

#include <string.h>

#include <X11/X.h>

#include "include/privates.h"
#include "include/resource.h"
#include "include/scrnintstr.h"

/* initialization of a VisualRec object and assign new server XID */
static inline VisualPtr dixVisualInit(VisualPtr visual)
{
    memset(visual, 0, sizeof(VisualRec));
    if (!dixAllocatePrivates(&visual->devPrivates, PRIVATE_VISUAL))
        return NULL;

    visual->vid = FakeClientID(0);
    return visual;
}

/* allocate a new VisualRec object and assign new server XID */
static inline VisualPtr dixVisualAlloc(void)
{
    VisualPtr visual = calloc(1, sizeof(VisualRec));
    if (!visual)
        return NULL;

    if (!dixAllocatePrivates(&visual->devPrivates, PRIVATE_VISUAL)) {
        free(visual);
        return NULL;
    }

    visual->vid = FakeClientID(0);
    return visual;
}

static inline void dixVisualFree(VisualPtr visual)
{
    if (visual)
        dixFreeObjectWithPrivates(visual, PRIVATE_VISUAL);
}

static inline void dixVisualCleanup(VisualPtr visual)
{
    if (visual)
        _dixFiniPrivates(visual->devPrivates, PRIVATE_VISUAL);
}

static inline VisualPtr dixVisualAllocArray(int numVisual)
{
    VisualPtr visual = malloc(numVisual * sizeof(VisualRec));
    if (!visual)
        return NULL;

    for (int x=0; x<numVisual; x++) {
        if (!dixVisualInit(&visual[x])) {
            for (int y=0; y<x; y++)
                dixVisualCleanup(&visual[y]);
            free(visual);
            return NULL;
        }
    }
    return visual;
}

static inline void dixVisualFreeArray(VisualPtr visual, int numVisual)
{
    if (visual) {
        for (int x=0; x<numVisual; x++)
            dixVisualCleanup(&visual[x]);
        free(visual);
    }
}

#endif /* __XSERVER_DIX_VISUAL_PRIV_H */
