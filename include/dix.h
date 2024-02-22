/***********************************************************

Copyright 1987, 1998  The Open Group

Permission to use, copy, modify, distribute, and sell this software and its
documentation for any purpose is hereby granted without fee, provided that
the above copyright notice appear in all copies and that both that
copyright notice and this permission notice appear in supporting
documentation.

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
OPEN GROUP BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Except as contained in this notice, the name of The Open Group shall not be
used in advertising or otherwise to promote the sale, use or other dealings
in this Software without prior written authorization from The Open Group.

Copyright 1987 by Digital Equipment Corporation, Maynard, Massachusetts.

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its
documentation for any purpose and without fee is hereby granted,
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in
supporting documentation, and that the name of Digital not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.

DIGITAL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
DIGITAL BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

******************************************************************/

#ifndef DIX_H
#define DIX_H

#include "callback.h"
#include "gc.h"
#include "window.h"
#include "input.h"
#include "cursor.h"
#include "geext.h"
#include "events.h"
#include <X11/extensions/XI.h>

#define EARLIER -1
#define SAMETIME 0
#define LATER 1

#define NullClient ((ClientPtr) 0)

#define REQUEST(type)                                                   \
    type * stuff = (type *)client->requestBuffer;

#define ARRAY_SIZE(a)  (sizeof((a)) / sizeof((a)[0]))

#define REQUEST_SIZE_MATCH(req)                                         \
    do {                                                                \
        if ((sizeof(req) >> 2) != client->req_len)                      \
            return(BadLength);                                          \
    } while (0)

#define REQUEST_AT_LEAST_SIZE(req)                                      \
    do {                                                                \
        if ((sizeof(req) >> 2) > client->req_len)                       \
            return(BadLength);                                          \
    } while (0)

#define REQUEST_AT_LEAST_EXTRA_SIZE(req, extra)                         \
    do {                                                                \
        if (((sizeof(req) + ((uint64_t) (extra))) >> 2) > client->req_len) \
            return(BadLength);                                          \
    } while (0)

#define REQUEST_FIXED_SIZE(req, n)                                      \
    do {                                                                \
        if ((((sizeof(req)) >> 2) > client->req_len) ||            \
            (((n) >> 2) >= client->req_len) ||                         \
            ((((uint64_t) sizeof(req) + (n) + 3) >> 2) != (uint64_t) client->req_len)) \
            return(BadLength);                                          \
    } while (0)

#define LEGAL_NEW_RESOURCE(id,client)           \
    do {                                        \
        if (!LegalNewID((id), (client))) {      \
            (client)->errorValue = (id);        \
            return BadIDChoice;                 \
        }                                       \
    } while (0)

#define WriteReplyToClient(pClient, size, pReply)                       \
    do {                                                                \
        if ((pClient)->swapped)                                         \
            (*ReplySwapVector[((xReq *)(pClient)->requestBuffer)->reqType]) \
                (pClient, (int)(size), pReply);                         \
        else                                                            \
            WriteToClient(pClient, (int)(size), (pReply));              \
    } while (0)

#define WriteSwappedDataToClient(pClient, size, pbuf)                   \
    do {                                                                \
        if ((pClient)->swapped)                                         \
            (*(pClient)->pSwapReplyFunc)(pClient, (int)(size), pbuf);   \
        else                                                            \
            WriteToClient(pClient, (int)(size), (pbuf));                \
    } while (0)

typedef struct _TimeStamp *TimeStampPtr;

#ifndef _XTYPEDEF_CLIENTPTR
typedef struct _Client *ClientPtr;

#define _XTYPEDEF_CLIENTPTR
#endif

typedef struct _WorkQueue *WorkQueuePtr;

extern _X_EXPORT ClientPtr clients[MAXCLIENTS];
extern _X_EXPORT ClientPtr serverClient;
extern _X_EXPORT int currentMaxClients;

typedef struct _TimeStamp {
    CARD32 months;              /* really ~49.7 days */
    CARD32 milliseconds;
} TimeStamp;

typedef int HWEventQueueType;
typedef HWEventQueueType *HWEventQueuePtr;

