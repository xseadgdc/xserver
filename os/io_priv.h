/* SPDX-License-Identifier: MIT OR X11
 *
 * Copyright © 2024 Enrico Weigelt, metux IT consult <info@metux.net>
 */
#ifndef __XORG_OS_IO_H
#define __XORG_OS_IO_H

#include "include/os.h"

/**
 * Read an file descriptor that client should have sent (along with request).
 *
 * Caller must have set the number of expected FDs via SetReqFds() before.
 */
int ReadFdFromClient(ClientPtr client);

#endif /* __XORG_OS_IO_H */
