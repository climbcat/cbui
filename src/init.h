#ifndef __CBUI_INIT_H__
#define __CBUI_INIT_H__


#define IMG_BUFF_CHANNELS 4
#define IMG_BUFF_MAX_WIDTH 3840
#define IMG_BUFF_MAX_HEIGHT 2160


struct CbuiState {
    MContext *ctx;
    PlafGlfw *plf;
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

    f32 TimeSince(f32 t) {
        return t_framestart - t; 
    }
};

static CbuiState *cbui;

CbuiState *CbuiInit(const char *title, bool start_in_fullscreen) {
    MContext *ctx = InitBaselayer();

    cbui = (CbuiState*) ArenaAlloc(ctx->a_life, sizeof(CbuiState));
    cbui->running = true;
    cbui->image_buffer = (u8*) ArenaAlloc(ctx->a_life, IMG_BUFF_CHANNELS * IMG_BUFF_MAX_WIDTH * IMG_BUFF_MAX_HEIGHT);
    cbui->ctx = ctx;
    cbui->plf = PlafGlfwInit(title, 640, 480, cbui->image_buffer);
    cbui->t_framestart = ReadSystemTimerMySec();
    cbui->t_framestart_prev = cbui->t_framestart;

    UI_Init(cbui->plf->width, cbui->plf->height, &cbui->frameno);

    ImageRGBA render_target = { (s32) cbui->plf->width, (s32) cbui->plf->height, (Color*) cbui->plf->image_buffer };
    QuadBufferInit(cbui->ctx->a_life);

    cbui->map_fonts = InitMap(cbui->ctx->a_life, MAX_RESOURCE_CNT);
    g_font_map = &cbui->map_fonts;
    cbui->map_textures = InitMap(cbui->ctx->a_life, MAX_RESOURCE_CNT);

    // load & check resource file
    ResourceStreamHandle hdl = ResourceStreamLoadAndOpen(cbui->ctx->a_tmp, cbui->ctx->a_life, "all.res");

    // map out the resources
    ResourceHdr *res = hdl.first;
    while (res) {
        // fonts
        if (res->tpe == RST_FONT) {
            FontAtlas *font = FontAtlasLoadBinaryStream(res->GetInlinedData(), res->data_sz);
            if (false) { font->Print(); }

            MapPut(&cbui->map_fonts, font->GetKey(), font);
            MapPut(&cbui->map_textures, font->GetKey(), &font->texture);
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
    SetFontAndSize(FS_48, hdl.names[RST_FONT]->GetStr());

    if (start_in_fullscreen) { PlafGlfwToggleFullscreen(cbui->plf); }

    return cbui;
}


#define FR_RUNNING_AVG_COUNT 4
void CbuiFrameStart() {
    ArenaClear(cbui->ctx->a_tmp);
    memset(cbui->image_buffer, 255, IMG_BUFF_CHANNELS * cbui->plf->width * cbui->plf->height);

    cbui->t_framestart = ReadSystemTimerMySec();
    cbui->dt = (cbui->t_framestart - cbui->t_framestart_prev) / 1000;
    cbui->dts[cbui->frameno % FR_RUNNING_AVG_COUNT] = cbui->dt;

    f32 sum = 0;
    for (s32 i = 0; i < FR_RUNNING_AVG_COUNT; ++i) { sum += cbui->dts[i]; }
    f32 dt_avg = sum / FR_RUNNING_AVG_COUNT;
    cbui->fr = 1.0f / dt_avg * 1000;
    cbui->t_framestart_prev = cbui->t_framestart;

    cbui->frameno++;
}

void CbuiFrameEnd() {
    XSleep(1);

    PlafGlfwUpdate(cbui->plf);
    UI_FrameEnd(cbui->ctx->a_tmp, cbui->plf->width, cbui->plf->height, cbui->plf->cursorpos.x, cbui->plf->cursorpos.y, cbui->plf->left.ended_down, cbui->plf->left.pushed);

    QuadBufferBlitAndClear(&cbui->map_textures, InitImageRGBA(cbui->plf->width, cbui->plf->height, cbui->image_buffer));
    PlafGlfwPushBuffer(cbui->plf);

    cbui->running = cbui->running && !GetEscape() && !GetWindowShouldClose(cbui->plf);
}

void CbuiExit() {
    PlafGlfwTerminate(cbui->plf);
}


#endif
