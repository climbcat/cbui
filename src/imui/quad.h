#ifndef __QUAD_H__
#define __QUAD_H__


//
//  Quads


struct QuadVertex {
    Vector2f pos;
    Vector2f tex;
    u64 tex_id;
    Color col;
};


inline
QuadVertex InitQuadVertex(Vector2f pos, Vector2f tex, Color color, u64 texture_id) {
    QuadVertex v = {};
    v.pos = pos;
    v.tex = tex;
    v.col = color;
    v.tex_id = texture_id;
    return v;
}

struct Quad { // renderable six-vertex quad
    QuadVertex verts[6];

    inline
    u64 GetTextureId() {
        return verts[0].tex_id;
    }
    inline
    Color GetColor() {
        return verts[0].col;
    }
    inline
    void SetColor(Color color) {
        for (s32 i = 0; i < 6; ++i) {
            verts[i].col = color;
        }
    }
    inline
    f32 GetWidth() {
        f32 x0 = verts[2].pos.x;
        f32 x1 = verts[0].pos.x;
        f32 width = x1 - x0;
        return width;
    }
    inline
    f32 GetHeight() {
        f32 y0 = verts[0].pos.y;
        f32 y1 = verts[2].pos.y;
        f32 width = y1 - y0;
        return width;
    }
    inline
    f32 GetX0() {
        f32 x0 = verts[2].pos.x;
        return x0;
    }
    inline
    f32 GetY0() {
        f32 y0 = verts[0].pos.y;
        return y0;
    }
    inline
    f32 GetX1() {
        f32 x1 = verts[0].pos.x;
        return x1;
    }
    inline
    f32 GetY1() {
        f32 y1 = verts[1].pos.y;
        return y1;
    }
    inline
    f32 GetTextureScaleX(s32 dest_width) {
        f32 u0 = verts[2].tex.x;
        f32 u1 = verts[0].tex.x;
        f32 scale_x = (u1 - u0) / dest_width;
        return scale_x;
    }
    inline
    f32 GetTextureScaleY(s32 dest_height) {
        f32 v0 = verts[0].tex.y;
        f32 v1 = verts[2].tex.y;
        f32 scale_y = (v1 - v0) / dest_height;
        return scale_y;
    }
    inline
    f32 GetTextureU0() {
        f32 u0 = verts[2].tex.x;
        return u0;
    }
    inline
    f32 GetTextureU1() {
        f32 u1 = verts[0].tex.x;
        return u1;
    }
    inline
    f32 GetTextureV0() {
        f32 v0 = verts[0].tex.y;
        return v0;
    }
    inline
    f32 GetTextureV1() {
        f32 v1 = verts[2].tex.y;
        return v1;
    }
    f32 GetTextureWidth() {
        f32 u0 = verts[2].tex.x;
        f32 u1 = verts[0].tex.x;
        return u1 - u0;
    }
    inline
    f32 GetTextureHeight() {
        f32 v0 = verts[0].tex.y;
        f32 v1 = verts[2].tex.y;
        return v1 - v0;
    }
};

Quad QuadSolid(f32 w, f32 h, f32 x0, f32 y0, Color c) {
    // lays down two three-vertex triangles: T1 = [ urc->lrc->llc ] and T2 = [ llc->ulc->urc ]
    // ulc: upper-left corner (etc.)
    Quad qh = {};
    f32 x1 = x0 + w;
    f32 y1 = y0 + h;

    Vector2f ulc_pos { x0, y0 };
    Vector2f urc_pos { x1, y0 };
    Vector2f lrc_pos { x1, y1 };
    Vector2f llc_pos { x0, y1 };

    qh.verts[0] = InitQuadVertex( urc_pos, { 0, 0 }, c, 0 );
    qh.verts[1] = InitQuadVertex( lrc_pos, { 0, 0 }, c, 0 );
    qh.verts[2] = InitQuadVertex( llc_pos, { 0, 0 }, c, 0 );
    qh.verts[3] = InitQuadVertex( llc_pos, { 0, 0 }, c, 0 );
    qh.verts[4] = InitQuadVertex( ulc_pos, { 0, 0 }, c, 0 );
    qh.verts[5] = InitQuadVertex( urc_pos, { 0, 0 }, c, 0 );

    return qh;
}

Quad QuadTextured(Sprite s, s32 x0, s32 y0, u64 texture_id) {
    // lays down two three-vertex triangles: T1 = [ urc->lrc->llc ] and T2 = [ llc->ulc->urc ]
    // ulc: upper-left corner (etc.)

    Quad qh = {};
    s32 x1 = x0 + s.w;
    s32 y1 = y0 + s.h;

    Vector2f ulc_pos { (f32) x0, (f32) y0 };
    Vector2f urc_pos { (f32) x1, (f32) y0 };
    Vector2f lrc_pos { (f32) x1, (f32) y1 };
    Vector2f llc_pos { (f32) x0, (f32) y1 };

    Vector2f ulc_tex { (f32) s.u0, (f32) s.v0 };
    Vector2f urc_tex { (f32) s.u1, (f32) s.v0 };
    Vector2f lrc_tex { (f32) s.u1, (f32) s.v1 };
    Vector2f llc_tex { (f32) s.u0, (f32) s.v1 };

    Color no_color = { 0, 0, 0, 255 };

    qh.verts[0] = InitQuadVertex( urc_pos, urc_tex, no_color, texture_id );
    qh.verts[1] = InitQuadVertex( lrc_pos, lrc_tex, no_color, texture_id );
    qh.verts[2] = InitQuadVertex( llc_pos, llc_tex, no_color, texture_id );
    qh.verts[3] = InitQuadVertex( llc_pos, llc_tex, no_color, texture_id );
    qh.verts[4] = InitQuadVertex( ulc_pos, ulc_tex, no_color, texture_id );
    qh.verts[5] = InitQuadVertex( urc_pos, urc_tex, no_color, texture_id );

    return qh;
}

