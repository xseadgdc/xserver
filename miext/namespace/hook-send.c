#define HOOK_NAME "send"

#include <dix-config.h>

#include <stdio.h>
#include <stdbool.h>

#include "dix/registry_priv.h"

#include "namespace.h"
#include "clientpriv.h"
#include "hooks.h"

void hookSend(CallbackListPtr *pcbl, void *unused, void *calldata)
{
    XNS_HOOK_HEAD(XaceSendAccessRec);
//    XNS_HOOK_LOG("--\n");

    // if no sending client, then it's coming internally from the server itself
    if (!client) {
        // FIXME: are there server-generated events that need to be filtered ?
        // or is it enough if we're checking those on receive ?
        goto pass;
    }

    // FIXME: need to check whether clients are in different NS
//    XNS_HOOK_LOG("got sending client\n");
    ClientPtr targetClient = wClient(param->pWin);
    struct XnamespaceClientPriv *obj = XnsClientPriv(targetClient);
    if (clientAllowedOnClient(client, targetClient)) {
        XNS_HOOK_LOG("same namespace\n");
        goto pass;
    } else {
        XNS_HOOK_LOG("different namespaces --> recv is @ %s\n", obj->ns->id);
    }

    for (int i = 0; i < param->count; i++) {
        XNS_HOOK_LOG("sending event of type %s to window 0x%lx of client %d\n",
            LookupEventName(param->events[i].u.u.type),
            (unsigned long)param->pWin->drawable.id,
            targetClient->index);
    }

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

    return;

pass:
    param->status = Success;
    return;
}
