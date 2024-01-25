#include <dix-config.h>

#include <string.h>
#include <X11/Xdefs.h>

#include "namespace.h"

struct Xnamespace namespaces[MAX_NAMESPACE] = {
    [ NS_ID_ROOT ] = {
        .id = NS_NAME_ROOT,
        .builtin = TRUE,
        .superPower = TRUE,
        .refcnt = 1,
    },
    [ NS_ID_ANONYMOUS ] = {
        .id = NS_NAME_ANONYMOUS,
        .builtin = TRUE,
        .refcnt = 1,
    }
};

int namespace_cnt = NS_ID_FIRST_USER;

Bool XnsLoadConfig(void)
{
    return TRUE;
}

struct Xnamespace *XnsFindByName(const char* nsname) {
    for (int x=0; x<namespace_cnt; x++)
        if (strcmp(namespaces[x].id, nsname) == 0)
            return &namespaces[x];
    return NULL;
}
