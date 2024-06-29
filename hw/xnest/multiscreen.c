
#include "multiscreen.h"
#include "scrnintstr.h"

XnestScreenRec xnestScreens[MAXSCREENS] = { 0 };

XnestScreenPtr xnestScreenPriv(ScreenPtr pScreen)
{
    return &xnestScreens[pScreen->myNum];
}

XnestScreenPtr xnestScreenByIdx(int idx)
{
    return &xnestScreens[idx];
}
