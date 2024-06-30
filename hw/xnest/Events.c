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
#include <X11/Xdefs.h>
#include <X11/Xproto.h>

#include "screenint.h"
#include "input.h"
#include "misc.h"
#include "scrnintstr.h"
#include "windowstr.h"
#include "servermd.h"
#include "inputstr.h"
#include "inpututils.h"

#include "mi.h"

#include "Xnest.h"

#include "Args.h"
#include "Color.h"
#include "Display.h"
#include "Screen.h"
#include "XNWindow.h"
#include "Events.h"
#include "Keyboard.h"
#include "Pointer.h"
#include "mipointer.h"

CARD32 lastEventTime = 0;

void
ProcessInputEvents(void)
{
    mieqProcessInputEvents();
}

int
TimeSinceLastInputEvent(void)
{
    if (lastEventTime == 0)
        lastEventTime = GetTimeInMillis();
    return GetTimeInMillis() - lastEventTime;
}

void
SetTimeSinceLastInputEvent(void)
{
    lastEventTime = GetTimeInMillis();
}

static Bool
xnestExposurePredicate(Display * dpy, XEvent * event, char *args)
{
    return event->type == Expose || event->type == ProcessedExpose;
}

static Bool
xnestNotExposurePredicate(Display * dpy, XEvent * event, char *args)
{
    return !xnestExposurePredicate(dpy, event, args);
}

void
xnestCollectExposures(void)
{
    XEvent X;
    WindowPtr pWin;
    RegionRec Rgn;
    BoxRec Box;

    while (XCheckIfEvent(xnestDisplay, &X, xnestExposurePredicate, NULL)) {
        pWin = xnestWindowPtr(X.xexpose.window);

        if (pWin && X.xexpose.width && X.xexpose.height) {
            Box.x1 = pWin->drawable.x + wBorderWidth(pWin) + X.xexpose.x;
            Box.y1 = pWin->drawable.y + wBorderWidth(pWin) + X.xexpose.y;
            Box.x2 = Box.x1 + X.xexpose.width;
            Box.y2 = Box.y1 + X.xexpose.height;

            RegionInit(&Rgn, &Box, 1);

            miSendExposures(pWin, &Rgn, Box.x2, Box.y2);
        }
    }
}

void
xnestQueueKeyEvent(int type, unsigned int keycode)
{
    lastEventTime = GetTimeInMillis();
    QueueKeyboardEvents(xnestKeyboardDevice, type, keycode);
}

static const char* evTypeName(XEvent *ev) {
    switch (ev->type) {
        case PropertyNotify: return "PropertyNotify";
        case KeyPress: return "KeyPress";
        case KeyRelease: return "KeyRelease";
        case ButtonPress: return "ButtonPress";
        case ButtonRelease: return "ButtonRelease";
        case MotionNotify: return "MotionNotify";
        case FocusIn: return "FocusIn";
        case FocusOut: return "FocusOut";
        case KeymapNotify: return "KeymapNotify";
        case EnterNotify: return "EnterNotify";
        case LeaveNotify: return "LeaveNotify";
        case DestroyNotify: return "DestroyNotify";
        case CirculateNotify: return "CirculateNotify";
        case ConfigureNotify: return "ConfigureNotify";
        case GravityNotify: return "GravityNotify";
        case MapNotify: return "MapNotify";
        case ReparentNotify: return "ReparentNotify";
        case UnmapNotify: return "UnmapNotify";
        case NoExpose: return "NoExpose";
        case ResizeRequest: return "ResizeRequest";
        default: return "UNKNOWN";
    }
    return "<UNKNOWN>";
}

static void rootlessEventDownstream(XEvent *ev)
{
    switch (ev->type) {
        // FIXME: check whether it's really a toplevel window
        case ConfigureNotify:
            // ignore messages sent to parents
            if (ev->xconfigure.window != ev->xconfigure.event)
                return;

            WindowPtr pWin = xnestWindowPtr(ev->xconfigure.window);
            xnestPrivWin *priv = xnestWindowPriv(pWin);

            // FIXME: missing aboveSibling
            // FIXME: missing borderWidth
            // FIXME: missing overrideRedirect
// FIXME: trouble w/ framed windows. does the WM fake this data ?
// problem: the client's content looks moved up under the title bar
//            pWin->drawable.x = ev->xconfigure.x;
//            pWin->drawable.y = ev->xconfigure.y;
            pWin->drawable.width = ev->xconfigure.width;
            pWin->drawable.height = ev->xconfigure.height;

            printf("ConfigureNotify: X=%d Y=%d W=%d H=%d\n",
                pWin->drawable.x, pWin->drawable.y, pWin->drawable.width, pWin->drawable.height);

            xEvent event = {
                .u.configureNotify = {
                .window = pWin->drawable.id,
                .aboveSibling = None, // FIXME ! need to look it up !!!
                .x = pWin->drawable.x,
                .y = pWin->drawable.y,
                .width = pWin->drawable.width,
                .height = pWin->drawable.height,
                .borderWidth = wBorderWidth(pWin),
                .override = pWin->overrideRedirect
                }
            };
            event.u.u.type = ConfigureNotify;

            printf(" ---> type=%d\n", (&event)->u.u.type);
//            DeliverEvents(pWin, &event, 1, NullWindow); /* ??? */
            DeliverEvents(pWin, &event, 1, pWin); /* ??? */
            printf(" ----> finished ConfigureNotify\n");
        break;
    }
}

