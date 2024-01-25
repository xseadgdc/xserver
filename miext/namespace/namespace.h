#ifndef __XSERVER_NAMESPACE_H
#define __XSERVER_NAMESPACE_H

#include <stdio.h>
#include <X11/Xmd.h>
#include <X11/Xatom.h>

#include "dix/registry_priv.h"
#include "include/dixstruct.h"
#include "include/privates.h"
#include "include/extinit.h"
#include "include/extnsionst.h"
#include "include/propertyst.h"
#include "include/window.h"
#include "include/windowstr.h"
#include "Xext/xacestr.h"

/* X namespace structure */
struct Xnamespace {
    const char *id;
    const char *parentId;
    Bool builtin;
    Bool allowMouseMotion;
    Bool allowTransparency;
    Bool isolateObjects;
    Bool isolatePointer;
    Bool superPower;
    const char *authProto;
    char *authTokenData;
    size_t authTokenLen;
    size_t refcnt;
    WindowPtr rootWindow;
};

struct XnamespaceClientPriv {
    Bool isServer;
    XID authId;
    struct Xnamespace* ns;
};

// FIXME: need a dynamic list
// FIXME: support multiple auth tokens per container
#define MAX_NAMESPACE 64
extern struct Xnamespace namespaces[MAX_NAMESPACE];
extern int namespace_cnt;

#define NS_ID_ROOT        0
#define NS_ID_ANONYMOUS   1
#define NS_ID_FIRST_USER  (NS_ID_ANONYMOUS+1)

#define NS_NAME_ROOT      "root"
#define NS_NAME_ANONYMOUS "anon"

extern DevPrivateKeyRec namespaceClientPrivKeyRec;

Bool XnsLoadConfig(void);
struct Xnamespace *XnsFindByName(const char* name);
struct Xnamespace* XnsFindByAuth(size_t szAuthProto, const char* authProto, size_t szAuthToken, const char* authToken);
void XnamespaceAssignClient(struct XnamespaceClientPriv *priv, struct Xnamespace *ns);
void XnamespaceAssignClientByName(struct XnamespaceClientPriv *priv, const char *name);

static inline struct XnamespaceClientPriv *XnsClientPriv(ClientPtr client) {
    if (client == NULL) return NULL;
    return dixLookupPrivate(&client->devPrivates, &namespaceClientPrivKeyRec);
}

static inline int streq(const char *c1, const char *c2) {
    return (strcmp(c1,c2) == 0);
}

static inline Bool winIsRoot(WindowPtr pWin) {
    if (!pWin)
        return FALSE;
    if (pWin->drawable.pScreen->root == pWin)
        return TRUE;
    return FALSE;
}

#define XNS_LOG(...) do { printf("XNS "); printf(__VA_ARGS__); } while (0)

static inline Bool redirectRootProperty(ATOM name) {
    const char *nameStr = NameForAtom(name);
    if (!nameStr)
        return FALSE;
    return (streq(nameStr, "CUT_BUFFER0") ||
            streq(nameStr, "CUT_BUFFER1") ||
            streq(nameStr, "CUT_BUFFER2") ||
            streq(nameStr, "CUT_BUFFER3"));
}

static inline int setWinStrProp(WindowPtr pWin, Atom name, const char *text) {
    return dixChangeWindowProperty(serverClient, pWin, name, XA_STRING,
                                   8, PropModeReplace, strlen(text), text, TRUE);
}

#endif /* __XSERVER_NAMESPACE_H */
