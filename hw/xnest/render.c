#include <dix-config.h>

#include <stdint.h>

#include <X11/X.h>
#include <X11/Xdefs.h>
#include <X11/Xproto.h>
#include <xcb/render.h>

#include "render/mipict.h"

#include "xnest-xcb.h"

struct xnest_render_picture_privrec {
    xcb_render_picture_t upstream_xid;
};

DevPrivateKeyRec xnestPicturePrivateKey;

static inline struct xnest_render_picture_privrec *
    xnest_render_picture_get_priv(PicturePtr pPict)
{
    if (pPict == NULL) {
        fprintf(stderr, "NULL passed\n");
        return NULL;
    }

    return dixLookupPrivate(&pPict->devPrivates, &xnestPicturePrivateKey);
}

static inline xcb_render_picture_t privId(struct xnest_render_picture_privrec *p)
{
    return p ? p->upstream_xid : 0;
}

static void xnest_render_composite(
    uint8_t op, PicturePtr pSrc, PicturePtr pMask, PicturePtr pDst,
    int16_t src_x, int16_t src_y, int16_t mask_x, int16_t mask_y,
    int16_t dst_x, int16_t dst_y, uint16_t width, uint16_t height)
{
    if (!pSrc)
        fprintf(stderr, "xnest_render_composite: NULL pScr\n");
    if (!pMask)
        fprintf(stderr, "xnest_render_composite: NULL pMask\n");
    if (!pDst)
        fprintf(stderr, "xnest_render_composite: NULL pDst\n");

    struct xnest_render_picture_privrec *pSrcPriv =
        xnest_render_picture_get_priv(pSrc);
    struct xnest_render_picture_privrec *pMaskPriv =
        xnest_render_picture_get_priv(pMask);
    struct xnest_render_picture_privrec *pDstPriv =
        xnest_render_picture_get_priv(pDst);

    xcb_render_picture_t upstreamSrc = privId(pSrcPriv);
    xcb_render_picture_t upstreamMask = privId(pMaskPriv);
    xcb_render_picture_t upstreamDst = privId(pDstPriv);

    fprintf(stderr, "xnest_render_composite upstream src=0x%x mask=0x%x dst=0x%x\n",
        upstreamSrc, upstreamMask, upstreamDst);

    xcb_render_composite(
        xnestUpstreamInfo.conn,
        op,
        upstreamSrc,
        upstreamMask,
        upstreamDst,
        src_x,
        src_y,
        mask_x,
        mask_y,
        dst_x,
        dst_y,
        width,
        height);
}

static void xnest_render_add_traps(
    PicturePtr pPicture, int16_t xOff, int16_t yOff, int ntrap,
    xTrap *traps)
{
    struct xnest_render_picture_privrec *pPicturePriv =
        xnest_render_picture_get_priv(pPicture);

    fprintf(stderr, "xnest_render_add_traps\n");
}

static void xnest_render_rasterize_trapezoid(
    PicturePtr alpha, xTrapezoid *trap, int x_off, int y_off)
{
    struct xnest_render_picture_privrec *alphaPriv =
        xnest_render_picture_get_priv(alpha);

    fprintf(stderr, "xnest_render_rasterize_trapezoid\n");
}

static void xnest_render_add_triangles(
    PicturePtr pPicture, int16_t xOff, int16_t yOff, int ntri, xTriangle *tris)
{
    struct xnest_render_picture_privrec *pPicturePriv =
        xnest_render_picture_get_priv(pPicture);

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

static int xnest_render_create_picture(PicturePtr pPicture)
{
    assert(pPicture);

    struct xnest_render_picture_privrec *pPicturePriv = 
        xnest_render_picture_get_priv(pPicture);
    assert(pPicturePriv);

    fprintf(stderr, "xnest_render_create_picture\n");

    xcb_render_create_picture_value_list_t params = { 
        .repeat = pPicture->repeat,
        .alphamap = 0, // convert
        .alphaxorigin = pPicture->alphaOrigin.x,
        .alphayorigin = pPicture->alphaOrigin.y,
        .clipxorigin = pPicture->clipOrigin.x,
        .clipyorigin = pPicture->clipOrigin.y,
        .clipmask = 0, // convert
        .graphicsexposure = pPicture->graphicsExposures,
        .subwindowmode = pPicture->subWindowMode,
        .polyedge = pPicture->polyEdge,
        .polymode = pPicture->polyMode,
        .dither = 0, // convert
        .componentalpha = 0, // convert
    };

    pPicturePriv->upstream_xid = xcb_generate_id(xnestUpstreamInfo.conn);
    fprintf(stderr, "create_picture: XID=0x%x\n", pPicturePriv->upstream_xid);

    xcb_render_create_picture_aux_checked(
        xnestUpstreamInfo.conn,
        pPicturePriv->upstream_xid,
        /* xcb_drawable_t 	drawable, */0,
        /* xcb_render_pictformat_t 	format, */ 0,
        (XCB_RENDER_CP_REPEAT | XCB_RENDER_CP_ALPHA_MAP |
         XCB_RENDER_CP_ALPHA_X_ORIGIN | XCB_RENDER_CP_ALPHA_Y_ORIGIN |
         XCB_RENDER_CP_CLIP_X_ORIGIN | XCB_RENDER_CP_CLIP_Y_ORIGIN |
         XCB_RENDER_CP_CLIP_MASK | XCB_RENDER_CP_GRAPHICS_EXPOSURE |
         XCB_RENDER_CP_SUBWINDOW_MODE | XCB_RENDER_CP_POLY_EDGE |
         XCB_RENDER_CP_POLY_MODE | XCB_RENDER_CP_DITHER |
         XCB_RENDER_CP_COMPONENT_ALPHA),
        &params
    );

    // FIXME: do we need that ?
    return miCreatePicture(pPicture);
}

static void xnest_render_destroy_picture(PicturePtr pPicture)
{
    fprintf(stderr, "xnest_render_destroy_picture\n");
    miDestroyPicture(pPicture);
}

Bool xnest_picture_init(ScreenPtr pScreen)
{
    if (!miPictureInit(pScreen, 0, 0))
        return FALSE;

    if (!dixRegisterPrivateKey(&xnestPicturePrivateKey,
                               PRIVATE_PICTURE,
                               sizeof(struct xnest_render_picture_privrec))) {
        fprintf(stderr, "failed to allocate PRIVATE_PICTURE\n");
        return FALSE;
    }

    PictureScreenPtr ps = GetPictureScreen(pScreen);
    ps->CreatePicture = xnest_render_create_picture;
    ps->DestroyPicture = xnest_render_destroy_picture;
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
