#ifndef __XSERVER_NAMESPACE_H
#define __XSERVER_NAMESPACE_H

#include <stdio.h>

#define XNS_LOG(...) do { printf("XNS "); printf(__VA_ARGS__); } while (0)

#endif /* __XSERVER_NAMESPACE_H */
