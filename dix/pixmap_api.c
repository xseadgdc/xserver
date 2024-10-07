/* SPDX-License-Identifier: MIT OR X11
 *
 * Copyright © 2024 Enrico Weigelt, metux IT consult <info@metux.net>
 *
 * @brief pixmap lifecycle API
 */
#include <dix-config.h>

#include "dix/dix_priv.h"
#include "include/dix_pixmap.h"
#include "include/pixmapstr.h"
#include "include/scrnintstr.h"

PixmapPtr dixPixmapCreate(ScreenPtr pScreen,
                          uint32_t width,
                          uint32_t height,
                          uint32_t depth,
                          uint32_t usage_hint)
{
    if (!pScreen) {
        LogMessage(X_ERROR, "dixCreatePixmap() called on NULL screen\n");
        return NULL;
    }

    if (!pScreen->CreatePixmap) {
        LogMessage(X_ERROR, "dixCreatePixmap() screen->CreatePixmap is NULL\n");
        return NULL;
    }

    return pScreen->CreatePixmap(pScreen, width, height, depth, usage_hint);
}

PixmapPtr dixPixmapGet(PixmapPtr pPixmap)
{
    if (pPixmap)
        pPixmap->refcnt++;
    return pPixmap;
}

void dixPixmapPut(PixmapPtr pPixmap)
{
    if (pPixmap && pPixmap->refcnt == 1)
        dixScreenRaisePixmapDestroy(pPixmap);
    if (pPixmap && pPixmap->drawable.pScreen && pPixmap->drawable.pScreen->DestroyPixmap)
        pPixmap->drawable.pScreen->DestroyPixmap(pPixmap);
}
