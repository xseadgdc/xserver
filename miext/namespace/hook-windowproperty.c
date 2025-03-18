#define HOOK_NAME "windowproperty"

#include <dix-config.h>

#include <stdio.h>

#include "dix/dix_priv.h"
#include "dix/registry_priv.h"
#include "dix/property_priv.h"
#include "Xext/xacestr.h"

#include "namespace.h"
#include "clientpriv.h"
#include "hooks.h"

void hookWindowProperty(CallbackListPtr *pcbl, void *unused, void *calldata)
{
    XNS_HOOK_HEAD(PropertyFilterParam);

    ATOM name = param->name;
    const char* nameStr = NameForAtom(name);
    if (!nameStr)
        return;

    struct XnamespaceClientPriv *obj = XnsClientPriv(wClient(param->pWin));

    goto out;

    if (clientSameNS(subj, obj))
        return;

    if (winIsRoot(param->pWin)) {
        if (redirectRootProperty(name)) {
            XNS_HOOK_LOG("redirect root window property %s (atom 0x%x) window 0x%lx of client %d\n",
                nameStr,
                name,
                (unsigned long)param->pWin->drawable.id,
                wClient(param->pWin)->index);
            if (subj->ns->rootWindow == NULL)
                printf("namespace root window is NULL\n");
            param->pWin = subj->ns->rootWindow;
            printf("--> returning\n");
            return;
        }
        XNS_HOOK_LOG("other root window property %s (atom 0x%x) window 0x%lx of client %d\n",
            nameStr,
            name,
            (unsigned long)param->pWin->drawable.id,
            wClient(param->pWin)->index);
        return;
    }

out:
    XNS_HOOK_LOG("non-root window property %s (atom 0x%x) window 0x%lx of client %d\n",
        nameStr,
        name,
        (unsigned long)param->pWin->drawable.id,
        wClient(param->pWin)->index);
}