/* dispatch.c */
extern _X_EXPORT void UpdateCurrentTime(void);

extern _X_EXPORT void UpdateCurrentTimeIf(void);

ClientPtr NextAvailableClient(void *ospriv);
void SendErrorToClient(ClientPtr client,
                       unsigned int majorCode,
                       unsigned int minorCode,
                       XID resId,
                       int errorCode);
void MarkClientException(ClientPtr client);

/* dixutils.c */

extern _X_EXPORT int dixLookupWindow(WindowPtr *result,
                                     XID id,
                                     ClientPtr client, Mask access_mode);

extern _X_EXPORT int dixLookupDrawable(DrawablePtr *result,
                                       XID id,
                                       ClientPtr client,
                                       Mask type_mask, Mask access_mode);

extern _X_EXPORT int dixLookupFontable(FontPtr *result,
                                       XID id,
                                       ClientPtr client, Mask access_mode);

extern _X_EXPORT void NoopDDA(void);

int AlterSaveSetForClient(ClientPtr client,
                          WindowPtr pWin,
                          unsigned mode,
                          Bool toRoot,
                          Bool map);

extern _X_EXPORT void BlockHandler(void *timeout);

extern _X_EXPORT void WakeupHandler(int result);

typedef void (*ServerBlockHandlerProcPtr) (void *blockData,
                                           void *timeout);

typedef void (*ServerWakeupHandlerProcPtr) (void *blockData,
                                            int result);

extern _X_EXPORT Bool RegisterBlockAndWakeupHandlers(ServerBlockHandlerProcPtr blockHandler,
                                                     ServerWakeupHandlerProcPtr wakeupHandler,
                                                     void *blockData);

extern _X_EXPORT void RemoveBlockAndWakeupHandlers(ServerBlockHandlerProcPtr blockHandler,
                                                   ServerWakeupHandlerProcPtr wakeupHandler,
                                                   void *blockData);

extern _X_EXPORT Bool QueueWorkProc(Bool (*function)(ClientPtr clientUnused,
                                                     void *closure),
                                    ClientPtr client,
                                    void *closure);

typedef Bool (*ClientSleepProcPtr) (ClientPtr client,
                                    void *closure);

Bool ClientSleep(ClientPtr client, ClientSleepProcPtr function,
                                  void *closure);

#ifndef ___CLIENTSIGNAL_DEFINED___
#define ___CLIENTSIGNAL_DEFINED___
Bool ClientSignal(ClientPtr client);
#endif                          /* ___CLIENTSIGNAL_DEFINED___ */

#define CLIENT_SIGNAL_ANY ((void *)-1)
int ClientSignalAll(ClientPtr client, ClientSleepProcPtr function, void* closure);
void ClientWakeup(ClientPtr client);
Bool ClientIsAsleep(ClientPtr client);

void SendGraphicsExpose(ClientPtr client,
                        RegionPtr pRgn,
                        XID drawable,
                        int major,
                        int minor);

/* atom.c */

extern _X_EXPORT Atom MakeAtom(const char * /*string */ ,
                               unsigned /*len */ ,
                               Bool /*makeit */ );

extern _X_EXPORT Bool ValidAtom(Atom /*atom */ );

extern _X_EXPORT const char *NameForAtom(Atom /*atom */ );

void AtomError(void) _X_NORETURN;
void FreeAllAtoms(void);
void InitAtoms(void);

/* main.c */

void SetVendorRelease(int release);

/* events.c */

extern void
SetMaskForEvent(int /* deviceid */ ,
                Mask /* mask */ ,
                int /* event */ );

void ConfineToShape(DeviceIntPtr pDev,
               RegionPtr shape,
               int *px,
               int *py);

Bool IsParent(WindowPtr maybeparent, WindowPtr child);

WindowPtr GetCurrentRootWindow(DeviceIntPtr pDev);
WindowPtr GetSpriteWindow(DeviceIntPtr pDev);

