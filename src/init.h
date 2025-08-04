#ifndef __CBUI_INIT_H__
#define __CBUI_INIT_H__


#define IMG_BUFF_CHANNELS 4
#define IMG_BUFF_MAX_WIDTH 3840
#define IMG_BUFF_MAX_HEIGHT 2160


//
//  Sprite render / control buffer API


Array<Frame> g_sprite_buffer;


void SpriteBufferInit(MArena *a_dest, u32 max_quads = 2048) {
    g_sprite_buffer = InitArray<Frame>(a_dest, max_quads);
}

void SpriteBufferPush(Frame sprite) {
    g_sprite_buffer.Add(sprite);
}

void SpriteBufferBlitAndClear(HashMap map_textures, s32 dest_width, s32 dest_height, u8 *dest_buffer) {

    for (s32 i = 0; i < g_sprite_buffer.len; ++i) {
        Frame s = g_sprite_buffer.arr[i];

        Texture *texture = (Texture*) MapGet(&map_textures, s.tex_id);

        if (texture->tpe == TT_8BIT) {
            // blit 8B sprite in the given color
            ImageB _texture = { texture->width, texture->height, texture->data };
            ImageRGBA _dest = { texture->width, texture->height, (Color*) texture->data };

            BlitGlyph2(s.w, s.h, s.x0, s.y0, s.u0, s.u1, s.v0, s.v1, s.color, _texture, _dest);
        }

    }

    /*
    void BlitQuads(Array<Quad> quads, HashMap *map_textues, ImageRGBA img) {
        for (u32 i = 0; i < quads.len; ++i) {
            Quad *q = quads.arr + i;

            s32 q_w = round( q->GetWidth() );
            s32 q_h = round( q->GetHeight() );
            s32 q_x0 = round( q->GetX0() );
            s32 q_y0 = round( q->GetY0() );
            u64 q_texture = q->GetTextureId();
            Color q_color = q->GetColor();

            // TODO: impl. robust versions to be able to blit a larger quad into the "smaller" window
            assert(img.height >= q_h);
            assert(img.width >= q_w);

            void *texture = (void*) MapGet(map_textues, q_texture);

            // byte-texture / glyphs
            if (q_texture != 0 && q_color.IsNonZero()) {
                f32 q_scale_x = q->GetTextureScaleX(q_w);
                f32 q_scale_y = q->GetTextureScaleY(q_h);
                f32 q_u0 = q->GetTextureU0();
                f32 q_v0 = q->GetTextureV0();

                BlitGlyph(q_w, q_h, q_x0, q_y0, q_u0, q_v0, q_scale_x, q_scale_y, q_color, (ImageB*) texture, img);
            }

            // mono-color quads
            else if (q_texture == 0 && q_color.IsNonZero()) {
                BlitMonoColor(q_w, q_h, q_x0, q_y0, q_color, img);
            }
        }
    }
    */

    g_sprite_buffer.len = 0;
}


//
//  UI core state variables


struct CbuiState {
    MContext *ctx;
    u64 frameno;
    u64 dts[8];
    u64 t_framestart;
    u64 t_framestart_prev;
    f32 dt;
    f32 fr;
    bool running;

    Array<Quad> quad_buffer;
    HashMap map_textures;
    HashMap map_fonts;

    u8 *image_buffer;
    PlafGlfw plf;

    f32 TimeSince(f32 t) {
        return t_framestart - t; 
    }
};

static CbuiState cbui;

