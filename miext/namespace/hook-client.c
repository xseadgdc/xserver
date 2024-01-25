#define HOOK_NAME "client"

#include <dix-config.h>

#include "dix/dix_priv.h"
#include "dix/registry_priv.h"

#include "mi/miinitext.h"

#include "include/extinit.h"
#include "include/extnsionst.h"
#include "include/propertyst.h"
#include "include/protocol-versions.h"
#include "include/windowstr.h"
#include "Xext/xacestr.h"

#include "namespace.h"
#include "clientpriv.h"
#include "hooks.h"

void hookClient(CallbackListPtr *pcbl, void *unused, void *calldata)
{
    XNS_HOOK_HEAD(XaceClientAccessRec);
    struct XnamespaceClientPriv *obj = XnsClientPriv(param->target);

    (void)obj;

    XNS_HOOK_LOG("(NS %s) access to client %d on request %s\n",
        subj->ns->id,
        param->target->index,
        LookupRequestName(client->majorOp, client->minorOp));
}
