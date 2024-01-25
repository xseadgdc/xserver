#ifndef __XSERVER_NAMESPACE_H
#define __XSERVER_NAMESPACE_H

#include <stdio.h>
#include <X11/Xmd.h>

#include "include/window.h"

struct Xnamespace {
    const char *id;
    Bool builtin;
    Bool superPower;
    size_t refcnt;
};

#define MAX_NAMESPACE 64
extern struct Xnamespace namespaces[MAX_NAMESPACE];
extern int namespace_cnt;

#define NS_ID_ROOT        0
#define NS_ID_ANONYMOUS   1
#define NS_ID_FIRST_USER  (NS_ID_ANONYMOUS+1)

#define NS_NAME_ROOT      "root"
#define NS_NAME_ANONYMOUS "anon"

Bool XnsLoadConfig(void);
struct Xnamespace *XnsFindByName(const char* name);

#define XNS_LOG(...) do { printf("XNS "); printf(__VA_ARGS__); } while (0)

#endif /* __XSERVER_NAMESPACE_H */
