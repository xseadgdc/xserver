#define HOOK_NAME "device"

#include <dix-config.h>

#include <stdio.h>

#include "dix/dix_priv.h"
#include "dix/registry_priv.h"
#include "Xext/xacestr.h"

#include "namespace.h"
#include "clientpriv.h"
#include "hooks.h"

static const char * devPermittedRequests[] = {
    "XKEYBOARD:GetMap",		// needed by xterm
    "XKEYBOARD:SelectEvents",	// needed by xterm
    "XKEYBOARD:Bell",		// needed by gimp
};

static int devRequestPermitted(const char* reqName) {
    for (int x=0; x<ARRAY_SIZE(devPermittedRequests); x++) {
        if (strcmp(reqName, devPermittedRequests[x])==0)
            return 1;
    }
    return 0;
}

void hookDevice(CallbackListPtr *pcbl, void *unused, void *calldata)
{
    XNS_HOOK_HEAD(XaceDeviceAccessRec);
    XNS_HOOK_LOG("-- \n");
    struct XnamespaceClientPriv *obj = XnsClientPriv(serverClient);

    // root NS can do anything
    if (clientSameNS(subj, obj))
        return;

    // should be safe to pass for anybody
    switch (client->majorOp) {
        case X_QueryPointer:
        case X_GetInputFocus:
        case X_GetKeyboardMapping:
        case X_GetModifierMapping:
        case X_GrabButton: // needed by xterm -- should be safe
            goto pass;
    }

    // FIXME: that's not very efficient, but didn't find a better way to do it yet
    const char* reqName = LookupRequestName(client->majorOp, client->minorOp);

    // FIXME: let pass X11:QueryPointer only on allow_peek_mouse
    if (devRequestPermitted(reqName))
        return;

    XNS_HOOK_LOG("BLOCKED: keyboard access on request %s obj %d target ns %s\n",
        reqName,
        serverClient->index,
        obj->ns->id);

    param->status = BadAccess;
    return;

pass:
    param->status = Success;
    return;
}
