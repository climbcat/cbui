#ifndef __SPRITE_H__
#define __SPRITE_H__


struct Sprite {
    s32 w;
    s32 h;
    f32 u0;
    f32 u1;
    f32 v0;
    f32 v1;
};

inline
void PrintSprite(Sprite s) {
    printf("sprite w: %d, h: %d u0: %.4f, u1: %.4f, v0: %.4f, v1: %.4f\n", s.w, s.h, s.u0, s.u1, s.v0, s.v1);
}


//
//  Quads


struct QuadVertex {  // vertex layout
    Vector2f pos;
    Vector2f tex;

    // TODO: we can pack this by using just some flags or bytes that refer 
    //      to (pre-packated / matching / uniform) values
    //      In which case we should rename these to 
    //          u_texture_idx
    //          u_color_idx
    //      or some such.

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

struct QuadHexaVertex { // renderable six-vertex quad
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
        for (u32 i = 0; i < 6; ++i) {
            verts[i].col = color;
        }
    }
    inline
    s32 GetWidth() {
        s32 x0 = (s32) verts[2].pos.x;
        s32 x1 = (s32) verts[0].pos.x;
        s32 width = x1 - x0;
        return width;
    }
    inline
    s32 GetHeight() {
        s32 y0 = (s32) verts[0].pos.y;
        s32 y1 = (s32) verts[2].pos.y;
        s32 width = y1 - y0;
        return width;
    }
    inline
    s32 GetX0() {
        s32 x0 = (s32) verts[2].pos.x;
        return x0;
    }
    inline
    s32 GetY0() {
        s32 y0 = (s32) verts[0].pos.y;
        return y0;
    }
    inline
    s32 GetX1() {
        s32 x1 = (s32) verts[0].pos.x;
        return x1;
    }
    inline
    s32 GetY1() {
        s32 y1 = (s32) verts[1].pos.y;
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

QuadHexaVertex QuadCookSolid(s32 w, s32 h, s32 x0, s32 y0, Color c) {
    // lays down two three-vertex triangles: T1 = [ urc->lrc->llc ] and T2 = [ llc->ulc->urc ]
    // ulc: upper-left corner (etc.)
    QuadHexaVertex qh = {};
    s32 x1 = x0 + w;
    s32 y1 = y0 + h;

    Vector2f ulc_pos { (f32) x0, (f32) y0 };
    Vector2f urc_pos { (f32) x1, (f32) y0 };
    Vector2f lrc_pos { (f32) x1, (f32) y1 };
    Vector2f llc_pos { (f32) x0, (f32) y1 };

    qh.verts[0] = InitQuadVertex( urc_pos, { 0, 0 }, c, 0 );
    qh.verts[1] = InitQuadVertex( lrc_pos, { 0, 0 }, c, 0 );
    qh.verts[2] = InitQuadVertex( llc_pos, { 0, 0 }, c, 0 );
    qh.verts[3] = InitQuadVertex( llc_pos, { 0, 0 }, c, 0 );
    qh.verts[4] = InitQuadVertex( ulc_pos, { 0, 0 }, c, 0 );
    qh.verts[5] = InitQuadVertex( urc_pos, { 0, 0 }, c, 0 );

    return qh;
}

QuadHexaVertex QuadCookTextured(Sprite s, s32 x0, s32 y0, u64 texture_id) {
    // lays down two three-vertex triangles: T1 = [ urc->lrc->llc ] and T2 = [ llc->ulc->urc ]
    // ulc: upper-left corner (etc.)

    QuadHexaVertex qh = {};
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
QuadHexaVertex QuadOffset(QuadHexaVertex *q, s16 x, s16 y, Color color, u64 texture_id) {
    QuadHexaVertex out = {};
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
//  SpriteMap


inline
Color SampleTextureRGBA(ImageRGBA *tex, f32 x, f32 y) {
    s32 i = (s32) round(tex->width * x);
    s32 j = (s32) round(tex->height * y);
    u32 idx = tex->width * j + i;
    Color b = tex->img[idx];
    return b;
}
inline
Color SampleTextureRGBASafe(ImageRGBA *tex, f32 x, f32 y, Color col_default) {
    s32 i = (s32) round(tex->width * x);
    s32 j = (s32) round(tex->height * y);
    if (i < 0 || i >= tex->width || j < 0 || j >= tex->height) {
        return col_default;
    }
    u32 idx = tex->width * j + i;
    Color b = tex->img[idx];
    return b;
}
void BlitSprite(Sprite s, s32 x0, s32 y0, ImageRGBA *img_dest, ImageRGBA *img_src) {
    s32 q_w = s.w;
    s32 q_h = s.h;
    s32 q_x0 = x0;
    s32 q_y0 = y0;

    assert(img_dest->height >= q_w);
    assert(img_dest->width >= q_h);

    u32 stride_img = img_dest->width;

    f32 q_scale_x = (s.u1 - s.u0) / q_w;
    f32 q_scale_y = (s.v1 - s.v0) / q_h;
    f32 q_u0 = s.u0;
    f32 q_v0 = s.v0;

    // i,j          : target coords
    // i_img, j_img : img coords

    for (s32 j = 0; j < q_h; ++j) {
        s32 j_img = j + q_y0;
        if (j_img < 0 || j_img > img_dest->height) {
            continue;
        }

        for (s32 i = 0; i < q_w; ++i) {
            s32 i_img = q_x0 + i;
            if (i_img < 0 || i_img > img_dest->width) {
                continue;
            }
            f32 x = q_u0 + i * q_scale_x;
            f32 y = q_v0 + j * q_scale_y;

            // TODO: how do we regularize this code?
            Color color_src = SampleTextureRGBASafe(img_src, x, y, Color { 0, 0, 0, 255 });

            if (color_src.a != 0) {
                // rudimentary alpha-blending
                s32 idx = j_img * stride_img + i_img;
                Color color_background = img_dest->img[idx];

                f32 alpha = (1.0f * color_src.a) / 255;
                Color color_blended;
                color_blended.r = (u8) (floor( alpha*color_src.r ) + floor( (1-alpha)*color_background.r ));
                color_blended.g = (u8) (floor( alpha*color_src.g ) + floor( (1-alpha)*color_background.g ));
                color_blended.b = (u8) (floor( alpha*color_src.b ) + floor( (1-alpha)*color_background.b ));
                color_blended.a = 255;

                img_dest->img[idx] = color_blended;
            }
        }
    }
}


struct SpriteMap {
    u32 size_tot;
    char map_name[32];
    char key_name[32];
    List<Sprite> sprites;
    ImageRGBA texture;
    u64 GetKey() {
        return HashStringValue(key_name);
    }

};

SpriteMap *SpriteMapLoadStream(u8 *base_ptr, u32 data_sz) {
    SpriteMap *smap = (SpriteMap*) base_ptr;
    smap->sprites.lst = (Sprite*) (base_ptr + sizeof(SpriteMap));
    smap->texture.img = (Color*) (base_ptr + sizeof(SpriteMap) + smap->sprites.len * sizeof(Sprite));

    assert(data_sz = sizeof(SpriteMap) + smap->sprites.len * sizeof(Sprite) + sizeof(Color) * smap->texture.width * smap->texture.height && "sanity check sprite map data size");
    return smap;
}


SpriteMap *CompileSpriteMapInline(MArena *a_dest, const char *name, const char *key_name, List<Sprite> sprites, List<u32> tex_keys, HashMap *texture_map) {
    s16 nx = (s16) floor( (f32) sqrt(sprites.len) );
    s32 ny = sprites.len / nx + 1;
    assert(nx >= 0 && sprites.len <= (u32) (nx * ny));

    printf("compiling sprite map;\n    name: %s\n    key_name: %s\n", name, key_name);
    printf("sprites count x: %u, y: %u \n", nx, ny);

    // calc bitmap size
    s32 bm_w = 0;
    s32 bm_h = 0;
    for (s32 j = 0; j < ny; ++j) {
        s32 row_w = 0;
        s32 row_max_h = 0;
        for (s32 i = 0; i < nx; ++i) {
            u32 idx = (u32) (i + j*nx);
            if (idx < sprites.len) {
                Sprite s = sprites.lst[idx];
                row_w += s.w;
                row_max_h = MaxS32(row_max_h, s.h);
            }
        }

        bm_w = MaxS32(bm_w, row_w);
        bm_h += row_max_h;
    }
    printf("comp. bitmap size: %d %d\n", bm_w, bm_h);

    // alloc sprite map memory
    SpriteMap *smap = (SpriteMap*) ArenaAlloc(a_dest, sizeof(SpriteMap));
    sprintf(smap->map_name, "%s", name);
    sprintf(smap->key_name, "%s", key_name);
    smap->sprites.len = sprites.len;
    smap->sprites.lst = (Sprite*) ArenaAlloc(a_dest, sizeof(Sprite) * sprites.len);
    smap->texture.width = bm_w;
    smap->texture.height = bm_h;
    smap->texture.img = (Color*) ArenaAlloc(a_dest, sizeof(Color) * bm_w * bm_h);
    smap->size_tot = sizeof(SpriteMap) + sizeof(Sprite) * sprites.len + sizeof(Color) * bm_w * bm_h;

    // copy data to sprites & texture
    s32 x = 0;
    s32 y = 0;
    for (s32 j = 0; j < ny; ++j) {
        s32 row_max_h = 0;
        for (s32 i = 0; i < nx; ++i) {
            u32 idx = (u32) (i + j*nx);
            if (idx < sprites.len) {
                Sprite s = sprites.lst[idx];
                ImageRGBA *texture = (ImageRGBA*) MapGet(texture_map, tex_keys.lst[idx]);
                BlitSprite(s, x, y, &smap->texture, texture);

                Sprite t = s;
                t.u0 = 1.0f * x / bm_w;
                t.u1 = (1.0f*x + t.w) / bm_w;
                t.v0 = 1.0f*y / bm_h;
                t.v1 = (1.0f*y + t.h) / bm_h;
                smap->sprites.lst[idx] = t;

                x += t.w;
                row_max_h = MaxS32(row_max_h, t.h);
            }
        }
        x = 0;
        y += row_max_h;

        printf("row_h: %d\n", row_max_h);
    }

    return smap;
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
    List<QuadHexaVertex> quads;
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

void BlitQuads(Array<QuadHexaVertex> quads, ImageRGBA *img) {


    // TODO: Don't blit a drawcall, just blit the frekkin quads directly:
    //      We can do the type switch below from raw quad data (as OGL would).


    // get the texture


    for (u32 i = 0; i < quads.len; ++i) {
        QuadHexaVertex *q = quads.arr + i;

        s32 q_w = q->GetWidth();
        s32 q_h = q->GetHeight();
        s32 q_x0 = q->GetX0();
        s32 q_y0 = q->GetY0();
        u64 q_texture = q->GetTextureId();
        Color q_color = q->GetColor();

        assert(img->height >= q_h);
        assert(img->width >= q_w);

        u32 stride_img = img->width;

        void *texture = GetTexture(q_texture);
        ImageB *texture_b = (ImageB*) texture;
        ImageRGBA *texture_rgba = (ImageRGBA*) texture;


        //
        // byte-texture / glyphs
        //
        if (q_texture != 0 && q_color.IsNonZero()) {
            assert(texture_b != NULL);

            f32 q_scale_x = q->GetTextureScaleX(q_w);
            f32 q_scale_y = q->GetTextureScaleY(q_h);
            f32 q_u0 = q->GetTextureU0();
            f32 q_v0 = q->GetTextureV0();

            // i,j          : target coords
            // i_img, j_img : img coords

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

        //
        // mono-color quads
        //
        else if (q_texture == 0 && q_color.IsNonZero()) {
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

        //
        // blit 32bit texture
        //
        else if (q_texture != 0 && q_color.IsZero()) {
            assert(texture_rgba != NULL);

            // TODO: integrate
            Sprite s = {};
            s.w = q_w;
            s.h = q_h;
            s.u0 = q->GetTextureU0();
            s.u1 = q->GetTextureU1();
            s.v0 = q->GetTextureV0();
            s.v1 = q->GetTextureV1();
            BlitSprite(s, q_x0, q_y0, img, texture_rgba);
        }
    }
}


//
// sprite render API (hides the drawcall buffer)


static Array<QuadHexaVertex> g_quad_buffer;
void SpriteRender_Init(MArena *a_life, u32 max_quads = 2048) {
    g_quad_buffer = InitArray<QuadHexaVertex>(a_life, max_quads);
}
void SpriteRender_PushDrawCall(DrawCall dc) {
    // TODO: do something with this for OGL
}
void SpriteRender_PushQuad(QuadHexaVertex quad) {
    g_quad_buffer.Add(quad);
}
void SpriteRender_BlitAndCLear(ImageRGBA render_target) {
    BlitQuads(g_quad_buffer, &render_target);
    g_quad_buffer.len = 0;
}


#endif
