#define HOOK_NAME "ext-access"

#include <dix-config.h>

#include "dix/dix_priv.h"
#include "dix/registry_priv.h"
#include "Xext/xacestr.h"

#include "namespace.h"
#include "clientpriv.h"
#include "hooks.h"

// called on X_QueryExtension
void hookExtAccess(CallbackListPtr *pcbl, void *unused, void *calldata)
{
    XNS_HOOK_HEAD(XaceExtAccessRec);
    XNS_HOOK_LOG(" --\n");
    const char *extname = param->ext->name;

    if (streq(extname, "BIG-REQUESTS") || streq(extname, "XC-MISC") ||
        streq(extname, "XKEYBOARD"))
        goto pass;

    XNS_HOOK_LOG("extension query: %s\n", extname);

    return;

pass:
    param->status = Success;
    return;
}
