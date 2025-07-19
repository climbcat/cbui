#include "../src/archive/init_arc.h"
#include "../src/algorithms/octree.h"
#include "../src/algorithms/indices.h"


List<Vector3f> CreateRandomPointCloud(MArena *a, u32 npoints, Vector3f center, Vector3f dimensions) {
    List<Vector3f> pc = InitList<Vector3f>(a, npoints);
    RandInit();
    Vector3f dims = dimensions;
    Vector3f min { center.x - dims.x * 0.5f, center.y - dims.y * 0.5f, center.z - dims.z * 0.5f };
    for (u32 i = 0; i < npoints; ++i) {
        Vector3f v {
            dims.x * Rand01_f32() + min.x,
            dims.y * Rand01_f32() + min.y,
            dims.z * Rand01_f32() + min.z
        };
        pc.Add(&v);
    }
    return pc;
}


void TestRandomPCWithNormals() {
    printf("TestRandomPCWithNormals\n");

    GameLoopOne *loop = InitGameLoopOne();
    SwRenderer *r = loop->GetRenderer();
    EntitySystem *es = InitEntitySystem();
    Entity *axes = EntityCoordAxes(es, NULL, r);
    
    MArena _a_pointclouds = ArenaCreate();
    MArena *a_pcs = &_a_pointclouds;

    // point clouds 
    List<Vector3f> points = CreateRandomPointCloud(a_pcs, 90, { 0.0f, 0.0f, 0.0f }, { 4.0f, 4.0f, 4.0f });
    List<Vector3f> normals = CreateRandomPointCloud(a_pcs, 90, { 0.1f, 0.1f, 0.1f }, { 0.05f, 0.05f, 0.05f });

    Entity *pc = EntityPoints(es, NULL, points);
    pc->ext_normals = &normals;
    pc->tpe = ET_POINTCLOUD_W_NORMALS;
    pc->color = Color { RGBA_GREEN };
    pc->color_alt = Color { RGBA_BLUE };

    EntitySystemPrint(es);

    loop->JustRun(es);
}


void TestVGROcTree() {
    printf("TestVGROcTree\n");


    // test parameters
    f32 rootcube_radius = 0.2f;
    //f32 leaf_size = rootcube_radius / 2.0 / 2.0 / 2.0 / 2.0;
    //f32 leaf_size = rootcube_radius / 2.0 / 2.0;

    // TODO: there is a bug, there should be about 8 vertices out for this case, we get 56. !
    f32 leaf_size = rootcube_radius;
    u32 nvertices_src = 100;
    u32 nvertices_src_2 = 1000;
    bool display_boxes = false;
    Color color_in { RGBA_BLUE };
    Color color_in_2 { RGBA_BLUE };
    Color color_out { RGBA_RED };
    Matrix4f box_transform = Matrix4f_Identity();
    Matrix4f src_transform = Matrix4f_Identity();
    Matrix4f src_transform_2 = Matrix4f_Identity();


    // src/dst storage
    MArena _a_tmp = ArenaCreate(); // vertices & branch list location
    MArena *a_tmp = &_a_tmp;
    MArena _a_dest = ArenaCreate(); // dest storage
    MArena *a_dest = &_a_dest;

    List<Vector3f> src = InitList<Vector3f>(a_tmp, nvertices_src);
    Vector3f rootcube_center { 0.0f, 0.0f, 0.0f };
    f32 pc_radius = 0.2f;
    RandInit(913424423);
    for (u32 i = 0; i < nvertices_src; ++i) {
        Vector3f v {
            rootcube_center.x - pc_radius + 2*pc_radius*Rand01_f32(),
            rootcube_center.y - pc_radius + 2*pc_radius*Rand01_f32(),
            rootcube_center.y - pc_radius + 2*pc_radius*Rand01_f32(),
        };
        src.Add(&v);
    }
    List<Vector3f> src_2 = InitList<Vector3f>(a_tmp, nvertices_src_2);
    Vector3f rootcube_center_2 { 0.1f, 0.1f, 0.1f };
    f32 pc_radius_2 = 0.1f;
    RandInit(913424423);
    for (u32 i = 0; i < nvertices_src_2; ++i) {
        Vector3f v {
            rootcube_center.x - pc_radius_2 + 2*pc_radius_2*Rand01_f32(),
            rootcube_center.y - pc_radius_2 + 2*pc_radius_2*Rand01_f32(),
            rootcube_center.y - pc_radius_2 + 2*pc_radius_2*Rand01_f32(),
        };
        src_2.Add(&v);
    }


    // run the vgr
    List<Vector3f> dest = InitList<Vector3f>(a_dest, nvertices_src);
    List<OcLeaf> leaf_blocks_out;
    List<OcBranch> branch_blocks_out;

    // run the *other* vgr
    VoxelGridReduce vgr = VoxelGridReduceInit(leaf_size, rootcube_radius, box_transform);
    vgr.AddPoints(src, src, src_transform);
    vgr.AddPoints(src_2, src_2, src_transform_2);
    List<Vector3f> points_out;
    List<Vector3f> normals_out;
    vgr.GetPoints(a_dest, &points_out, &normals_out);
    leaf_blocks_out = vgr.leaves;
    branch_blocks_out = vgr.branches;


    // visualize
    GameLoopOne *loop = InitGameLoopOne();
    SwRenderer *r = loop->GetRenderer();
    EntitySystem *es = InitEntitySystem();
    Entity *axes = EntityCoordAxes(es, NULL, r);

    Entity *src_pc = EntityPoints(es, NULL, src);
    src_pc->transform = src_transform;
    src_pc->tpe = ET_POINTCLOUD;
    src_pc->color = color_in;

    Entity *src_pc_2 = EntityPoints(es, NULL, src_2);
    src_pc_2->transform = src_transform_2;
    src_pc_2->tpe = ET_POINTCLOUD;
    src_pc_2->color = color_in_2;

    Entity *dest_pc = EntityPoints(es, NULL, points_out);
    src_pc->transform = src_transform;
    dest_pc->tpe = ET_POINTCLOUD;
    dest_pc->color = color_out;

    // print leaf boxes
    printf("\n");
    for (u32 i = 0; i < leaf_blocks_out.len; ++i) {
        OcLeaf leaf = leaf_blocks_out.lst[i];

        for (u32 j = 0; j < 8; ++j) {
            printf("%u ", leaf.cnt[j]);
        }
        #if VGR_DEBUG
        for (u32 j = 0; j < 8; ++j) {
            if (leaf.cnt[j] > 0) {
                printf("c: %f %f %f, r: %f\n", leaf.center[j].x, leaf.center[j].y, leaf.center[j].z, leaf.radius[j]);
                if (display_boxes) {
                    Entity *cub = EntityAABox(es, leaf.center[j], leaf.radius[j], r);
                }
            }
        }
        #endif
    }

    // print vgr stats
    printf("\n");
    vgr.stats.Print();

    loop->JustRun(es);
}


