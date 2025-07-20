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


#endif
