/***********************************************************

Copyright 1987, 1998  The Open Group

Permission to use, copy, modify, distribute, and sell this software and its
documentation for any purpose is hereby granted without fee, provided that
the above copyright notice appear in all copies and that both that
copyright notice and this permission notice appear in supporting
documentation.

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
OPEN GROUP BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Except as contained in this notice, the name of The Open Group shall not be
used in advertising or otherwise to promote the sale, use or other dealings
in this Software without prior written authorization from The Open Group.

Copyright 1987 by Digital Equipment Corporation, Maynard, Massachusetts.

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its
documentation for any purpose and without fee is hereby granted,
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in
supporting documentation, and that the name of Digital not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.

DIGITAL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
DIGITAL BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

******************************************************************/

/* The panoramix components contained the following notice */
/*****************************************************************

Copyright (c) 1991, 1997 Digital Equipment Corporation, Maynard, Massachusetts.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software.

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
DIGITAL EQUIPMENT CORPORATION BE LIABLE FOR ANY CLAIM, DAMAGES, INCLUDING,
BUT NOT LIMITED TO CONSEQUENTIAL OR INCIDENTAL DAMAGES, OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR
IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Except as contained in this notice, the name of Digital Equipment Corporation
shall not be used in advertising or otherwise to promote the sale, use or other
dealings in this Software without prior written authorization from Digital
Equipment Corporation.

******************************************************************/

#include <dix-config.h>
#include <version-config.h>

#include <pixman.h>
#include <X11/X.h>
#include <X11/Xos.h>            /* for unistd.h  */
#include <X11/Xproto.h>
#include <X11/fonts/font.h>
#include <X11/fonts/fontstruct.h>
#include <X11/fonts/libxfont2.h>

#include "config/hotplug_priv.h"
#include "dix/callback_priv.h"
#include "dix/cursor_priv.h"
#include "dix/dix_priv.h"
#include "dix/input_priv.h"
#include "dix/gc_priv.h"
#include "dix/registry_priv.h"
#include "os/audit.h"
#include "os/auth.h"
#include "os/client_priv.h"
#include "os/cmdline.h"
#include "os/ddx_priv.h"
#include "os/osdep.h"
#include "os/screensaver.h"

#include "scrnintstr.h"
#include "misc.h"
#include "os.h"
#include "windowstr.h"
#include "resource.h"
#include "dixstruct.h"
#include "gcstruct.h"
#include "extension.h"
#include "colormap.h"
#include "colormapst.h"
#include "cursorstr.h"
#include "selection.h"
#include "servermd.h"
#include "dixfont.h"
#include "extnsionst.h"
#include "privates.h"
#include "exevents.h"
#ifdef XINERAMA
#include "panoramiXsrv.h"
#else
#include "dixevents.h"          /* InitEvents() */
#endif /* XINERAMA */

#ifdef DPMSExtension
#include <X11/extensions/dpmsconst.h>
#include "dpmsproc.h"
#endif

extern void Dispatch(void);

CallbackListPtr RootWindowFinalizeCallback = NULL;

