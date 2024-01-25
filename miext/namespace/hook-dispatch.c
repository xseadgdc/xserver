#define HOOK_NAME "dispatch"

#include <dix-config.h>

#include "namespace.h"
#include "hooks.h"

void hookCoreDispatch(CallbackListPtr *pcbl, void *unused, void *calldata)
{
    XNS_HOOK_HEAD(XaceCoreDispatchRec);
    XNS_HOOK_LOG("-- \n");

//    struct XnamespaceClientPriv *obj = XnsClientPriv(wClient(param->pWin));

//        if (SecurityDoCheck(subj, obj, DixSendAccess, 0) == Success)
//            return;
//
//        for (i = 0; i < param->count; i++)
//            if (param->events[i].u.u.type != UnmapNotify &&
//                param->events[i].u.u.type != ConfigureRequest &&
//                param->events[i].u.u.type != ClientMessage) {
//
//                SecurityAudit("Security: denied client %d from sending event "
//                              "of type %s to window 0x%lx of client %d\n",
//                              param->client->index,
//                              LookupEventName(param->events[i].u.u.type),
//                              (unsigned long)param->pWin->drawable.id,
//                              wClient(param->pWin)->index);
//                param->status = BadAccess;
//                return;
//            }

//    if (clientSameNS(subj, obj)) {
//        XNS_HOOK_LOG("same namespace\n");
//    }

//    for (int i = 0; i < param->count; i++) {
//        XNS_HOOK_LOG("sending event of type %s to window 0x%lx of client %d\n",
//            LookupEventName(param->events[i].u.u.type),
//            (unsigned long)param->pWin->drawable.id,
//            wClient(param->pWin)->index);
//    }
}
