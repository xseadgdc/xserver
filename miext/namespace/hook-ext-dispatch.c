#define HOOK_NAME "ext-dispatch"

#include <dix-config.h>

#include <stdio.h>
#include <X11/extensions/XKB.h>

#include "dix/dix_priv.h"
#include "dix/registry_priv.h"
#include "Xext/xacestr.h"

#include "namespace.h"
#include "clientpriv.h"
#include "hooks.h"

void hookExtDispatch(CallbackListPtr *pcbl, void *unused, void *calldata)
{
    XNS_HOOK_HEAD(XaceExtAccessRec);
    const char *extname = param->ext->name;

    XNS_HOOK_LOG("-- \n");

    // unrestricted access to these
    if (streq(extname, "BIG-REQUESTS") || streq(extname, "XC-MISC"))
        goto pass;

    if (streq(extname, "XKEYBOARD")) {
        switch (client->minorOp) {
            case X_kbUseExtension:
            case X_kbGetMap:
            case X_kbSelectEvents: // fixme: might need special filtering
                goto pass;
        }

        XNS_HOOK_LOG("XKEYBOARD call: %s\n", param->ext->name);
    }

    XNS_HOOK_LOG("extension call: %s\n", param->ext->name);
    return;

pass:
    param->status = Success;
    return;
}