int
dix_main(int argc, char *argv[], char *envp[])
{
    int i;
    HWEventQueueType alwaysCheckForInput[2];

    display = "0";

    InitRegions();

    CheckUserParameters(argc, argv, envp);

    CheckUserAuthorization();

    ProcessCommandLine(argc, argv);

    alwaysCheckForInput[0] = 0;
    alwaysCheckForInput[1] = 1;
    while (1) {
        serverGeneration++;
	fprintf(stderr, "startup: 001 %ld\n", serverGeneration);
        ScreenSaverTime = defaultScreenSaverTime;
        ScreenSaverInterval = defaultScreenSaverInterval;
        ScreenSaverBlanking = defaultScreenSaverBlanking;
        ScreenSaverAllowExposures = defaultScreenSaverAllowExposures;

	fprintf(stderr, "startup: 002 %ld\n", serverGeneration);
        InitBlockAndWakeupHandlers();
        /* Perform any operating system dependent initializations you'd like */
        OsInit();
        if (serverGeneration == 1) {
            CreateWellKnownSockets();
            for (i = 1; i < LimitClients; i++)
                clients[i] = NullClient;
            serverClient = calloc(1, sizeof(ClientRec));
            if (!serverClient)
                FatalError("couldn't create server client");
            InitClient(serverClient, 0, (void *) NULL);
        }
        else
            ResetWellKnownSockets();
        clients[0] = serverClient;
        currentMaxClients = 1;

	fprintf(stderr, "startup: 003 %ld\n", serverGeneration);
        /* clear any existing selections */
        InitSelections();

	fprintf(stderr, "startup: 004 %ld\n", serverGeneration);
        /* Initialize privates before first allocation */
        dixResetPrivates();

        /* Initialize server client devPrivates, to be reallocated as
         * more client privates are registered
         */
	fprintf(stderr, "startup: 005 %ld\n", serverGeneration);
        if (!dixAllocatePrivates(&serverClient->devPrivates, PRIVATE_CLIENT))
            FatalError("failed to create server client privates");

	fprintf(stderr, "startup: 006 %ld\n", serverGeneration);
        if (!InitClientResources(serverClient)) /* for root resources */
            FatalError("couldn't init server resources");

	fprintf(stderr, "startup: 007 %ld\n", serverGeneration);
        SetInputCheck(&alwaysCheckForInput[0], &alwaysCheckForInput[1]);
        screenInfo.numScreens = 0;

        InitAtoms();
        InitEvents();
        xfont2_init_glyph_caching();
        dixResetRegistry();
        InitFonts();
	fprintf(stderr, "startup: 008 %ld calling InitCallbackManager\n", serverGeneration);
        InitCallbackManager();
	fprintf(stderr, "startup: 009 %ld returned from InitCallbackManager\n", serverGeneration);
        InitOutput(&screenInfo, argc, argv);

	fprintf(stderr, "startup: 010 %ld returned from InitOutput\n", serverGeneration);
        if (screenInfo.numScreens < 1)
            FatalError("no screens found");
        InitExtensions(argc, argv);
	fprintf(stderr, "startup: 011 %ld returned from InitExtensions\n", serverGeneration);

        for (i = 0; i < screenInfo.numGPUScreens; i++) {
            ScreenPtr pScreen = screenInfo.gpuscreens[i];
            if (!PixmapScreenInit(pScreen))
                FatalError("failed to create screen pixmap properties");
            if (pScreen->CreateScreenResources &&
                !(*pScreen->CreateScreenResources) (pScreen))
                FatalError("failed to create screen resources");
        }

	fprintf(stderr, "startup: 012 %ld processed gpuscreens\n", serverGeneration);

        for (i = 0; i < screenInfo.numScreens; i++) {
            ScreenPtr pScreen = screenInfo.screens[i];

            if (!PixmapScreenInit(pScreen))
                FatalError("failed to create screen pixmap properties");
            if (pScreen->CreateScreenResources &&
                !(*pScreen->CreateScreenResources) (pScreen))
                FatalError("failed to create screen resources");
            if (!CreateGCperDepth(i))
                FatalError("failed to create scratch GCs");
            if (!CreateDefaultStipple(i))
                FatalError("failed to create default stipple");
            if (!CreateRootWindow(pScreen))
                FatalError("failed to create root window");
            CallCallbacks(&RootWindowFinalizeCallback, pScreen);
        }

	fprintf(stderr, "startup: 013 %ld processed screens\n", serverGeneration);
        if (SetDefaultFontPath(defaultFontPath) != Success) {
            ErrorF("[dix] failed to set default font path '%s'",
                   defaultFontPath);
        }
        if (!SetDefaultFont("fixed")) {
            FatalError("could not open default font");
        }

        if (!(rootCursor = CreateRootCursor())) {
            FatalError("could not open default cursor font");
        }

        rootCursor = RefCursor(rootCursor);
	fprintf(stderr, "startup: 014 %ld created root cursor\n", serverGeneration);

#ifdef XINERAMA
        /*
         * Consolidate window and colourmap information for each screen
         */
        if (!noPanoramiXExtension) {
		fprintf(stderr, "startup: 014B %ld consolidate panoramix\n", serverGeneration);
            PanoramiXConsolidate();
	}
#endif /* XINERAMA */

        fflush(stderr);

        for (i = 0; i < screenInfo.numScreens; i++) {
            fprintf(stderr, "init root window on screen %d\n", i);
            InitRootWindow(screenInfo.screens[i]->root);
            fprintf(stderr, "finished root window on screen %d\n", i);
        }

	fflush(stderr);
	fprintf(stderr, "startup: 015 %ld initialized root windows\n", serverGeneration);
	fflush(stderr);

        InitCoreDevices();
        InitInput(argc, argv);
        InitAndStartDevices();
        ReserveClientIds(serverClient);
	fprintf(stderr, "startup: 016 %ld\n", serverGeneration);

        dixSaveScreens(serverClient, SCREEN_SAVER_FORCER, ScreenSaverReset);

        dixCloseRegistry();

	fprintf(stderr, "startup: 017 %ld\n", serverGeneration);

#ifdef XINERAMA
        if (!noPanoramiXExtension) {
            if (!PanoramiXCreateConnectionBlock()) {
                FatalError("could not create connection block info");
            }
        }
        else
#endif /* XINERAMA */
        {
            if (!CreateConnectionBlock()) {
                FatalError("could not create connection block info");
            }
        }

	fprintf(stderr, "startup: 018 %ld\n", serverGeneration);

        NotifyParentProcess();
	fprintf(stderr, "startup: 019 %ld\n", serverGeneration);

        InputThreadInit();
	fprintf(stderr, "startup: 020 %ld\n", serverGeneration);

        Dispatch();

	fprintf(stderr, "shutdown: 001 %ld\n", serverGeneration);
        UnrefCursor(rootCursor);

	fprintf(stderr, "shutdown: 002 %ld\n", serverGeneration);
        UndisplayDevices();
	fprintf(stderr, "shutdown: 003 %ld\n", serverGeneration);
        DisableAllDevices();
	fprintf(stderr, "shutdown: 004 %ld\n", serverGeneration);

        /* Now free up whatever must be freed */
        if (screenIsSaved == SCREEN_SAVER_ON)
            dixSaveScreens(serverClient, SCREEN_SAVER_OFF, ScreenSaverReset);
        FreeScreenSaverTimer();
	fprintf(stderr, "shutdown: 005 %ld\n", serverGeneration);

        CloseDownExtensions();
	fprintf(stderr, "shutdown: 006 %ld\n", serverGeneration);

#ifdef XINERAMA
        {
            Bool remember_it = noPanoramiXExtension;

            noPanoramiXExtension = TRUE;
            FreeAllResources();
            noPanoramiXExtension = remember_it;
        }
#else
        FreeAllResources();
#endif /* XINERAMA */
	fprintf(stderr, "shutdown: 007 %ld\n", serverGeneration);

        CloseInput();
	fprintf(stderr, "shutdown: 008 %ld\n", serverGeneration);

        InputThreadFini();

	fprintf(stderr, "shutdown: 009 %ld\n", serverGeneration);

        for (i = 0; i < screenInfo.numScreens; i++)
            screenInfo.screens[i]->root = NullWindow;

	fprintf(stderr, "shutdown: 010 %ld\n", serverGeneration);

        CloseDownDevices();

        CloseDownEvents();
	fprintf(stderr, "shutdown: 011 %ld\n", serverGeneration);

        for (i = screenInfo.numGPUScreens - 1; i >= 0; i--) {
            dixFreeScreen(screenInfo.gpuscreens[i]);
            screenInfo.numGPUScreens = i;
        }
        memset(&screenInfo.numGPUScreens, 0, sizeof(screenInfo.numGPUScreens));
	fprintf(stderr, "shutdown: 012 %ld\n", serverGeneration);

        for (i = screenInfo.numScreens - 1; i >= 0; i--) {
            dixFreeScreen(screenInfo.screens[i]);
            screenInfo.numScreens = i;
        }
        memset(&screenInfo.screens, 0, sizeof(screenInfo.numGPUScreens));
	fprintf(stderr, "shutdown: 013 %ld\n", serverGeneration);

        ReleaseClientIds(serverClient);
        dixFreePrivates(serverClient->devPrivates, PRIVATE_CLIENT);
        serverClient->devPrivates = NULL;
	fprintf(stderr, "shutdown: 014 %ld\n", serverGeneration);

	dixFreeRegistry();

        FreeFonts();
	fprintf(stderr, "shutdown: 015 %ld\n", serverGeneration);

        FreeAllAtoms();

        FreeAuditTimer();

	fprintf(stderr, "shutdown: calling DeleteCallbackManager\n");
        DeleteCallbackManager();
	fprintf(stderr, "shutdown: returned from DeleteCallbackManager\n");

        ClearWorkQueue();

        if (dispatchException & DE_TERMINATE) {
            CloseWellKnownConnections();
        }

	fprintf(stderr, "shutdown: 001\n");
        OsCleanup((dispatchException & DE_TERMINATE) != 0);

	fprintf(stderr, "shutdown: 002\n");
        if (dispatchException & DE_TERMINATE) {
            ddxGiveUp(EXIT_NO_ERROR);
            break;
        }

	fprintf(stderr, "shutdown: 003\n");
        free(ConnectionInfo);
        ConnectionInfo = NULL;
	fprintf(stderr, "shutdown: 004\n");
    }
    return 0;
}