void TestQuaternionRotMult() {
    printf("TestQuaternionRotMult\n");

    GameLoopOne *loop = InitGameLoopOne();
    SwRenderer *r = loop->GetRenderer();
    EntitySystem *es = InitEntitySystem();

    Entity *axes = EntityCoordAxes(es, NULL, r);
    Entity *box0 = EntityAABox(es, NULL, { 0.0f, 0.0f, 0.f }, 0.2f, r);
    box0->color = Color { RGBA_RED };
    Entity *box = EntityAABox(es, NULL, { 0.0f, 0.0f, 0.f }, 0.2f, r);

    Vector3f r1 { 0.0f, 1.0f, 0.0f };
    r1.Normalize();
    float angle0 = 30.0f * deg2rad;
    float angle1 = 15.0f * deg2rad;
    Quat q1 = QuatAxisAngle(r1, angle1);
    box0->transform = TransformBuild(r1, angle0);
    box->transform = TransformQuaternion( QuatMult(q1, q1) );

    loop->JustRun(es);
}


void TestSlerpAndMat2Quat() {
    printf("TestSlerpAndMat2Quat\n");

    GameLoopOne *loop = InitGameLoopOne();
    SwRenderer *r = loop->GetRenderer();
    EntitySystem *es = InitEntitySystem();

    Entity *axes = EntityCoordAxes(es, NULL, r);

    Entity *box0 = EntityAABox(es, NULL, { 0.0f, 0.0f, 0.f }, 0.2f, r);
    box0->color = Color { RGBA_RED };
    Entity *box1 = EntityAABox(es, NULL, { 0.0f, 0.0f, 0.f }, 0.2f, r);
    box1->color = Color { RGBA_BLUE };
    Entity *box = EntityAABox(es, NULL, { 0.0f, 0.0f, 0.f }, 0.2f, r);
    box->color = Color { RGBA_GREEN };

    Vector3f r1 { 0.1f, 1.0f, -0.4f };
    r1.Normalize();
    f32 angle0 = 5.0f * deg2rad;
    f32 angle1 = 65.0f * deg2rad;


    box0->transform = TransformBuild(r1, angle0);
    box1->transform = TransformBuild(r1, angle1);
    box->transform = TransformBuild(r1, angle0);

    Quat q0 = QuatFromTransform(box0->transform);
    Quat q1 = QuatFromTransform(box1->transform);
    Quat q = q0;

    f32 t = 0.0f;
    f32 dt = 0.03f;
    while (loop->GameLoopRunning()) {

        t += dt;
        if ( t <= 0.001f || t >= 0.999f ) {
            dt *= -1;
            t += dt;
        }
        q = Slerp(q0, q1, t);
        box->transform = TransformQuaternion( q );

        loop->CycleFrame(es);
    }
    loop->Terminate();
}


