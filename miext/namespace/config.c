#include <dix-config.h>

#include <string.h>
#include <X11/Xdefs.h>

#include "os/auth.h"

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

static const char *container_conf_file = "containers.conf";

static struct Xnamespace* select_ns(const char* name)
{
    for (int x=0; x<namespace_cnt; x++) {
        if (strcmp(namespaces[x].id, name)==0) {
            return &namespaces[x];
        }
    }

    namespace_cnt++;
    namespaces[namespace_cnt-1].id = strdup(name);
    return &namespaces[namespace_cnt-1];
}

#define atox(c) ('0' <= c && c <= '9' ? c - '0' : \
                 'a' <= c && c <= 'f' ? c - 'a' + 10 : \
                 'A' <= c && c <= 'F' ? c - 'A' + 10 : -1)

// warning: no error checking, no buffer clearing
static int hex2bin(const char *in, char *out)
{
    while (in[0] && in[1]) {
        int top = atox(in[0]);
        if (top == -1)
            return 0;
        int bottom = atox(in[1]);
        if (bottom == -1)
            return 0;
        *out++ = (top << 4) | bottom;
        in += 2;
    }
    return 1;
}

/*
 * loadConfig
 *
 * Load the container config
*/
static void parseLine(char *line, struct Xnamespace **walk_ns)
{
    // trim newline and comments
    char *c1 = strchr(line, '\n');
    if (c1 != NULL)
        *c1 = 0;
    c1 = strchr(line, '#');
    if (c1 != NULL)
        *c1 = 0;

    /* get the first token */
    char *token = strtok(line, " ");

    if (token == NULL)
        return;

    // if no "container" statement hasn't been issued yet, use root NS
    struct Xnamespace * curr = (*walk_ns ? *walk_ns : &namespaces[0]);

    if (strcmp(token, "container") == 0)
    {
        if ((token = strtok(NULL, " ")) == NULL)
        {
            XNS_LOG("container missing id\n");
            return;
        }

        curr = *walk_ns = select_ns(token);

        if ((token = strtok(NULL, " ")) == NULL) {
            XNS_LOG("container missing parent id\n");
            curr->parentId = strdup("root");
            return;
        }
        curr->parentId = strdup(token);
        return;
    }

    if (strcmp(token, "auth") == 0)
    {
        token = strtok(NULL, " ");
        if (token == NULL)
            return;

        curr->authProto = strdup(token);
        token = strtok(NULL, " ");

        curr->authTokenLen = strlen(token)/2;
        curr->authTokenData = calloc(1, curr->authTokenLen);
        if (!curr->authTokenData) {
            curr->authTokenLen = 0;
            return;
        }
        hex2bin(token, curr->authTokenData);

        AddAuthorization(strlen(curr->authProto),
                         curr->authProto,
                         curr->authTokenLen,
                         curr->authTokenData);
        return;
    }

    if (strcmp(token, "isolate") == 0)
    {
        while ((token = strtok(NULL, " ")) != NULL)
        {
            if (strcmp(token, "objects") == 0)
                curr->isolateObjects = TRUE;
            else if (strcmp(token, "pointer") == 0)
                curr->isolatePointer = TRUE;
            else
                XNS_LOG("unknown isolate: %s\n", token);
        }
        return;
    }

    if (strcmp(token, "allow") == 0)
    {
        while ((token = strtok(NULL, " ")) != NULL)
        {
            if (strcmp(token, "mouse-motion") == 0)
                curr->allowMouseMotion = TRUE;
            else
                XNS_LOG("unknown allow: %s\n", token);
        }
        return;
    }

    if (strcmp(token, "superpower") == 0)
    {
        curr->superPower = TRUE;
        return;
    }

    XNS_LOG("unknown token \"%s\"\n", token);
}

Bool XnsLoadConfig(void)
{
    char linebuf[1024];
    FILE *fp = fopen(container_conf_file, "r");

    if (fp == NULL)
    {
        XNS_LOG("failed loading container config: %s\n", container_conf_file);
        return FALSE;
    }

    struct Xnamespace *walk_ns = NULL;
    while (fgets(linebuf, sizeof(linebuf), fp) != NULL)
        parseLine(linebuf, &walk_ns);

    fclose(fp);

    for (int x=0; x<namespace_cnt; x++) {
        struct Xnamespace *ns = &namespaces[x];
        XNS_LOG("namespace: \"%s\" \"%s\" \"%s\" \"",
            ns->id,
            ns->parentId,
            ns->authProto);
        for (int i=0; i<ns->authTokenLen; i++)
            printf("%02X", (unsigned char)ns->authTokenData[i]);
        printf("\"\n");
    }

    return TRUE;
}

struct Xnamespace *XnsFindByName(const char* nsname) {
    for (int x=0; x<namespace_cnt; x++)
        if (strcmp(namespaces[x].id, nsname) == 0)
            return &namespaces[x];
    return NULL;
}
