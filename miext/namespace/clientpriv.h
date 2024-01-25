#ifndef __XSERVER_NAMESPACE_CLIENTPRIV_H
#define __XSERVER_NAMESPACE_CLIENTPRIV_H

#include <stdbool.h>
#include <assert.h>

#include "windowstr.h"

struct Xnamespace;
struct XnamespaceClientPriv;

bool clientSameNS(struct XnamespaceClientPriv *p1, struct XnamespaceClientPriv *p2);
int isServer(ClientPtr client);
bool clientAllowedOnClient(ClientPtr subj, ClientPtr obj);

#endif /* __XSERVER_NAMESPACE_CLIENTPRIV_H */
