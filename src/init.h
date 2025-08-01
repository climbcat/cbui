#ifndef __CBUI_INIT_H__
#define __CBUI_INIT_H__


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

    f32 TimeSince(f32 t) {
        return t_framestart - t; 
    }
};

static CbuiState _g_cbui_state;
static CbuiState *cbui;

CbuiState *CbuiInit(const char *title, bool start_in_fullscreen) {
    _g_cbui_state = {};
    cbui = &_g_cbui_state;
    cbui->running = true;
    cbui->ctx = InitBaselayer();
    cbui->plf = PlafGlfwInit(title);
    cbui->plf->image_buffer = ImageBufferInit(cbui->ctx->a_life);
    cbui->t_framestart = ReadSystemTimerMySec();
    cbui->t_framestart_prev = cbui->t_framestart;

    UI_Init(cbui->plf->width, cbui->plf->height, &cbui->frameno);

    ImageRGBA render_target = { (s32) cbui->plf->width, (s32) cbui->plf->height, (Color*) cbui->plf->image_buffer };
    QuadBufferInit(cbui->ctx->a_life);

    g_texture_map = InitMap(cbui->ctx->a_life, MAX_RESOURCE_CNT);
    g_resource_map = InitMap(cbui->ctx->a_life, MAX_RESOURCE_CNT);

    // load & check resource file
    ResourceStreamHandle hdl = ResourceStreamLoadAndOpen(cbui->ctx->a_tmp, cbui->ctx->a_life, "all.res");
    g_font_names = hdl.names[RST_FONT];
    bool log_verbose = false;

    // map out the resources
    ResourceHdr *res = hdl.first;
    while (res) {
        // fonts
        if (res->tpe == RST_FONT) {
            FontAtlas *font = FontAtlasLoadBinaryStream(res->GetInlinedData(), res->data_sz);
            if (log_verbose) { font->Print(); }

            MapPut(&g_resource_map, font->GetKey(), font);
            MapPut(&g_texture_map, font->GetKey(), &font->texture);
        }


        // sprite maps
        // TODO: do something else // load each sprite map individually
        /*
        else if (res->tpe == RST_SPRITE) {
            SpriteMap *smap = SpriteMapLoadStream((u8*) res->GetInlinedData(), res->data_sz);
            if (log_verbose) {

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
    SetFontAndSize(FS_48, g_font_names->GetStr());

    if (start_in_fullscreen) { PlafGlfwToggleFullscreen(cbui->plf); }

    return cbui;
}


#define FR_RUNNING_AVG_COUNT 4
void CbuiFrameStart() {
    ArenaClear(cbui->ctx->a_tmp);
    ImageBufferClear(cbui->plf->width, cbui->plf->height);

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
    // TODO: get delta t and framerate under control
    XSleep(1);

    UI_FrameEnd(cbui->ctx->a_tmp, cbui->plf->width, cbui->plf->height);
    QuadBufferBlitAndClear(InitImageRGBA(cbui->plf->width, cbui->plf->height, g_image_buffer));

    PlafGlfwUpdate(cbui->plf);
    // TODO: clean up these globals
    g_mouse_x = cbui->plf->cursorpos.x;
    g_mouse_y = cbui->plf->cursorpos.y;
    g_mouse_down = MouseLeft().ended_down;
    g_mouse_pushed = MouseLeft().pushed;

    cbui->running = cbui->running && !GetEscape() && !GetWindowShouldClose(cbui->plf);
}

void CbuiExit() {
    PlafGlfwTerminate(cbui->plf);
}


#endif
