#define HOOK_NAME "resource"

#include <dix-config.h>

#include <stdio.h>

#include "dix/dix_priv.h"
#include "dix/registry_priv.h"
#include "mi/miinitext.h"
#include "Xext/xacestr.h"

#include "namespace.h"
#include "clientpriv.h"
#include "hooks.h"

static int checkAllowed(Mask requested, Mask allowed) {
    return ((requested & allowed) == requested);
}

void hookResourceAccess(CallbackListPtr *pcbl, void *unused, void *calldata)
{
    XNS_HOOK_HEAD(XaceResourceAccessRec);
    int owner_id = CLIENT_ID(param->id); // FIXME: need to check it for illegal value ?
    struct XnamespaceClientPriv *obj = XnsClientPriv(clients[owner_id]);
    char accModeStr[128];
    LookupDixAccessName(param->access_mode, (char*)&accModeStr, sizeof(accModeStr));

    // special filtering for windows: block transparency for untrusted clients
    if (param->rtype == RT_WINDOW) {
        WindowPtr pWindow = (WindowPtr) param->res;
        if (param->access_mode & DixCreateAccess) {
            if (!subj->ns->allowTransparency)
                pWindow->forcedBG = TRUE;
        }
    }

    // resource access inside same container is always permitted
    if (clientSameNS(subj, obj))
        goto pass;

    // server resources
    if (obj->isServer) {
        if (param->rtype == X11_RESTYPE_COLORMAP) {
            if (checkAllowed(param->access_mode, DixReadAccess | DixGetPropAccess | DixUseAccess | DixGetAttrAccess | DixAddAccess))
                goto pass;
        }

        if (param->rtype == X11_RESTYPE_WINDOW) {
            if (checkAllowed(param->access_mode, DixGetAttrAccess))
                goto pass;
        }

        printf("server object %s %s %s\n",
            accModeStr,
            LookupResourceName(param->rtype),
            LookupRequestName(client->majorOp, client->minorOp));

        if (checkAllowed(param->access_mode, DixReadAccess)) {
            printf("passing read access to server owned resources\n");
            goto pass;
        }
    }
//    Mask allowed = SecurityResourceMask;
//
//    /* additional permissions for specific resource types */
//    if (param->rtype == RT_WINDOW)
//        allowed |= SecurityWindowExtraMask;
//
//    /* special checks for server-owned resources */
//    if (cid == 0) {
//        if (param->rtype & RC_DRAWABLE)
//            /* additional operations allowed on root windows */
//            allowed |= SecurityRootWindowExtraMask;
//
//        else if (ac->rtype == RT_COLORMAP)
//            /* allow access to default colormaps */
//            allowed = param->access_mode;
//
//        else
//            /* allow read access to other server-owned resources */
//            allowed |= DixReadAccess;
//    }

//    param->status = BadAccess;    /* deny access */
    XNS_HOOK_LOG("access 0x%07lx %s to %s 0x%06lx of client %d @ %s\n",
        (unsigned long)param->access_mode,
        accModeStr,
        LookupResourceName(param->rtype),
        (unsigned long)param->id,
        owner_id, // resource owner
        obj->ns->id);
    return;

pass:
    // request is passed as it is (or already had been rewritten)
    param->status = Success;
    return;
}