void TestPointCloudsBoxesAndSceneGraph() {
    printf("TestPointCloudsBoxesAndSceneGraph\n");

    // point clouds 
    MArena _a_pointclouds = ArenaCreate();
    MArena *a_pcs = &_a_pointclouds;
    List<Vector3f> points_1 = CreateRandomPointCloud(a_pcs, 90, { 0.0f, 0.0f, 0.0f }, { 4.0f, 4.0f, 4.0f });
    List<Vector3f> points_2 = CreateRandomPointCloud(a_pcs, 300, { 0.0f, 0.0f, -1.f }, { 4.0f, 4.0f, 2.0f });
    List<Vector3f> points_3 = CreateRandomPointCloud(a_pcs, 300, { -1.0f, -1.0f, -1.f }, { 2.0f, 2.0f, 2.0f });

    GameLoopOne *loop = InitGameLoopOne();
    SwRenderer *r = loop->GetRenderer();
    EntitySystem *es = InitEntitySystem();

    // entities
    Entity *axes = EntityCoordAxes(es, NULL, r);
    Entity *box = EntityAABox(es, NULL, { 0.3f, 0.0f, 0.7f }, 0.2f, r);
    Entity *box2 = EntityAABox(es, NULL, { 0.3f, 0.0f, -0.7f }, 0.2f, r);
    Entity *box3 = EntityAABox(es, NULL, { -0.7f, 0.0f, 0.0f }, 0.2f, r);

    box->tpe = ET_LINES_ROT;
    box2->tpe = ET_LINES_ROT;
    box3->tpe = ET_LINES_ROT;

    Entity *pc_1 = EntityPoints(es, axes, points_1);
    Entity *pc_2 = EntityPoints(es, pc_1, points_2);
    Entity *pc_3 = EntityPoints(es, pc_2, points_3);

    pc_1->color = { RGBA_GREEN };
    pc_2->color = { RGBA_BLUE };
    pc_3->color = { RGBA_RED };


    // test point colors
    RandInit();
    List<Color> point_colors = InitList<Color>(a_pcs, points_1.len);
    for (u32 i = 0; i < points_1.len; ++i) {
        Color c;
        c.a = 255;
        c.r = RandMinMaxI(0, 255);
        c.g = RandMinMaxI(0, 255);
        c.b = RandMinMaxI(0, 255);
        point_colors.Add(c);
    }
    pc_1->SetColorsList(point_colors);


    // test render only active entities
    pc_2->DeActivate();
    EntitySystemPrint(es);
    while (loop->GameLoopRunning()) {
        Key kpress = loop->mouse.last_keypress_frame;

        if (kpress == OUR_GLFW_KEY_LEFT) {
            es->CursorPrev();
        }
        else if (kpress == OUR_GLFW_KEY_RIGHT) {
            es->CursorNext();
        }
        else if (kpress == OUR_GLFW_KEY_UP) {
            es->CursorUp();
        }
        else if (kpress == OUR_GLFW_KEY_DOWN) {
            es->CursorDown();
        }
        else if (kpress == OUR_GLFW_KEY_SPACE) {
            es->CursorToggleEntityActive();
        }

        loop->CycleFrame(es);
    }
    loop->Terminate();
}




