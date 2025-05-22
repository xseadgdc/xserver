/* SPDX-License-Identifier: MIT OR X11
 *
 * Copyright © 2024 Enrico Weigelt, metux IT consult <info@metux.net>
 */
#ifndef _XSERVER_DIX_REQHANDLERS_H
#define _XSERVER_DIX_REQHANDLERS_H

#include "include/dix.h"

/*
 * prototypes for various X11 request handlers
 *
 * those should only be called by the dispatcher
 */

/* events.c */
int ProcAllowEvents(ClientPtr pClient);
int ProcChangeActivePointerGrab(ClientPtr pClient);
int ProcGrabButton(ClientPtr pClient);
int ProcGetInputFocus(ClientPtr pClient);
int ProcGrabKey(ClientPtr pClient);
int ProcGrabKeyboard(ClientPtr pClient);
int ProcGrabPointer(ClientPtr pClient);
int ProcQueryPointer(ClientPtr pClient);
int ProcRecolorCursor(ClientPtr pClient);
int ProcSendEvent(ClientPtr pClient);
int ProcSetInputFocus(ClientPtr pClient);
int ProcUngrabButton(ClientPtr pClient);
int ProcUngrabKey(ClientPtr pClient);
int ProcUngrabKeyboard(ClientPtr pClient);
int ProcUngrabPointer(ClientPtr pClient);
int ProcWarpPointer(ClientPtr pClient);

int SProcChangeActivePointerGrab(ClientPtr pClient);
int SProcGrabButton(ClientPtr pClient);
int SProcGrabKey(ClientPtr pClient);
int SProcGrabKeyboard(ClientPtr pClient);
int SProcGrabPointer(ClientPtr pClient);
int SProcRecolorCursor(ClientPtr pClient);
int SProcSetInputFocus(ClientPtr pClient);
int SProcSendEvent(ClientPtr pClient);
int SProcUngrabButton(ClientPtr pClient);
int SProcUngrabKey(ClientPtr pClient);
int SProcUngrabKeyboard(ClientPtr pClient);
int SProcWarpPointer(ClientPtr pClient);

#endif /* _XSERVER_DIX_REQHANDLERS_H */
