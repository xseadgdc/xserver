/* SPDX-License-Identifier: MIT OR X11
 *
 * Copyright © 2024 Enrico Weigelt, metux IT consult <info@metux.net>
 */
#include <dix-config.h>

#include <X11/Xproto.h>
#include <X11/Xprotostr.h>
#include <xcb/xcb.h>

#include "parse-geometry.h"

static int __readint(const char *str, const char **next)
{
    int res = 0, sign = 1;

    if (*str == '+')
        str++;
    else if (*str == '-') {
        str++;
        sign = -1;
    }

    for (; (*str >= '0') && (*str <= '9'); str++)
        res = (res * 10) + (*str - '0');

    *next = str;
    return sign * res;
}

int xnest_parse_geometry(const char *string, xRectangle *geometry)
{
    int mask = 0;
    const char *next;
    xRectangle temp = { 0 };

    if ((string == NULL) || (*string == '\0')) return 0;

    if (*string == '=')
        string++;  /* ignore possible '=' at beg of geometry spec */

    if (*string != '+' && *string != '-' && *string != 'x') {
        temp.width = __readint(string, &next);
        if (string == next)
            return 0;
        string = next;
        mask |= XCB_CONFIG_WINDOW_WIDTH;
    }

    if (*string == 'x' || *string == 'X') {
        string++;
        temp.height = __readint(string, &next);
        if (string == next)
            return 0;
        string = next;
        mask |= XCB_CONFIG_WINDOW_HEIGHT;
    }

    if ((*string == '+') || (*string == '-')) {
        if (*string == '-') {
            string++;
            temp.x = -__readint(string, &next);
            if (string == next)
                return 0;
            string = next;
        }
        else
        {
            string++;
            temp.x = __readint(string, &next);
            if (string == next)
                return 0;
            string = next;
        }
        mask |= XCB_CONFIG_WINDOW_X;
        if ((*string == '+') || (*string == '-')) {
            if (*string == '-') {
                string++;
                temp.y = -__readint(string, &next);
                if (string == next)
                    return 0;
                string = next;
            }
            else
            {
                string++;
                temp.y = __readint(string, &next);
                if (string == next)
                    return 0;
                string = next;
            }
            mask |= XCB_CONFIG_WINDOW_Y;
        }
    }

    if (*string != '\0') return 0;

    if (mask & XCB_CONFIG_WINDOW_X)
        geometry->x = temp.x;
    if (mask & XCB_CONFIG_WINDOW_Y)
        geometry->y = temp.y;
    if (mask & XCB_CONFIG_WINDOW_WIDTH)
        geometry->width = temp.width;
    if (mask & XCB_CONFIG_WINDOW_HEIGHT)
        geometry->height = temp.height;

    return mask;
}
