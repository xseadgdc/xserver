#define HOOK_NAME "server"

#include <dix-config.h>

#include <stdio.h>

#include "dix/dix_priv.h"
#include "dix/registry_priv.h"
#include "mi/miinitext.h"
#include "Xext/xacestr.h"

#include "namespace.h"
#include "clientpriv.h"
#include "hooks.h"

void hookServerAccess(CallbackListPtr *pcbl, void *unused, void *calldata)
{
    XNS_HOOK_HEAD(XaceServerAccessRec);
    struct XnamespaceClientPriv *obj = XnsClientPriv(serverClient);

    XNS_HOOK_LOG("(NS %s) access to server configuration request %s obj NS %s\n",
        subj->ns->id,
        LookupRequestName(client->majorOp, client->minorOp),
        obj->ns->id);
}