void _PrintIndices(const char *prefix, List<u32> idxs) {
    printf("%s", prefix);
    for (u32 i = 0; i < idxs.len; ++i) {
        printf("%u ", idxs.lst[i]);
    }
    printf("\n");
}
void TestIndexSetOperations() {
    printf("TestIndexSetOperations\n");

    MContext *ctx = InitBaselayer();
    MArena *a = ctx->a_tmp;

    // naiive values
    List<u32> values = InitList<u32>(a, 10);
    for (u32 i = 0; i < 10; ++i) {
        values.Add( i );
    }

    // covering indices with duplicates
    List<u32> idxs = InitList<u32>(a, 15);
    for (u32 i = 0; i < 15; ++i) {
        idxs.Add( i % 10 );
    }

    // list to remove
    List<u32> idxs_rm = InitList<u32>(a, 15);
    idxs_rm.Add( (u32) 0 );
    idxs_rm.Add( 4 );
    idxs_rm.Add( 5 );
    idxs_rm.Add( 6 );
    idxs_rm.Add( 7 );

    // list to extract
    List<u32> idxs_extr = InitList<u32>(a, 15);
    idxs_extr.Add( 9 );
    idxs_extr.Add( 5 );
    idxs_extr.Add( 1 );
    idxs_extr.Add( (u32) 0 );
    idxs_extr.Add( 2 );

    printf("\n");
    _PrintIndices("values:    ", values);
    _PrintIndices("idxs:      ", idxs);
    _PrintIndices("idxs_rm:   ", idxs_rm);
    _PrintIndices("idxs_extr: ", idxs_extr);

    // extract
    List<u32> vals_extr_out;
    List<u32> idxs_extr_out;
    List<u32> indirect = IndicesExtract<u32>(a, a, values, idxs_extr, &vals_extr_out, &idxs_extr_out);

    printf("\ntest IndicesExtract()\n");
    _PrintIndices("vals_extr_out: ", vals_extr_out);
    _PrintIndices("idxs_extr_out: ", idxs_extr_out);

    //: remove
    List<u32> vals_rm_out;
    List<u32> idxs_rm_out;
    List<u32> indirect_rm = IndicesRemove<u32>(a, a, values, idxs, idxs_rm, &vals_rm_out, &idxs_rm_out);
    printf("\ntest IndicesRemove()\n");
    _PrintIndices("vals_rm_out: ", vals_rm_out);
    _PrintIndices("idxs_rm_out: ", idxs_rm_out);
}


void TestLayoutGlyphQuads() {
    printf("TestLayoutGlyphQuads\n");

    MContext *ctx = InitBaselayer();
    GameLoopOne *loop = InitGraphics(ctx);

    //LayoutTextLine("The quick brown fox jumps over the lazy dog", 50, 100, ColorRandom());
    //LayoutTextLine("The other quick brown fox jumps over the other lazy dog", 100, 200, ColorRandom());

    //SR_Render();
    loop->JustShowBuffer();
}


void TestBrownianGlyphs() {
    printf("TestBrownianGlyphs\n");

    MContext *ctx = InitBaselayer();
    GameLoopOne *loop = InitGraphics(ctx);


    Color clbl = ColorRandom();
    u32 ncols = 100;
    List<Color> cols = InitList<Color>(ctx->a_pers, ncols);
    for (u32 i = 0; i < ncols; ++i) {
        cols.Add( ColorRandom() );
    }

    SetFontSize(FS_72);
    List<QuadHexaVertex> quads = LayoutTextAutowrap(ctx->a_pers, g_text_plotter, StrL("The quick brown fox jumps over the lazy dog"), 470, 300, 300, 300, ColorRandom(), TAL_CENTER);
    DrawCall dc = {};
    dc.tpe = DCT_TEXTURE_BYTE;
    dc.texture_key = g_text_plotter->GetKey();
    dc.quads = quads;
    for (u32 i = 0; i < quads.len; ++i) {
        QuadHexaVertex *q = quads.lst + i;
        for (u32 j = 0; j < 6; ++j) {
            QuadVertex *v = q->verts + j;
            v->col = cols.lst[i % cols.len];
        }
    }
    List<QuadHexaVertex> quads_initial = ListCopy(ctx->a_life, quads);
    SetFontSize(FS_48);

    f32 magnitude = 0.8f;
    while (loop->GameLoopRunning()) {
        loop->FrameStart2D();

        // brownian motion applied to each character, effect
        for (u32 i = 0; i < quads.len; ++i) {
            QuadHexaVertex *q = quads.lst + i;
            Vector2f d { magnitude * RandPM1_f32(), magnitude * RandPM1_f32() };
            for (u32 j = 0; j < 6; ++j) {
                QuadVertex *v = q->verts + j;
                v->pos = v->pos + d;
            }
        }

        // reset on key_space
        Key k = loop->GetMouseTrap()->last_keypress_frame;
        if (k == OUR_GLFW_KEY_SPACE) {
            memcpy(quads.lst, quads_initial.lst, sizeof(QuadHexaVertex) * quads_initial.len);
        }
        else if (k == OUR_GLFW_KEY_UP) {
            magnitude *= 1.5f;
        }
        else if (k == OUR_GLFW_KEY_DOWN) {
            magnitude /= 1.5f;
        }

        // render
        //LayoutTextLine("press space/up/down", 50, 50, clbl);
        //SR_Push(dc);

        loop->FrameEnd2D();
    }
    loop->Terminate();
}


