#include <dix-config.h>

#include <stdint.h>

#include <X11/X.h>
#include <X11/Xdefs.h>
#include <X11/Xproto.h>
#include <xcb/render.h>

#include "render/mipict.h"

#include "xnest-xcb.h"

/*** glyph private data **/
struct xnest_render_glyph_privrec {
    xcb_render_glyph_t upstream_xid;
    struct xnest_upstream_info *upstream_info;
};

DevPrivateKeyRec xnestGlyphPrivateKey;

static inline struct xnest_render_glyph_privrec *
    xnest_render_glyph_get_priv(GlyphPtr glyph)
{
    if (glyph == NULL)
        return NULL;

    return dixLookupPrivate(&glyph->devPrivates, &xnestGlyphPrivateKey);
}

static inline Bool xnest_render_register_glyph_priv(void)
{
    if (!dixRegisterPrivateKey(&xnestGlyphPrivateKey,
                               PRIVATE_PICTURE,
                               sizeof(struct xnest_render_glyph_privrec))) {
        fprintf(stderr, "failed to allocate PRIVATE_PICTURE\n");
        return FALSE;
    }
    return TRUE;
}

/*** picture private data ***/
struct xnest_render_picture_privrec {
    xcb_render_picture_t upstream_xid;
    struct xnest_upstream_info *upstream_info;
};

DevPrivateKeyRec xnestPicturePrivateKey;

static inline struct xnest_render_picture_privrec *
    xnest_render_picture_get_priv(PicturePtr pPict)
{
    if (pPict == NULL)
        return NULL;

    return dixLookupPrivate(&pPict->devPrivates, &xnestPicturePrivateKey);
}

static inline Bool xnest_render_register_picture_priv(void)
{
    if (!dixRegisterPrivateKey(&xnestPicturePrivateKey,
                               PRIVATE_PICTURE,
                               sizeof(struct xnest_render_picture_privrec))) {
        fprintf(stderr, "failed to allocate PRIVATE_PICTURE\n");
        return FALSE;
    }
    return TRUE;
}

static inline xcb_render_picture_t privId(struct xnest_render_picture_privrec *p)
{
    return p ? p->upstream_xid : 0;
}

/* **** */

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
        pSrcPriv->upstream_info->conn,
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

// TODO
static void xnest_render_add_traps(
    PicturePtr pPicture, int16_t xOff, int16_t yOff, int ntrap,
    xTrap *traps)
{
    struct xnest_render_picture_privrec *pPicturePriv =
        xnest_render_picture_get_priv(pPicture);

    fprintf(stderr, "FIXME: xnest_render_add_traps\n");
}

// TODO
static void xnest_render_rasterize_trapezoid(
    PicturePtr alpha, xTrapezoid *trap, int x_off, int y_off)
{
    struct xnest_render_picture_privrec *alphaPriv =
        xnest_render_picture_get_priv(alpha);

    fprintf(stderr, "xnest_render_rasterize_trapezoid\n");
}

// TODO
static void xnest_render_add_triangles(
    PicturePtr pPicture, int16_t xOff, int16_t yOff, int ntri, xTriangle *tris)
{
    struct xnest_render_picture_privrec *pPicturePriv =
        xnest_render_picture_get_priv(pPicture);

    fprintf(stderr, "xnest_render_add_triangles\n");
}

// TODO
static void xnest_render_trapezoids(
    uint8_t op, PicturePtr pSrc, PicturePtr pDst, PictFormatPtr maskFormat,
    int16_t xSrc, int16_t ySrc, int ntrap, xTrapezoid *traps)
{
    fprintf(stderr, "xnest_render_trapezoids\n");
}

// TODO
static void xnest_render_triangles(
    uint8_t op, PicturePtr pSrc, PicturePtr pDst, PictFormatPtr maskFormat,
    int16_t xSrc, int16_t ySrc, int ntris, xTriangle *tris)
{
    fprintf(stderr, "xnest_render_triangles\n");
}

// TODO
static void xnest_render_unrealize_glyph(ScreenPtr pScreen, GlyphPtr glyph)
{
    fprintf(stderr, "xnest_render_unrealize_glyph\n");
    assert(pScreen);
    assert(glyph);
}

// TODO
static Bool xnest_render_realize_glyph(ScreenPtr pScreen, GlyphPtr glyph)
{
    fprintf(stderr, "xnest_render_realize_glyph\n");
    assert(pScreen);
    assert(glyph);

//    struct 
    return TRUE;
}

// TODO
static void xnest_render_glyphs(
    uint8_t op, PicturePtr pSrc, PicturePtr pDst, PictFormatPtr maskFormat,
    int16_t xSrc, int16_t ySrc, int nlists, GlyphListPtr lists, GlyphPtr *glyphs)
{
    fprintf(stderr, "FIXME: xnest_render_glyphs\n");
    if (maskFormat)
        fprintf(stderr, "--> have maskFormat\n");
    if (lists) {
        fprintf(stderr, "--> have lists --> num=%d\n", nlists);
        for (int i=0; i<nlists; i++) {
            fprintf(stderr, "glyph list ent %d: offset=(%d:%d) len=%d\n",
                i, lists[i].xOff, lists[i].yOff, lists[i].len);
        }
    }

    if (glyphs)
        fprintf(stderr, "have pointer to glyphptr\n");
}

// TODO
static int xnest_render_create_picture(PicturePtr pPicture)
{
    assert(pPicture);

    struct xnest_render_picture_privrec *pPicturePriv =
        xnest_render_picture_get_priv(pPicture);
    assert(pPicturePriv);

    // FIXME: should be fetched from screen ?
    pPicturePriv->upstream_info = &xnestUpstreamInfo;

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

    pPicturePriv->upstream_xid = xcb_generate_id(pPicturePriv->upstream_info->conn);
    fprintf(stderr, "create_picture: XID=0x%x\n", pPicturePriv->upstream_xid);

    // FIXME: need to fetch drawable / window XID
    // FIXME: need to fetch formats --> QueryPictFormats
    xcb_render_create_picture_aux_checked(
        pPicturePriv->upstream_info->conn,
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
    assert(pPicture);

    struct xnest_render_picture_privrec *pPicturePriv =
        xnest_render_picture_get_priv(pPicture);
    assert(pPicturePriv);

    xcb_render_free_picture_checked(
        pPicturePriv->upstream_info->conn,
        pPicturePriv->upstream_xid
    );
    pPicturePriv->upstream_xid = XCB_NONE;

    miDestroyPicture(pPicture);
}

Bool xnest_picture_init(ScreenPtr pScreen)
{
    if (!(miPictureInit(pScreen, 0, 0) && xnest_render_register_glyph_priv()
                                       && xnest_render_register_picture_priv()))
        return FALSE;

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
    ps->RealizeGlyph = xnest_render_realize_glyph;

    return TRUE;
}
