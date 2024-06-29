
#include "multiscreen.h"
#include "scrnintstr.h"

XnestScreenRec xnestScreens[MAXSCREENS] = { 0 };

XnestScreenPtr xnestAllocScreenPriv(ScreenPtr pScreen)
{
    xnestScreens[pScreen->myNum].pScreen = pScreen;
    return &xnestScreens[pScreen->myNum];
}

XnestScreenPtr xnestScreenPriv(ScreenPtr pScreen)
{
    return &xnestScreens[pScreen->myNum];
}

XnestScreenPtr xnestScreenByIdx(int idx)
{
    return &xnestScreens[idx];
}