void TestUIDragPanel() {
    printf("TestUIPanel\n");

    MContext *ctx = InitBaselayer();
    GameLoopOne *loop = InitGraphics(ctx);

    s32 l = 180;
    s32 t = 140;
    s32 w = 400;
    s32 h = 250;
    s32 border = 1;
    f32 graynexx;
    bool show_pnl = true;


    while (loop->GameLoopRunning()) {
        loop->FrameStart2D(ColorGray(0.9f));

        SetFontSize(FS_72);
        //LayoutTextLine("The quick brown fox jumps over the lazy dog", 50, 80, ColorBlack());

        // bordered drag-panel
        if (show_pnl) {
            graynexx = 0.8f;
            border = 1;
            if (loop->mouse.LimsLTWHLastFrame(l, t, w, h)) {
                graynexx = 0.7f;

                if (loop->mouse.l) {
                    graynexx = 0.6f;
                    border = 2;
                    l += loop->mouse.dx;
                    t += loop->mouse.dy;
                }
                bool clicked = loop->mouse.ClickedRecently();
                if (clicked) {
                    printf("click\n");
                    //show_pnl = false;
                }
            }
            PanelPlot(l, t, w, h, border, ColorBlack(), ColorGray(graynexx));
            SetFontSize(FS_48);
            //LayoutTextLine("The other quick brown fox jumps over the other lazy dog", l, t, ColorBlack());
            // TODO: align, e.g. TAL_CENTER
        }

        if (loop->GetMouseTrap()->last_keypress_frame == OUR_GLFW_KEY_SPACE) {
            show_pnl = true;
        }

        loop->FrameEnd2D();
    }
    loop->Terminate();

    // TODO: draw a boundaried panel - OK
    // TODO: make it respond to mouse interaction - OK
    // TODO: make it drag-able - OK
    // TODO: have it have text layed out in it - OK
}


void TestUILayoutWidgetAPI() {
    printf("TestUILayoutWidgetAPI\n");

    MContext *ctx = InitBaselayer();
    GameLoopOne *loop = InitGraphics(ctx);

    while (loop->GameLoopRunning()) {
        loop->FrameStart2D(ColorGray(0.95f));

        UI_LayoutVertical();

        if (false) {
            // builder code: nested horizontal layout in a vertical layout
            bool first_v = true;
            bool nested_h = true;

            if (first_v) {
                UI_LayoutVertical();

                // TODO: this one breaks it
                //UI_LayoutVerticalCenterX();

                UI_Label("Vert");
                UI_Label(" ical ");
                UI_Label("text");
            }
            if (nested_h) {
                UI_LayoutHorizontal();
                UI_Label("Horiz");
                UI_Label(" ontal ");
                UI_Label("label");
            }

            if (nested_h) {
                UI_Pop();
            }
            if (first_v) {
                UI_Pop();
            }
            UI_Button("hest");
        }


        if (true) {
            UI_Pop();

            // builder code: also centering panels
            UI_CoolPanel(600, 600);
            UI_SpaceV(30);
            UI_Label("A descriptive label");
            UI_SpaceV(30);
            if (UI_Button("Ok")) {
                printf("clicked Ok!\n");
            }
            UI_Pop();

            UI_CoolPanel(500, 500);
            UI_SpaceV(30);

            UI_LayoutHorizontal();
            UI_Label("Evocative ");
            UI_Label("text");
            UI_Pop();

            UI_SpaceV(30);
            if (UI_Button("Cancel")) {
                printf("clicked Cancel!\n");
            }
        }


        // frame end
        //UI_FrameEnd(ctx->a_tmp);
        loop->FrameEnd2D();
    }
    loop->Terminate();
}


void TestResourceLoad() {
    printf("TestResourceLoad\n");

    MContext *ctx = InitBaselayer();
    ResourceStreamHandle hdl = ResourceStreamLoadAndOpen(ctx->a_tmp, ctx->a_life, "all.res");

    for (u32 i = 0; i < RST_CNT; ++i) {
        printf("names (%u):\n", i);
        StrLstPrint(hdl.names[i]);
        printf("key-names (%u):\n", i);
        StrLstPrint(hdl.key_names[i]);
    }
}