CbuiState *CbuiInit(const char *title, bool start_in_fullscreen) {
    MContext *ctx = InitBaselayer();

    cbui = {};
    cbui.running = true;
    cbui.image_buffer = (u8*) ArenaAlloc(ctx->a_life, IMG_BUFF_CHANNELS * IMG_BUFF_MAX_WIDTH * IMG_BUFF_MAX_HEIGHT);
    cbui.ctx = ctx;
    PlafGlfwInit(&cbui.plf, title, 640, 480, cbui.image_buffer);
    cbui.t_framestart = ReadSystemTimerMySec();
    cbui.t_framestart_prev = cbui.t_framestart;

    UI_Init(cbui.plf.width, cbui.plf.height, &cbui.frameno);
    QuadBufferInit(cbui.ctx->a_life);
    SpriteBufferInit(cbui.ctx->a_life);

    cbui.map_fonts = InitMap(cbui.ctx->a_life, MAX_RESOURCE_CNT);
    g_font_map = &cbui.map_fonts;
    cbui.map_textures = InitMap(cbui.ctx->a_life, MAX_RESOURCE_CNT);

    // load & check resource file
    ResourceStreamHandle hdl = ResourceStreamLoadAndOpen(cbui.ctx->a_tmp, cbui.ctx->a_life, "all.res");

    // map out the resources
    ResourceHdr *res = hdl.first;
    while (res) {
        // fonts
        if (res->tpe == RST_FONT) {
            FontAtlas *font = FontAtlasLoadBinaryStream(res->GetInlinedData(), res->data_sz);
            if (false) { font->Print(); }

            MapPut(&cbui.map_fonts, font->GetKey(), font);
            MapPut(&cbui.map_textures, font->GetKey(), &font->texture);
        }


        // sprite maps
        // TODO: do something else // load each sprite map individually
        /*
        else if (res->tpe == RST_SPRITE) {
            SpriteMap *smap = SpriteMapLoadStream((u8*) res->GetInlinedData(), res->data_sz);
            if (false) {

                printf("sprite map: %s, %s, count: %u, atlas w: %u, atlas h: %u\n", smap->map_name, smap->key_name, smap->sprites.len, smap->texture.width, smap->texture.height);
            }

            MapPut(&g_resource_map, smap->GetKey(), smap);
            MapPut(&g_texture_map, smap->GetKey(), &smap->texture);
        }
        */


        // other
        else {
            printf("WARN: unknown resource detected\n");
        }

        // iter
        res = res->GetInlinedNext();
    }
    SetFontAndSize(FS_30, hdl.names[RST_FONT]->GetStr());

    if (start_in_fullscreen) { PlafGlfwToggleFullscreen(&cbui.plf); }

    return &cbui;
}


#define FR_RUNNING_AVG_COUNT 4
void CbuiFrameStart() {
    ArenaClear(cbui.ctx->a_tmp);
    memset(cbui.image_buffer, 255, IMG_BUFF_CHANNELS * cbui.plf.width * cbui.plf.height);

    cbui.t_framestart = ReadSystemTimerMySec();
    cbui.dt = (cbui.t_framestart - cbui.t_framestart_prev) / 1000;
    cbui.dts[cbui.frameno % FR_RUNNING_AVG_COUNT] = cbui.dt;

    f32 sum = 0;
    for (s32 i = 0; i < FR_RUNNING_AVG_COUNT; ++i) { sum += cbui.dts[i]; }
    f32 dt_avg = sum / FR_RUNNING_AVG_COUNT;
    cbui.fr = 1.0f / dt_avg * 1000;
    cbui.t_framestart_prev = cbui.t_framestart;

    cbui.frameno++;
}

void CbuiFrameEnd() {
    XSleep(1);

    PlafGlfwUpdate(&cbui.plf);
    UI_FrameEnd(cbui.ctx->a_tmp, cbui.plf.width, cbui.plf.height, cbui.plf.cursorpos.x, cbui.plf.cursorpos.y, cbui.plf.left.ended_down, cbui.plf.left.pushed);

    QuadBufferBlitAndClear(&cbui.map_textures, InitImageRGBA(cbui.plf.width, cbui.plf.height, cbui.image_buffer));
    SpriteBufferBlitAndClear(cbui.map_textures, cbui.plf.width, cbui.plf.height, cbui.image_buffer);
    PlafGlfwPushBuffer(&cbui.plf);

    cbui.running = cbui.running && !GetEscape() && !GetWindowShouldClose(&cbui.plf);
}

void CbuiExit() {
    PlafGlfwTerminate(&cbui.plf);
}


#endif
