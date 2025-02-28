/* SPDX-License-Identifier: MIT OR X11
 *
 * Copyright © 2024 Enrico Weigelt, metux IT consult <info@metux.net>
 */
#ifndef __XNEST__PARSE_GEOMETRY_H
#define __XNEST__PARSE_GEOMETRY_H

#include <X11/Xproto.h>

int xnest_parse_geometry(const char *string, xRectangle *geometry);

#endif /* __XNEST__XCB_H */
