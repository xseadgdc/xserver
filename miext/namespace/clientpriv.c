
#include <dix-config.h>

#include "windowstr.h"
#include "propertyst.h"

#include "namespace.h"
#include "clientpriv.h"

bool clientSameNS(struct XnamespaceClientPriv *p1, struct XnamespaceClientPriv *p2)
{
    return (p1->ns == p2->ns);
}

int isServer(ClientPtr client) {
    struct XnamespaceClientPriv *subj = XnsClientPriv(client);
    return subj->isServer;
}

// TRUE if subj client is allowed to do things on obj)
// usually if they're in the same namespace or subj is in a parent
// namespace of obj
bool clientAllowedOnClient(ClientPtr subj, ClientPtr obj) {
    struct XnamespaceClientPriv *subjPriv = XnsClientPriv(subj);
    struct XnamespaceClientPriv *objPriv = XnsClientPriv(obj);

    if (subjPriv && subjPriv->ns->superPower)
        return TRUE;

    if (subjPriv && objPriv && subjPriv->ns == objPriv->ns)
        return TRUE;

    // FIXME: check for parent namespaces
    return FALSE;
}
