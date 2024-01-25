#ifndef __XSERVER_NAMESPACE_H
#define __XSERVER_NAMESPACE_H

#include <stdio.h>
#include <X11/Xmd.h>

#include "include/dixstruct.h"
#include "include/privates.h"
#include "include/window.h"

struct Xnamespace {
    const char *id;
    Bool builtin;
    Bool superPower;
    const char *authProto;
    char *authTokenData;
    size_t authTokenLen;
    size_t refcnt;
};

struct XnamespaceClientPriv {
    Bool isServer;
    XID authId;
    struct Xnamespace* ns;
};

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

#define XNS_LOG(...) do { printf("XNS "); printf(__VA_ARGS__); } while (0)

#endif /* __XSERVER_NAMESPACE_H */