inline
Quad QuadOffset(Quad *q, s16 x, s16 y, Color color, u64 texture_id) {
    Quad out = {};
    for (u32 i = 0; i < 6; ++i) {
        QuadVertex v = *(q->verts + i);
        v.pos.x += x;
        v.pos.y += y;
        v.col = color;
        v.tex_id = texture_id;
        out.verts[i] = v;
    }
    return out;
}


//
//  DrawCall
//  Blit


enum DrawCallType {
    DCT_UNDEF,
    DCT_SOLID,
    DCT_TEXTURE_BYTE,
    DCT_TEXTURE_RGBA,
    DCT_CNT
};


struct DrawCall {
    DrawCallType tpe;
    u64 texture_key;
    List<Quad> quads;
};


static HashMap g_texture_map;
void *GetTexture(u64 key) {
    void *result = (void*) MapGet(&g_texture_map, key);
    return result;
}

inline
u8 SampleTexture(ImageB *tex, f32 x, f32 y) {
    u32 i = (s32) round(tex->width * x);
    u32 j = (s32) round(tex->height * y);
    u32 idx = tex->width * j + i;
    u8 b = tex->img[idx];
    return b;
}

void BlitQuads(Array<Quad> quads, ImageRGBA *img) {

    for (u32 i = 0; i < quads.len; ++i) {
        Quad *q = quads.arr + i;

        s32 q_w = round( q->GetWidth() );
        s32 q_h = round( q->GetHeight() );
        s32 q_x0 = round( q->GetX0() );
        s32 q_y0 = round( q->GetY0() );
        u64 q_texture = q->GetTextureId();
        Color q_color = q->GetColor();

        assert(img->height >= q_h);
        assert(img->width >= q_w);

        u32 stride_img = img->width;

        void *texture = GetTexture(q_texture);
        ImageB *texture_b = (ImageB*) texture;
        ImageRGBA *texture_rgba = (ImageRGBA*) texture;


        // byte-texture / glyphs
        if (q_texture != 0 && q_color.IsNonZero()) {
            assert(texture_b != NULL);

            f32 q_scale_x = q->GetTextureScaleX(q_w);
            f32 q_scale_y = q->GetTextureScaleY(q_h);
            f32 q_u0 = q->GetTextureU0();
            f32 q_v0 = q->GetTextureV0();

            // i,j          : target coords
            // i_img, j_img : img coords

            // TODO: extract code into raster function

            for (s32 j = 0; j < q_h; ++j) {
                s32 j_img = j + q_y0;
                if (j_img < 0 || j_img > img->height) {
                    continue;
                }

                for (s32 i = 0; i < q_w; ++i) {
                    s32 i_img = q_x0 + i;
                    if (i_img < 0 || i_img > img->width) {
                        continue;
                    }
                    f32 x = q_u0 + i * q_scale_x;
                    f32 y = q_v0 + j * q_scale_y;
                    if (u8 alpha_byte = SampleTexture(texture_b, x, y)) {
                        // rudimentary alpha-blending
                        u32 idx = (u32) (j_img * stride_img + i_img);
                        Color color_background = img->img[idx];

                        f32 alpha = (1.0f * alpha_byte) / 255;
                        Color color_blended;
                        color_blended.r = (u8) (floor( alpha*q_color.r ) + floor( (1-alpha)*color_background.r ));
                        color_blended.g = (u8) (floor( alpha*q_color.g ) + floor( (1-alpha)*color_background.g ));
                        color_blended.b = (u8) (floor( alpha*q_color.b ) + floor( (1-alpha)*color_background.b ));
                        color_blended.a = 255;

                        img->img[idx] = color_blended;
                    }
                }
            }
        }

        // mono-color quads
        else if (q_texture == 0 && q_color.IsNonZero()) {

            // TODO: extract code into raster function

            s32 j_img;
            s32 i_img;
            u32 idx;
            for (s32 j = 0; j < q_h; ++j) {
                j_img = j + q_y0;
                if (j_img < 0 || j_img > img->height) {
                    continue;
                }

                for (s32 i = 0; i < q_w; ++i) {
                    i_img = q_x0 + i;
                    if (i_img < 0 || i_img > img->width) {
                        continue;
                    }

                    idx = j_img * stride_img + i_img;
                    img->img[idx] = q_color;
                }
            }
        }
    }
}


//
// sprite render API (hides buffer)


static Array<Quad> g_quad_buffer;
void QuadBufferInit(MArena *a_dest, u32 max_quads = 2048) {
    g_quad_buffer = InitArray<Quad>(a_dest, max_quads);
}

void QuadBufferPush(Quad quad) {
    g_quad_buffer.Add(quad);
}

void QuadBufferBlitAndClear(ImageRGBA render_target) {
    BlitQuads(g_quad_buffer, &render_target);
    g_quad_buffer.len = 0;
}


#endif