SpriteMap *GetSpriteMap(const char *key_name) {
    SpriteMap *smap = (SpriteMap*) MapGet(&g_resource_map, HashStringValue(key_name));
    return smap;
}
void TestRenderSprites() {
    printf("TestRenderSprites\n");

    MContext *ctx = InitBaselayer();
    GameLoopOne *loop = InitGraphics(ctx, 580, 800);

    SpriteMap *smap = GetSpriteMap("aliens_01");
    u64 smap_key = smap->GetKey();

    Color gray = ColorGray(0.3f);
    while (loop->GameLoopRunning()) {
        loop->FrameStart2D(gray);

        DrawCall dc;
        dc.tpe = DCT_TEXTURE_RGBA;
        dc.texture_key = smap_key;
        dc.quads = InitList<QuadHexaVertex>(ctx->a_tmp, smap->sprites.len);

        u32 naliens_y = 12;
        u32 naliens_x = 11;
        for (u32 j = 0; j < naliens_y; ++j) {
            for (u32 i = 0; i < naliens_x; ++i) {
                s32 x = i * 50 + 20;
                s32 y = j * 50 + 10;
                Sprite s = smap->sprites.lst[i + j*naliens_x];
                //dc.quads.Add(QuadCookTextured(s, x, y));

                Widget *frame = UI_CoolPanel(s.w + 2, s.h + 2);
                frame->SetFeature(WF_ABSREL_POSITION);
                frame->col_bckgrnd = ColorBlack();
                frame->col_border = ColorWhite();
                frame->sz_border = 1;
                frame->x0 = x - 1;
                frame->y0 = y - 1;
            }
        }

        //UI_FrameEnd(ctx->a_tmp);

        // NOTE!: here we put the sprite ddraw call (SR_Push(dc)) after UI_FrameEnd, which puts the widget 
        //      draw calls onto the draw list.
        //      Doing it contrary would cause them to do not appear. This means something needs fixing,
        //      re-ordering, or something. We want a way to display things in the right order, without 
        //      hacing the API like this. Sprites should be grouped with their "container" widget, and be
        //      drawn on top of that, but also below other widgets that are on top, e.g.:
        // TODO:  Interleaving

        //SR_Push(dc);

        loop->FrameEnd2D();
    }
    loop->Terminate();
}


void TestLayoutPanels() {
    printf("TestLayoutPanels\n");

    MContext *ctx = InitBaselayer();
    GameLoopOne *loop = InitGraphics(ctx, 1000, 500);

    SetFontSize(FS_18);


    //#define NUM_BTNS 3;
    static bool btn_slct[3];

    Color gray = ColorGray(0.3f);
    while (loop->GameLoopRunning()) {
        loop->FrameStart2D(gray);

        // wrapper
        Widget *w0 = UI_LayoutVertical();
        w0->features_flg |= WF_EXPAND_VERTICAL;
        w0->features_flg |= WF_EXPAND_HORIZONTAL;

        {
            // wrapper
            Widget *w1 = UI_LayoutHorizontal();
            w1->features_flg |= WF_EXPAND_VERTICAL;
            w1->features_flg |= WF_EXPAND_HORIZONTAL;

            {
                // list view
                Widget *l_nodes = UI_LayoutVertical();
                   l_nodes->features_flg |= WF_DRAW_BACKGROUND_AND_BORDER;
                l_nodes->col_bckgrnd = ColorRed();
                l_nodes->col_border = ColorGray(0.9f);
                l_nodes->sz_border = 5;
                l_nodes->features_flg |= WF_EXPAND_VERTICAL;
                l_nodes->w = 740;

                {
                    Widget *itm_1;
                    if (UI_ToggleButton("item 1", &btn_slct[0], &itm_1)) {
                        for (u32 i = 0; i < 3; ++i) {
                            btn_slct[i] = 0;
                        }
                        btn_slct[0] = 1;
                        printf("item 1 pushed\n");
                    }
                    itm_1->features_flg |= WF_EXPAND_HORIZONTAL;
                    itm_1->h = 20;
                }
                {
                    Widget *itm_1;
                    if (UI_ToggleButton("item 2", &btn_slct[1], &itm_1)) {
                        for (u32 i = 0; i < 3; ++i) {
                            btn_slct[i] = 0;
                        }
                        btn_slct[1] = 1;
                        printf("item 2 pushed\n");
                    }
                    itm_1->features_flg |= WF_EXPAND_HORIZONTAL;
                    itm_1->h = 20;
                }
                {
                    Widget *itm_1;
                    if (UI_ToggleButton("item 3", &btn_slct[2], &itm_1)) {
                        for (u32 i = 0; i < 3; ++i) {
                            btn_slct[i] = 0;
                        }
                        btn_slct[2] = 1;
                        printf("item 3 pushed\n");
                    }
                    itm_1->features_flg |= WF_EXPAND_HORIZONTAL;
                    itm_1->h = 20;
                }

                UI_Pop();
            }

            {
                // action btns
                Widget *l_btns = UI_LayoutVertical();
                l_btns->features_flg |= WF_DRAW_BACKGROUND_AND_BORDER;
                l_btns->col_bckgrnd = ColorBlue();
                l_btns->col_border = ColorGray(0.7f);
                l_btns->sz_border = 5;
                l_btns->features_flg |= WF_EXPAND_HORIZONTAL;
                l_btns->features_flg |= WF_EXPAND_VERTICAL;

                UI_Label("action 1", ColorWhite());
                UI_Label("action 2", ColorWhite());
                UI_Label("action 3", ColorWhite());

                UI_Pop();
            }

            UI_Pop();
        }

        Widget *l_status = UI_LayoutHorizontal();
        l_status->features_flg |= WF_DRAW_BACKGROUND_AND_BORDER;
        l_status->col_border = ColorGray(0.5f);
        l_status->sz_border = 5;
        l_status->col_bckgrnd = ColorGreen();
        l_status->h = 50;
        l_status->features_flg |= WF_EXPAND_HORIZONTAL;


        UI_Label("status bar", ColorWhite());

        //UI_FrameEnd(ctx->a_tmp);
        loop->FrameEnd2D();
    }
    loop->Terminate();
}