void NoticeTime(const DeviceIntPtr dev, TimeStamp time);
void NoticeEventTime(InternalEvent *ev, DeviceIntPtr dev);
TimeStamp LastEventTime(int deviceid);
Bool LastEventTimeWasReset(int deviceid);
void LastEventTimeToggleResetFlag(int deviceid, Bool state);
void LastEventTimeToggleResetAll(Bool state);

extern void
EnqueueEvent(InternalEvent * /* ev */ ,
             DeviceIntPtr /* device */ );
extern void
PlayReleasedEvents(void);

extern void
ActivatePointerGrab(DeviceIntPtr /* mouse */ ,
                    GrabPtr /* grab */ ,
                    TimeStamp /* time */ ,
                    Bool /* autoGrab */ );

extern void
DeactivatePointerGrab(DeviceIntPtr /* mouse */ );

extern void
ActivateKeyboardGrab(DeviceIntPtr /* keybd */ ,
                     GrabPtr /* grab */ ,
                     TimeStamp /* time */ ,
                     Bool /* passive */ );

extern void
DeactivateKeyboardGrab(DeviceIntPtr /* keybd */ );

extern BOOL
ActivateFocusInGrab(DeviceIntPtr /* dev */ ,
                    WindowPtr /* old */ ,
                    WindowPtr /* win */ );

extern void
AllowSome(ClientPtr /* client */ ,
          TimeStamp /* time */ ,
          DeviceIntPtr /* thisDev */ ,
          int /* newState */ );

extern void
ReleaseActiveGrabs(ClientPtr client);

extern GrabPtr
CheckPassiveGrabsOnWindow(WindowPtr /* pWin */ ,
                          DeviceIntPtr /* device */ ,
                          InternalEvent * /* event */ ,
                          BOOL /* checkCore */ ,
                          BOOL /* activate */ );

int DeliverEventsToWindow(DeviceIntPtr dev,
                          WindowPtr pWin,
                          xEventPtr pEvents,
                          int count,
                          Mask filter,
                          GrabPtr grab);

void DeliverRawEvent(RawDeviceEvent *ev, DeviceIntPtr dev);

extern int
DeliverDeviceEvents(WindowPtr /* pWin */ ,
                    InternalEvent * /* event */ ,
                    GrabPtr /* grab */ ,
                    WindowPtr /* stopAt */ ,
                    DeviceIntPtr /* dev */ );

extern int
DeliverOneGrabbedEvent(InternalEvent * /* event */ ,
                       DeviceIntPtr /* dev */ ,
                       enum InputLevel /* level */ );

extern void
DeliverTouchEvents(DeviceIntPtr /* dev */ ,
                   TouchPointInfoPtr /* ti */ ,
                   InternalEvent * /* ev */ ,
                   XID /* resource */ );

extern Bool
DeliverGestureEventToOwner(DeviceIntPtr dev, GestureInfoPtr gi,
                           InternalEvent *ev);

extern void
UpdateSpriteForScreen(DeviceIntPtr /* pDev */ ,
                      ScreenPtr /* pScreen */ );

void WindowHasNewCursor(WindowPtr pWin);

extern Bool
CheckDeviceGrabs(DeviceIntPtr /* device */ ,
                 InternalEvent * /* event */ ,
                 WindowPtr /* ancestor */ );

extern void
DeliverFocusedEvent(DeviceIntPtr /* keybd */ ,
                    InternalEvent * /* event */ ,
                    WindowPtr /* window */ );

extern int
DeliverGrabbedEvent(InternalEvent * /* event */ ,
                    DeviceIntPtr /* thisDev */ ,
                    Bool /* deactivateGrab */ );

extern void
FreezeThisEventIfNeededForSyncGrab(DeviceIntPtr thisDev,
                                   InternalEvent *event);

extern void
FixKeyState(DeviceEvent * /* event */ ,
            DeviceIntPtr /* keybd */ );

extern void
RecalculateDeliverableEvents(WindowPtr /* pWin */ );

int OtherClientGone(void *value, XID id);

extern void
DoFocusEvents(DeviceIntPtr /* dev */ ,
              WindowPtr /* fromWin */ ,
              WindowPtr /* toWin */ ,
              int /* mode */ );

