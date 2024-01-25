// 2do: set namespace properties on windows

#include <dix-config.h>

#include <stdio.h>
#include <X11/Xatom.h>
#include <X11/Xmd.h>

#include "dix/dix_priv.h"
#include "dix/property_priv.h"
#include "dix/registry_priv.h"
#include "dix/selection_priv.h"
#include "dix/window_priv.h"
#include "include/extinit_priv.h"
#include "include/extnsionst.h"
#include "include/os.h"
#include "include/propertyst.h"
#include "include/protocol-versions.h"
#include "include/scrnintstr.h"
#include "include/windowstr.h"
#include "mi/miinitext.h"
#include "Xext/xacestr.h"

#include "namespace.h"
#include "clientpriv.h"
#include "hooks.h"

Bool noNamespaceExtension = TRUE;

DevPrivateKeyRec namespaceClientPrivKeyRec = { 0 };

/*
 * Access modes that untrusted clients are allowed on trusted objects.
 */
//static const Mask SecurityResourceMask =
//    DixGetAttrAccess | DixReceiveAccess | DixListPropAccess |
//    DixGetPropAccess | DixListAccess;
//static const Mask SecurityWindowExtraMask = DixRemoveAccess;
//static const Mask SecurityRootWindowExtraMask =
//    DixReceiveAccess | DixSendAccess | DixAddAccess | DixRemoveAccess;
//static const Mask SecurityDeviceMask =
//    DixGetAttrAccess | DixReceiveAccess | DixGetFocusAccess |
//    DixGrabAccess | DixSetAttrAccess | DixUseAccess;
//static const Mask SecurityServerMask = DixGetAttrAccess | DixGrabAccess;
//static const Mask SecurityClientMask = DixGetAttrAccess;

static void cbRootWindow(CallbackListPtr *pcbl, void *data, void *screen)
{
    ScreenPtr pScreen = (ScreenPtr)screen;

    // only act on first screen
    if (pScreen->myNum)
        return;

    /* create the virtual root windows */
    WindowPtr realRoot = pScreen->root;
    namespaces[NS_ID_ROOT].rootWindow = realRoot;

    assert(realRoot);

    for (int x=NS_ID_ROOT+1; x<namespace_cnt; x++) {
        int rc = 0;
        WindowPtr pWin = dixCreateWindow(
            FakeClientID(0), realRoot, 0, 0, 23, 23,
            0, /* bw */
            InputOutput,
            0, /* vmask */
            NULL, /* vlist */
            0, /* depth */
            serverClient,
            wVisual(realRoot), /* visual */
            &rc);

        if (!pWin)
            FatalError("cbRootWindow: cant create namespace root for %s\n", namespaces[x].id);

        Mask mask = pWin->eventMask;
        pWin->eventMask = 0;    /* subterfuge in case AddResource fails */
        if (!AddResource(pWin->drawable.id, X11_RESTYPE_WINDOW, (void *) pWin))
            FatalError("cbRootWindow: BadAlloc\n");
        pWin->eventMask = mask;

        namespaces[x].rootWindow = pWin;

        // set window name
        char buf[PATH_MAX] = { 0 };
        snprintf(buf, sizeof(buf)-1, "XNS-ROOT:%s", namespaces[x].id);
        setWinStrProp(pWin, XA_WM_NAME, buf);
    }
}

void
NamespaceExtensionInit(void)
{
    int ret = TRUE;

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

    AddCallback(&PostInitRootWindowCallback, cbRootWindow, NULL);
    AddCallback(&ClientStateCallback, hookClientState, NULL);
    AddCallback(&PropertyWriteFilterCallback, hookWindowProperty, NULL);
    AddCallback(&SelectionFilterCallback, hookSelectionFilter, NULL);

    ret &= XaceRegisterCallback(XACE_EXT_DISPATCH, hookExtDispatch, NULL);
    ret &= XaceRegisterCallback(XACE_EXT_ACCESS, hookExtAccess, NULL);
    ret &= XaceRegisterCallback(XACE_RESOURCE_ACCESS, hookResourceAccess, NULL);
    ret &= XaceRegisterCallback(XACE_DEVICE_ACCESS, hookDevice, NULL);
    ret &= XaceRegisterCallback(XACE_PROPERTY_ACCESS, hookPropertyAccess, NULL);
    ret &= XaceRegisterCallback(XACE_SEND_ACCESS, hookSend, NULL);
    ret &= XaceRegisterCallback(XACE_RECEIVE_ACCESS, hookReceive, NULL);
    ret &= XaceRegisterCallback(XACE_CLIENT_ACCESS, hookClient, NULL);
    ret &= XaceRegisterCallback(XACE_SERVER_ACCESS, hookServerAccess, NULL);

    if (!ret)
        FatalError("NamespaceExtensionInit: Failed to register callbacks\n");
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