void TestRotatingBoxes() {
    printf("TestRotatingBoxes\n");

    GameLoopOne *loop = InitGameLoopOne();
    SwRenderer *r = loop->GetRenderer();
    EntitySystem *es = InitEntitySystem();

    Entity *axes = EntityCoordAxes(es, NULL, r);
    Entity *box = EntityAABox(es, NULL, { 0.3f, 0.0f, 0.7f }, 0.2f, r);
    box->tpe = ET_LINES_ROT;
    Entity *box2 = EntityAABox(es, NULL, { 0.3f, 0.0f, -0.7f }, 0.2f, r);
    box2->tpe = ET_LINES_ROT;
    Entity *box3 = EntityAABox(es, NULL, { -0.7f, 0.0f, 0.0f }, 0.2f, r);
    box3->tpe = ET_LINES_ROT;
    EntitySystemPrint(es);


    while (loop->GameLoopRunning()) {
        // E.g.: update entity transforms
        // E.g.: update debug UI
        // E.g.: run simulations
        // E.g.: pull worker thread statuses

        loop->CycleFrame(es);
    }
    loop->Terminate();
}


void BoxesAndPointClouds() {
    printf("BoxesAndPointClouds\n");

    // point clouds 
    MArena _a_pointclouds = ArenaCreate();
    MArena *a_pcs = &_a_pointclouds;
    List<Vector3f> points_1 = CreateRandomPointCloud(a_pcs, 90, { 0.0f, 0.0f, 0.0f }, { 4.0f, 4.0f, 4.0f });
    List<Vector3f> points_2 = CreateRandomPointCloud(a_pcs, 300, { 0.0f, 0.0f, -1.f }, { 4.0f, 4.0f, 2.0f });
    List<Vector3f> points_3 = CreateRandomPointCloud(a_pcs, 300, { -1.0f, -1.0f, -1.f }, { 2.0f, 2.0f, 2.0f });

    GameLoopOne *loop = InitGameLoopOne();
    SwRenderer *r = loop->GetRenderer();
    EntitySystem *es = InitEntitySystem();

    // entities
    Entity *axes = EntityCoordAxes(es, NULL, r);
    Entity *box = EntityAABox(es, NULL, { 0.3f, 0.0f, 0.7f }, 0.2f, r);
    Entity *box2 = EntityAABox(es, NULL, { 0.3f, 0.0f, -0.7f }, 0.2f, r);
    Entity *box3 = EntityAABox(es, NULL, { -0.7f, 0.0f, 0.0f }, 0.2f, r);

    box->tpe = ET_LINES_ROT;
    box2->tpe = ET_LINES_ROT;
    box3->tpe = ET_LINES_ROT;

    Entity *pc_1 = EntityPoints(es, axes, points_1);
    Entity *pc_2 = EntityPoints(es, pc_1, points_2);
    Entity *pc_3 = EntityPoints(es, pc_2, points_3);

    pc_1->color = { RGBA_GREEN };
    pc_2->color = { RGBA_BLUE };
    pc_3->color = { RGBA_RED };


    // test point colors
    RandInit();
    List<Color> point_colors = InitList<Color>(a_pcs, points_1.len);
    for (u32 i = 0; i < points_1.len; ++i) {
        Color c;
        c.a = 255;
        c.r = RandMinMaxI(0, 255);
        c.g = RandMinMaxI(0, 255);
        c.b = RandMinMaxI(0, 255);
        point_colors.Add(c);
    }
    pc_1->SetColorsList(point_colors);


    // test render only active entities
    pc_2->DeActivate();
    EntitySystemPrint(es);
    while (loop->GameLoopRunning()) {
        Key kpress = loop->mouse.last_keypress_frame;

        if (kpress == OUR_GLFW_KEY_LEFT) {
            es->CursorPrev();
        }
        else if (kpress == OUR_GLFW_KEY_RIGHT) {
            es->CursorNext();
        }
        else if (kpress == OUR_GLFW_KEY_UP) {
            es->CursorUp();
        }
        else if (kpress == OUR_GLFW_KEY_DOWN) {
            es->CursorDown();
        }
        else if (kpress == OUR_GLFW_KEY_SPACE) {
            es->CursorToggleEntityActive();
        }

        loop->CycleFrame(es);
    }
    loop->Terminate();
}



