#include <dix-config.h>

#include <stdio.h>
#include <X11/Xmd.h>

#include "dix/dix_priv.h"
#include "dix/selection_priv.h"
#include "include/extinit_priv.h"
#include "include/os.h"

#include "namespace.h"
#include "hooks.h"

Bool noNamespaceExtension = TRUE;

DevPrivateKeyRec namespaceClientPrivKeyRec = { 0 };

void
NamespaceExtensionInit(void)
{
    if (!dixRegisterPrivateKey
        (&namespaceClientPrivKeyRec, PRIVATE_CLIENT, sizeof(struct XnamespaceClientPriv)))
        FatalError("ContainerExtensionSetup: Can't allocate client private.\n");

    XNS_LOG("initializing namespace extension ...\n");

    /* load configuration */
    if (!XnsLoadConfig()) {
        XNS_LOG("No config file. disabling Xns extension\n");
        return;
    }

    /* Do the serverClient */
    struct XnamespaceClientPriv *srv = XnsClientPriv(serverClient);
    *srv = (struct XnamespaceClientPriv) { .isServer = TRUE };
    XnamespaceAssignClient(srv, &namespaces[NS_ID_ROOT]);

    AddCallback(&ClientStateCallback, hookClientState, NULL);
    AddCallback(&SelectionFilterCallback, hookSelectionFilter, NULL);
}

void XnamespaceAssignClient(struct XnamespaceClientPriv *priv, struct Xnamespace *newns)
{
    if (priv->ns != NULL)
        priv->ns->refcnt--;

    priv->ns = newns;

    if (newns != NULL)
        newns->refcnt++;
}

void XnamespaceAssignClientByName(struct XnamespaceClientPriv *priv, const char *name)
{
    struct Xnamespace *newns = XnsFindByName(name);

    if (newns == NULL)
        newns = &namespaces[NS_ID_ANONYMOUS];

    XnamespaceAssignClient(priv, newns);
}

struct Xnamespace* XnsFindByAuth(size_t szAuthProto, const char* authProto, size_t szAuthToken, const char* authToken)
{
    char *xauthProto = (char*)malloc(szAuthProto+1);
    if (!xauthProto)
        goto notfound;

    memcpy(xauthProto, authProto, szAuthProto);
    xauthProto[szAuthProto] = 0;

    // start at first user-defined NS --> idx = 2
    for (int x=NS_ID_FIRST_USER; x<namespace_cnt; x++)
    {
        struct Xnamespace *ns = &namespaces[x];
        if ((strcmp(ns->authProto, xauthProto) == 0) &&
            (ns->authTokenLen == szAuthToken) &&
            (memcmp(ns->authTokenData, authToken, szAuthToken)==0))
        {
            free(xauthProto);
            return ns;
        }
    }
    free(xauthProto);

notfound:
    // default to rootns if credentials aren't assigned to specific NS
    return &(namespaces[NS_ID_ROOT]);
}
