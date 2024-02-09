/* SPDX-License-Identifier: MIT OR X11
 *
 * Copyright © 2024 Enrico Weigelt, metux IT consult <info@metux.net>
 * Copyright © 2000 Jakub Jelinek (jakub@redhat.com)
 */
#ifndef _XSERVER_XF86_SBUSBUS_H
#define _XSERVER_XF86_SBUSBUS_H

#include <X11/Xdefs.h>

#include "xf86sbusBus.h"

typedef struct sbus_prom_node {
    int node;
    /* Because of misdesigned openpromio */
    int cookie[2];
} sbusPromNode, *sbusPromNodePtr;

typedef struct sbus_device {
    int devId;
    int fbNum;
    int fd;
    int width, height;
    sbusPromNode node;
    const char *descr;
    const char *device;
} sbusDevice, *sbusDevicePtr;

Bool xf86SbusConfigure(void *busData, sbusDevicePtr sBus);
void xf86SbusConfigureNewDev(void *busData, sbusDevicePtr sBus, GDevRec* GDev);

#endif /* _XSERVER_XF86_SBUSBUS_H */