void TestBoxesAndPointClouds() {
    printf("TestBoxesAndPointClouds\n");

    // point clouds 
    MArena _a_pointclouds = ArenaCreate();
    MArena *a_pcs = &_a_pointclouds;
    List<Vector3f> points_1 = CreateRandomPointCloud(a_pcs, 90, { 0.0f, 0.0f, 0.0f }, { 4.0f, 4.0f, 4.0f });
    List<Vector3f> points_2 = CreateRandomPointCloud(a_pcs, 300, { 0.0f, 0.0f, -1.f }, { 4.0f, 4.0f, 2.0f });
    List<Vector3f> points_3 = CreateRandomPointCloud(a_pcs, 300, { -1.0f, -1.0f, -1.f }, { 2.0f, 2.0f, 2.0f });

    GameLoopOne *loop = InitGameLoopOne();
    SwRenderer *r = loop->GetRenderer();
    EntitySystem *es = InitEntitySystem();

    // entities
    Entity *axes = EntityCoordAxes(es, NULL, r);
    Entity *box = EntityAABox(es, NULL, { 0.3f, 0.0f, 0.7f }, 0.2f, r);
    Entity *box2 = EntityAABox(es, NULL, { 0.3f, 0.0f, -0.7f }, 0.2f, r);
    Entity *box3 = EntityAABox(es, NULL, { -0.7f, 0.0f, 0.0f }, 0.2f, r);

    box->tpe = ET_LINES_ROT;
    box2->tpe = ET_LINES_ROT;
    box3->tpe = ET_LINES_ROT;

    Entity *pc_1 = EntityPoints(es, axes, points_1);
    Entity *pc_2 = EntityPoints(es, pc_1, points_2);
    Entity *pc_3 = EntityPoints(es, pc_2, points_3);

    pc_1->color = { RGBA_GREEN };
    pc_2->color = { RGBA_BLUE };
    pc_3->color = { RGBA_RED };


    // test point colors
    RandInit();
    List<Color> point_colors = InitList<Color>(a_pcs, points_1.len);
    for (u32 i = 0; i < points_1.len; ++i) {
        Color c;
        c.a = 255;
        c.r = RandMinMaxI(0, 255);
        c.g = RandMinMaxI(0, 255);
        c.b = RandMinMaxI(0, 255);
        point_colors.Add(c);
    }
    pc_1->SetColorsList(point_colors);


    // test render only active entities
    pc_2->DeActivate();
    EntitySystemPrint(es);
    while (loop->GameLoopRunning()) {
        Key kpress = loop->mouse.last_keypress_frame;

        if (kpress == OUR_GLFW_KEY_LEFT) {
            es->CursorPrev();
        }
        else if (kpress == OUR_GLFW_KEY_RIGHT) {
            es->CursorNext();
        }
        else if (kpress == OUR_GLFW_KEY_UP) {
            es->CursorUp();
        }
        else if (kpress == OUR_GLFW_KEY_DOWN) {
            es->CursorDown();
        }
        else if (kpress == OUR_GLFW_KEY_SPACE) {
            es->CursorToggleEntityActive();
        }

        loop->CycleFrame(es);
    }
    loop->Terminate();
}


void Test() {
    //TestRandomPCWithNormals();
    //TestVGROcTree();
    
    //TestQuaternionRotMult(); // TODO: fix
    //TestSlerpAndMat2Quat();
    //TestPointCloudsBoxesAndSceneGraph();
    //TestIndexSetOperations();
    //TestLayoutGlyphQuads();
    //TestBrownianGlyphs();
    //TestUIDragPanel();
    //TestUILayoutWidgetAPI();
    //TestResourceLoad();
    //TestRenderSprites();
    //TestLayoutPanels();
    //TestBoxesAndPointClouds();
    TestRotatingBoxes();
}
