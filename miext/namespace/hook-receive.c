#define HOOK_NAME "recieve"

#include <dix-config.h>

#include <stdio.h>
#include <stdbool.h>

#include "dix/registry_priv.h"

#include "namespace.h"
#include "clientpriv.h"
#include "hooks.h"

static bool isRootWin(WindowPtr win) {
    return (win->parent == NullWindow && wClient(win)->index == 0);
}

#include "exglobals.h"

void
hookReceive(CallbackListPtr *pcbl, void *unused, void *calldata)
{
    XNS_HOOK_HEAD(XaceReceiveAccessRec);
    struct XnamespaceClientPriv *obj = XnsClientPriv(wClient(param->pWin));

    // send and receive within same namespace permitted without restrictions
    if (clientSameNS(subj, obj)) {
//        XNS_HOOK_LOG("subj and obj in same namespace\n");
        return;
    }

    XNS_HOOK_LOG("subj and obj in different namespaces --> %s vs %s\n",
        subj->ns->id, obj->ns->id);

    for (int i=0; i<param->count; i++) {
        XNS_HOOK_LOG("rec #%d\n", i);
        int type = param->events[i].u.u.type;
        switch (type) {
            case GenericEvent:
                XNS_HOOK_LOG("generic event\n");
                xGenericEvent *gev = (xGenericEvent*)&param->events[i].u;
                if (gev->extension == IReqCode) {
                    switch (gev->evtype) {
                        case XI_RawMotion:
                            if ((!subj->ns->allowMouseMotion) || (!isRootWin(param->pWin)))
                                param->status = BadAccess;
                            continue;
                        default:
                            param->status = BadAccess;
                            XNS_HOOK_LOG("XI unknown %d\n", gev->evtype);
                    }
                }
            break;

            default:
                XNS_HOOK_LOG("event type 0%0x 0%0x %s %s\n", type, param->events[i].u.u.detail,
                    LookupEventName(type), (type & 128) ? "fake" : "");
                XNS_HOOK_LOG("====> default case --> BadAccess\n");
                param->status = BadAccess;

                if (isRootWin(param->pWin))
                    XNS_HOOK_LOG("receiving from root window\n");
                else
                    XNS_HOOK_LOG("not from root window\n");
            break;
        }
    }

    if (param->status == BadAccess) {
        XNS_HOOK_LOG("BLOCKED client %d [NS %s] receiving event sent to window 0x%lx of client %d [NS %s]\n",
            client->index,
            subj->ns->id,
            (unsigned long)param->pWin->drawable.id,
            wClient(param->pWin)->index,
            obj->ns->id);
    }
}
