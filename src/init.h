#ifndef __CBUI_INIT_H__
#define __CBUI_INIT_H__


MContext *InitCbui(u64 *frameno, PlafGlfw **platform_out) {

    MContext *ctx = InitBaselayer();
    PlafGlfw *plf = PlafGlfwInit("Testris");
    *platform_out = plf;
    plf->image_buffer = ImageBufferInit(ctx->a_life);

    InitImUi(plf->width, plf->height, frameno);

    ImageRGBA render_target = { (s32) plf->width, (s32) plf->height, (Color*) plf->image_buffer };
    SpriteRender_Init(ctx->a_life);

    g_texture_map = InitMap(ctx->a_life, MAX_RESOURCE_CNT);
    g_resource_map = InitMap(ctx->a_life, MAX_RESOURCE_CNT);

    // load & check resource file
    ResourceStreamHandle hdl = ResourceStreamLoadAndOpen(ctx->a_tmp, ctx->a_life, "all.res");
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
        else if (res->tpe == RST_SPRITE) {
            SpriteMap *smap = SpriteMapLoadStream((u8*) res->GetInlinedData(), res->data_sz);
            if (log_verbose) {

                printf("sprite map: %s, %s, count: %u, atlas w: %u, atlas h: %u\n", smap->map_name, smap->key_name, smap->sprites.len, smap->texture.width, smap->texture.height);
            }

            MapPut(&g_resource_map, smap->GetKey(), smap);
            MapPut(&g_texture_map, smap->GetKey(), &smap->texture);
        }

        // other
        else {
            printf("WARN: unknown resource detected\n");
        }

        // iter
        res = res->GetInlinedNext();
    }
    SetFontAndSize(FS_48, g_font_names->GetStr());

    return ctx;
}


struct CbuiState {
    MContext *ctx;
    u64 frameno;
    PlafGlfw *plf;
    bool running;
};

static CbuiState _g_cbui_state;
static CbuiState *cbui;

CbuiState *CbuiInit__() {
    cbui = &_g_cbui_state;
    cbui->ctx = InitCbui(&cbui->frameno, &cbui->plf);
    cbui->running = true;
    return cbui;
}

void CbuiFrameStart() {
    ArenaClear(cbui->ctx->a_tmp);
    cbui->frameno++;
    PlafGlfwUpdate(cbui->plf);
    ImageBufferClear(cbui->plf->width, cbui->plf->height);


    // TODO: clean up these globals
    g_mouse_x = cbui->plf->cursorpos.x;
    g_mouse_y = cbui->plf->cursorpos.y;
    g_mouse_down = MouseLeft().ended_down;
    g_mouse_pushed = MouseLeft().pushed;
}

void CbuiFrameEnd() {
    UI_FrameEnd(cbui->ctx->a_tmp, cbui->plf->width, cbui->plf->height);
    SpriteRender_BlitAndCLear(InitImageRGBA(cbui->plf->width, cbui->plf->height, g_image_buffer));
    PlafGlfwUpdate(cbui->plf);

    cbui->running = cbui->running && !GetEscape() && !GetWindowShouldClose(cbui->plf);

    // TODO: get delta t and framerate under control
    XSleep(1);
}

void CbuiExit() {
    PlafGlfwTerminate(cbui->plf);
}


#endif