extern int
SetInputFocus(ClientPtr /* client */ ,
              DeviceIntPtr /* dev */ ,
              Window /* focusID */ ,
              CARD8 /* revertTo */ ,
              Time /* ctime */ ,
              Bool /* followOK */ );

extern int
GrabDevice(ClientPtr /* client */ ,
           DeviceIntPtr /* dev */ ,
           unsigned /* this_mode */ ,
           unsigned /* other_mode */ ,
           Window /* grabWindow */ ,
           unsigned /* ownerEvents */ ,
           Time /* ctime */ ,
           GrabMask * /* mask */ ,
           int /* grabtype */ ,
           Cursor /* curs */ ,
           Window /* confineToWin */ ,
           CARD8 * /* status */ );

extern void
InitEvents(void);

extern void
CloseDownEvents(void);

extern void
DeleteWindowFromAnyEvents(WindowPtr /* pWin */ ,
                          Bool /* freeResources */ );

extern Mask
EventMaskForClient(WindowPtr /* pWin */ ,
                   ClientPtr /* client */ );

int DeliverEvents(WindowPtr pWin,
                 xEventPtr xE,
                 int count,
                 WindowPtr otherParent);

extern Bool
CheckMotion(DeviceEvent * /* ev */ ,
            DeviceIntPtr /* pDev */ );

void WriteEventsToClient(ClientPtr pClient, int count, xEventPtr events);

int TryClientEvents(ClientPtr client,
                DeviceIntPtr device,
                xEventPtr pEvents,
                int count,
                Mask mask,
                Mask filter,
                GrabPtr grab);

void WindowsRestructured(void);

extern int
SetClientPointer(ClientPtr /* client */ ,
                 DeviceIntPtr /* device */ );

DeviceIntPtr PickPointer(ClientPtr client);
DeviceIntPtr PickKeyboard(ClientPtr client);

extern Bool
IsInterferingGrab(ClientPtr /* client */ ,
                  DeviceIntPtr /* dev */ ,
                  xEvent * /* events */ );

void ReinitializeRootWindow(WindowPtr win, int xoff, int yoff);

void ScreenRestructured(ScreenPtr pScreen);
>>>>>>> c8f2569254 (wip)

/*
 *  ServerGrabCallback stuff
 */

extern CallbackListPtr ServerGrabCallback;

typedef enum { SERVER_GRABBED, SERVER_UNGRABBED,
    CLIENT_PERVIOUS, CLIENT_IMPERVIOUS
} ServerGrabState;

typedef struct {
    ClientPtr client;
    ServerGrabState grabstate;
} ServerGrabInfoRec;

/*
 *  EventCallback stuff
 */

extern _X_EXPORT CallbackListPtr EventCallback;

typedef struct {
    ClientPtr client;
    xEventPtr events;
    int count;
} EventInfoRec;

/*
 *  DeviceEventCallback stuff
 */

extern CallbackListPtr DeviceEventCallback;

typedef struct {
    InternalEvent *event;
    DeviceIntPtr device;
} DeviceEventInfoRec;

extern CallbackListPtr RootWindowFinalizeCallback;

extern int
XItoCoreType(int xi_type);
extern Bool
DevHasCursor(DeviceIntPtr pDev);
Bool IsPointerDevice(DeviceIntPtr dev);
Bool IsKeyboardDevice(DeviceIntPtr dev);
extern Bool
IsPointerEvent(InternalEvent *event);
extern Bool
IsTouchEvent(InternalEvent *event);
Bool
IsGestureEvent(InternalEvent *event);
Bool
IsGestureBeginEvent(InternalEvent *event);
Bool
IsGestureEndEvent(InternalEvent *event);

Bool IsMaster(DeviceIntPtr dev);
Bool IsFloating(DeviceIntPtr dev);

extern _X_HIDDEN void
CopyKeyClass(DeviceIntPtr device, DeviceIntPtr master);
extern _X_HIDDEN int
CorePointerProc(DeviceIntPtr dev, int what);
extern _X_HIDDEN int
CoreKeyboardProc(DeviceIntPtr dev, int what);

extern void *lastGLContext;

#endif                          /* DIX_H */
