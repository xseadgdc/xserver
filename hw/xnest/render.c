#include <dix-config.h>

#include <stdint.h>

#include <X11/X.h>
#include <X11/Xdefs.h>
#include <X11/Xproto.h>
#include <xcb/render.h>

#include "render/mipict.h"

#include "xnest-xcb.h"

struct xnest_picture_privrec {
    xcb_render_picture_t upstream_xid;
};

DevPrivateKeyRec xnestPicturePrivateKey;

static inline struct xnest_render_picture_privrec *
    xnest_render_picture_get_priv(PicturePtr pPict)
{
    void *p = dixLookupPrivate(&pPict->devPrivates, &xnestPicturePrivateKey);
    if (p)
        fprintf(stderr, "PRIV OK\n");
    else
        fprintf(stderr, "PRIV MISSING\n");
    return p;
}

static void xnest_render_composite(
    uint8_t op, PicturePtr pSrc, PicturePtr pMask, PicturePtr pDst,
    int16_t xSrc, int16_t ySrc, int16_t xMask, int16_t yMask,
    int16_t xDst, int16_t yDst, uint16_t width, uint16_t height)
{
    struct xnest_render_picture_privrec *pSrcPriv =
        xnest_render_picture_get_priv(pSrc);
    struct xnest_render_picture_privrec *pMaskPriv =
        xnest_render_picture_get_priv(pMask);
    struct xnest_render_picture_privrec *pDstkPriv =
        xnest_render_picture_get_priv(pDst);

    fprintf(stderr, "xnest_render_composite\n");
}

static void xnest_render_add_traps(
    PicturePtr pPicture, int16_t xOff, int16_t yOff, int ntrap,
    xTrap *traps)
{
    struct xnest_render_picture_privrec *pPicture =
        xnest_render_picture_get_priv(pPicture);

    fprintf(stderr, "xnest_render_add_traps\n");
}

static void xnest_render_rasterize_trapezoid(
    PicturePtr alpha, xTrapezoid *trap, int x_off, int y_off)
{
    struct xnest_render_picture_privrec *pPicture =
        xnest_render_picture_get_priv(pPicture);

    fprintf(stderr, "xnest_render_rasterize_trapezoid\n");
}

static void xnest_render_add_triangles(
    PicturePtr pPicture, int16_t xOff, int16_t yOff, int ntri, xTriangle *tris)
{
    fprintf(stderr, "xnest_render_add_triangles\n");
}

static void xnest_render_trapezoids(
    uint8_t op, PicturePtr pSrc, PicturePtr pDst, PictFormatPtr maskFormat,
    int16_t xSrc, int16_t ySrc, int ntrap, xTrapezoid *traps)
{
    fprintf(stderr, "xnest_render_trapezoids\n");
}

static void xnest_render_triangles(
    uint8_t op, PicturePtr pSrc, PicturePtr pDst, PictFormatPtr maskFormat,
    int16_t xSrc, int16_t ySrc, int ntris, xTriangle *tris)
{
    fprintf(stderr, "xnest_render_triangles\n");
}

static void xnest_render_unrealize_glyph(ScreenPtr pScreen, GlyphPtr glyph)
{
    fprintf(stderr, "xnest_render_unrealize_glyph\n");
}

static void xnest_render_glyphs(
    uint8_t op, PicturePtr pSrc, PicturePtr pDst, PictFormatPtr maskFormat,
    int16_t xSrc, int16_t ySrc, int nlists, GlyphListPtr lists, GlyphPtr *glyphs)
{
    fprintf(stderr, "xnest_render_glyphs\n");
}

static int xnest_render_create_picture(PicturePtr pPict)
{
    fprintf(stderr, "xnest_render_create_picture\n");

    return miCreatePicture(pPict);
}

Bool xnest_picture_init(ScreenPtr pScreen)
{
    if (!miPictureInit(pScreen, 0, 0))
        return FALSE;

    if (!dixRegisterPrivateKey(&xnestPicturePrivateKey, PRIVATE_PICTURE, sizeof(struct xnest_picture_privrec))) {
        fprintf(stderr, "failed to allocate PRIVATE_PICTURE\n");
        return FALSE;
    }

    PictureScreenPtr ps = GetPictureScreen(pScreen);
    ps->CreatePicture = xnest_render_create_picture;
    ps->Composite = xnest_render_composite;
    ps->Glyphs = xnest_render_glyphs;
    ps->UnrealizeGlyph = xnest_render_unrealize_glyph;
    ps->CompositeRects = miCompositeRects;
    ps->RasterizeTrapezoid = xnest_render_rasterize_trapezoid;
    ps->Trapezoids = xnest_render_trapezoids;
    ps->AddTraps = xnest_render_add_traps;
    ps->AddTriangles = xnest_render_add_triangles;
    ps->Triangles = xnest_render_triangles;

    return TRUE;
}