void
xnestCollectEvents(void)
{
    XEvent X;
    int valuators[2];
    ValuatorMask mask;
    ScreenPtr pScreen;

    while (XCheckIfEvent(xnestDisplay, &X, xnestNotExposurePredicate, NULL)) {
        switch (X.type) {
        case KeyPress:
            xnestUpdateModifierState(X.xkey.state);
            xnestQueueKeyEvent(KeyPress, X.xkey.keycode);
            break;

        case KeyRelease:
            xnestUpdateModifierState(X.xkey.state);
            xnestQueueKeyEvent(KeyRelease, X.xkey.keycode);
            break;

        case ButtonPress:
            valuator_mask_set_range(&mask, 0, 0, NULL);
            xnestUpdateModifierState(X.xkey.state);
            lastEventTime = GetTimeInMillis();
            QueuePointerEvents(xnestPointerDevice, ButtonPress,
                               X.xbutton.button, POINTER_RELATIVE, &mask);
            break;

        case ButtonRelease:
            valuator_mask_set_range(&mask, 0, 0, NULL);
            xnestUpdateModifierState(X.xkey.state);
            lastEventTime = GetTimeInMillis();
            QueuePointerEvents(xnestPointerDevice, ButtonRelease,
                               X.xbutton.button, POINTER_RELATIVE, &mask);
            break;

        case MotionNotify:
            valuators[0] = X.xmotion.x;
            valuators[1] = X.xmotion.y;
            valuator_mask_set_range(&mask, 0, 2, valuators);
            lastEventTime = GetTimeInMillis();
            QueuePointerEvents(xnestPointerDevice, MotionNotify,
                               0, POINTER_ABSOLUTE, &mask);
            break;

        case FocusIn:
            if (X.xfocus.detail != NotifyInferior) {
                pScreen = xnestScreen(X.xfocus.window);
                if (pScreen)
                    xnestDirectInstallColormaps(pScreen);
            }
            break;

        case FocusOut:
            if (X.xfocus.detail != NotifyInferior) {
                pScreen = xnestScreen(X.xfocus.window);
                if (pScreen)
                    xnestDirectUninstallColormaps(pScreen);
            }
            break;

        case KeymapNotify:
            break;

        case EnterNotify:
            if (X.xcrossing.detail != NotifyInferior) {
                pScreen = xnestScreen(X.xcrossing.window);
                if (pScreen) {
                    NewCurrentScreen(inputInfo.pointer, pScreen, X.xcrossing.x,
                                     X.xcrossing.y);
                    valuators[0] = X.xcrossing.x;
                    valuators[1] = X.xcrossing.y;
                    valuator_mask_set_range(&mask, 0, 2, valuators);
                    lastEventTime = GetTimeInMillis();
                    QueuePointerEvents(xnestPointerDevice, MotionNotify,
                                       0, POINTER_ABSOLUTE, &mask);
                    xnestDirectInstallColormaps(pScreen);
                }
            }
            break;

        case LeaveNotify:
            if (X.xcrossing.detail != NotifyInferior) {
                pScreen = xnestScreen(X.xcrossing.window);
                if (pScreen) {
                    xnestDirectUninstallColormaps(pScreen);
                }
            }
            break;

        case DestroyNotify:
            if (xnestParentWindow != (Window) 0 &&
                X.xdestroywindow.window == xnestParentWindow)
                exit(0);
            break;

        case CirculateNotify:
        case ConfigureNotify:
        case GravityNotify:
        case MapNotify:
        case ReparentNotify:
        case UnmapNotify:
        case NoExpose:
            break;

        default:
//            ErrorF("xnest warning: unhandled event: %d\n", X.type);
            break;
        }
        if (xnestRootless)
            rootlessEventDownstream(&X);
    }
}
