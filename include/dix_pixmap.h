/* SPDX-License-Identifier: MIT OR X11
 *
 * Copyright © 2024 Enrico Weigelt, metux IT consult <info@metux.net>
 *
 * @brief generic pixmap functions
 *
 * These functions replacing directly calling into ScreenRec vectors and also
 * take care of calling hooks.

 * NOTE: The ScreenProc vectors should never be called directly anymore,
 * otherwise hooks might not be called.
 */
#ifndef DIX_PIXMAP_H
#define DIX_PIXMAP_H

#include <X11/Xfuncproto.h>

#include "screenint.h" /* ScreenPtr */
#include "window.h" /* WindowPtr */

/*
 * @brief create a pixmap on specified screen
 *
 * Call into driver and hooks for creating a pixmap with given geometry.
 *
 * @param pScreen the screen to create the pixmap on
 * @param width width of the new pixmap
 * @param height height of the new pixmap
 * @param depth color depth of the new pixmap
 * @param usage_hints allocation hints - see CREATE_PIXMAP_USAGE_* defines
 * @result pointer to new pixmap or NULL on failure
 */
_X_EXPORT PixmapPtr dixPixmapCreate(ScreenPtr pScreen,
                                    uint32_t width,
                                    uint32_t height,
                                    uint32_t depth,
                                    uint32_t usage_hint);

/*
 * @brief increase pixmap reference counter
 *
 * Increase the pixmap's reference counter.
 *
 * @param pPixmap the pixmap who's reference counter is increased
 * @return the original pPixmap value (for function chaining)
 */
_X_EXPORT PixmapPtr dixPixmapGet(PixmapPtr pPixmap);

/*
 * @brief decrease reference counter and potentially release it
 *
 * Decrease the pixmap's reference counter. If it reaches zero, destroy it.
 *
 * @param pPixmap the pixmap to release (NULL protected)
 */
_X_EXPORT void dixPixmapPut(PixmapPtr pPixmap);

#endif /* DIX_PIXMAP_H */
