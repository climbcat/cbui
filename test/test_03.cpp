//#include "../release/0.2.0/jg_baselayer.h"
#include "../../baselayer/src/baselayer.h"

#include "../src/cbui.h"
#include "../src/imui/color.h"
#include "../src/imui/sprite.h"
#include "../src/imui/resource.h"
#include "../src/imui/font.h"
#include "../src/imui/imui.h"
#include "../src/init.h"



void TestUILayoutFeatures() {
    printf("TestUILayoutFeatures\n");

    CbuiInit("TestUILayoutFeatures", false);
    s32 TB_mode = 0;

    f32 time = 0;
    UI_SetFontSize(FS_24);
    UI_DebugMode(true);
    UI_DebugNames(true);

    while (cbui.running) {
        CbuiFrameStart();

        switch (TB_mode) {
        case 0: {
            UI_Center();

            UI_LayoutVertical();
            UI_Label("Test: Align Left");
            UI_Label("A couple");
            UI_Label("lines");
            UI_Label("of");
            UI_Label("text");
        } break;

        case 1: {
            UI_Center();

            Widget *w = UI_LayoutVertical(0);
            w->SetFlag(WF_DRAW_BACKGROUND_AND_BORDER);
            w->col_bckgrnd = COLOR_BLUE;
            Widget *l = UI_Label("Test: Align Left");
            l->col_text = COLOR_GRAY_50;
            UI_Label("A couple");
            UI_Label("lines");
            UI_Label("of");
            UI_Label("text");
        } break;

        case 2: {
            UI_Center();

            UI_LayoutVertical(-1);
            UI_Label("Test: Alight right");
            UI_Label("A couple");
            UI_Label("lines");
            UI_Label("of");
            UI_Label("text");

        } break;

        case 3: {
            UI_Center();

            UI_LayoutHorizontal();

            UI_Label("Hori");
            UI_Label("zon");
            UI_Label("tal");

            Widget *s0 = UI_Sibling();
            s0->w = 50;
            s0->h = 400;

            UI_LayoutVertical(1);
            UI_Label("Vert-");
            UI_Label("ical");
            UI_Label("");
            UI_Label("layout");
            UI_Pop();

            Widget *s1 = UI_Sibling();
            s1->w = 50;
            s1->h = 400;

            UI_Label("layout");
        } break;

        case 4: {
            UI_Center();

            Widget *vert = UI_LayoutVertical(-1);
            vert->DBG_tag = StrL("vert");

            vert->w = 500;
            vert->h = 120;

            Widget *s0 = UI_Sibling();
            s0->DBG_tag = StrL("s0");
            s0->w = 400;
            s0->h = 100;

            Widget *s1 = UI_Sibling();
            s1->DBG_tag = StrL("s1");
            s1->w = 300;
            s1->h = 50;
        } break;

        case 5: {
            UI_Center();

            Widget *vert = UI_LayoutVertical(0);
            vert->DBG_tag = StrL("vert");

            vert->w = 500;
            vert->h = 120;

            Widget *s0 = UI_Sibling();
            s0->DBG_tag = StrL("s0");
            s0->w = 400;
            s0->h = 100;

            Widget *s1 = UI_Sibling();
            s1->DBG_tag = StrL("s1");
            s1->w = 300;
            s1->h = 50;
        } break;

        case 6: {
            UI_Center();

            Widget *vert = UI_LayoutVertical();
            vert->DBG_tag = StrL("vert");

            vert->w = 500;
            vert->h = 120;

            Widget *s0 = UI_Sibling();
            s0->DBG_tag = StrL("s0");
            s0->w = 400;
            s0->h = 100;

            Widget *s1 = UI_Sibling();
            s1->DBG_tag = StrL("s1");
            s1->w = 300;
            s1->h = 50;
        } break;

        case 7: {
            UI_Center();

            Widget *vert = UI_LayoutHorizontal(-1);
            vert->DBG_tag = StrL("horiz");

            // NOTE: out-comment to check behavior
            vert->w = 120;
            vert->h = 450;

            Widget *s0 = UI_Sibling();
            s0->DBG_tag = StrL("s0");
            s0->w = 100;
            s0->h = 400;

            Widget *s1 = UI_Sibling();
            s1->DBG_tag = StrL("s1");
            s1->w = 50;
            s1->h = 300;
        } break;

        case 8: {
            UI_Center();

            Widget *vert = UI_LayoutHorizontal(0);
            vert->DBG_tag = StrL("horiz");

            // NOTE: out-comment to check behavior
            vert->w = 120;
            vert->h = 450;

            Widget *s0 = UI_Sibling();
            s0->DBG_tag = StrL("s0");
            s0->w = 100;
            s0->h = 400;

            Widget *s1 = UI_Sibling();
            s1->DBG_tag = StrL("s1");
            s1->w = 50;
            s1->h = 300;
        } break;

        case 9: {
            UI_Center();

            Widget *vert = UI_LayoutHorizontal(1);
            vert->DBG_tag = StrL("horiz");

            // NOTE: out-comment to check behavior
            vert->w = 120;
            vert->h = 450;

            Widget *s0 = UI_Sibling();
            s0->DBG_tag = StrL("s0");
            s0->w = 100;
            s0->h = 400;

            Widget *s1 = UI_Sibling();
            s1->DBG_tag = StrL("s1");
            s1->w = 50;
            s1->h = 300;
        } break;

        case 10: {
            UI_Center();

            Widget *vert = UI_LayoutVertical(0);
            vert->DBG_tag = StrL("vert");
            vert->w = 500;
            vert->h = 320;

            Widget *s0 = UI_Sibling();
            s0->DBG_tag = StrL("s0");
            s0->w = 400;
            s0->h = 100;

            Widget *exp = UI_ExpanderH();
            exp->features_flg |= WF_EXPAND_VERTICAL;
            exp->DBG_tag = StrL("exp");
            exp->h = 50;

            Widget *s1 = UI_Sibling();
            s1->DBG_tag = StrL("s1");
            s1->w = 300;
            s1->h = 50;

        } break;

        default: break; }

        if (GetSpace()) {
            TB_mode = (TB_mode + 1) % 11;
        }
        if (GetFKey(2)) {
            UI_DebugMode(!g_ui_debugmode);
        }
        if (GetFKey(3)) {
            UI_DebugNames(!g_ui_debugnames);
        }

        CbuiFrameEnd();
    }
    CbuiExit();
}


void TestSceneGraph() {
    printf("TestSceneGraph\n");

    CbuiInit("TestSceneGraph", false);
    Perspective persp = ProjectionInit(cbui.plf.width, cbui.plf.height);
    OrbitCamera cam = OrbitCameraInit(persp.aspect);
    Array<Wireframe> objs = InitArray<Wireframe>(cbui.ctx->a_pers, 100);

    SceneGraphInit();
    Transform *t0 = SceneGraphAlloc();
    Transform *t1 = SceneGraphAlloc(t0);
    Transform *t2 = SceneGraphAlloc(t1);
    Transform *t3 = SceneGraphAlloc(t2);
    Transform *t4 = SceneGraphAlloc(t3);

    u32 mode = 0;
    u32 mode_cnt = 2;

    while (cbui.running) {
        CbuiFrameStart();
        OrbitCameraUpdate(&cam, cbui.plf.cursorpos.dx, cbui.plf.cursorpos.dy, cbui.plf.left.ended_down, cbui.plf.scroll.yoffset_acc);
        OrbitCameraPan(&cam, persp.fov, persp.aspect, cbui.plf.cursorpos.x_frac, cbui.plf.cursorpos.y_frac, MouseRight().pushed, MouseRight().released);
        // start


        if (GetSpace()) {
            mode++;
            mode = mode % mode_cnt;

            printf("switched to mode %d\n", mode);
        }

        float dtheta = 0.5f;

        Matrix4f t_root = TransformBuildTranslation({ 0, 0.5, 0 });
        Matrix4f t_arm = TransformBuildTranslation({ 2, 0, 0 });
        Matrix4f t_hand = TransformBuildTranslation({ 0.8, 0, 0 });
        Matrix4f t_finger = TransformBuildTranslation({ 0.3, 0, 0 });
        Matrix4f rot_y = TransformBuildRotateY(dtheta * cbui.frameno * deg2rad);
        Matrix4f rot_y_inv = TransformGetInverse(rot_y);
        Matrix4f rot_y2 = TransformBuildRotateY(dtheta * 2.5f * cbui.frameno * deg2rad);

        if (mode == 0) {
            t0->t_loc = t_root * rot_y;
            t1->t_loc = t_arm;
            t2->t_loc = rot_y;
            t3->t_loc = t_hand * rot_y2;
            t4->t_loc = t_finger;

            // assigns t_world to each node
            SceneGraphUpdate();

            // apply the calculated world transforms to our boxes
            objs.len = 0;
            objs.Add(CreatePlane(10));

            Wireframe box_root = CreateAABox( 0.2, 0.2, 0.2 );
            box_root.color = COLOR_BLACK;
            box_root.transform = t0->t_world;
            objs.Add(box_root);

            // this gray box rotates with the center:
            /*
            Wireframe box1 = CreateAABox( 0.2, 0.2, 0.2 );
            box1.color = COLOR_GRAY;
            box1.transform = t1->t_world;
            objs.Add(box1);
            */

            Wireframe box2 = CreateAABox( 0.2, 0.2, 0.2 );
            box2.color = COLOR_BLUE;
            box2.transform = t2->t_world;
            objs.Add(box2);

            Wireframe box3 = CreateAABox( 0.2, 0.2, 0.2 );
            box3.color = COLOR_GREEN;
            box3.transform = t3->t_world;
            objs.Add(box3);

            Wireframe box4 = CreateAABox( 0.2, 0.2, 0.2 );
            box4.color = COLOR_RED;
            box4.transform = t4->t_world;
            objs.Add(box4);
        }

        else if (mode == 1) {
            objs.len = 0;
            objs.Add(CreatePlane(10));

            Wireframe box_root = CreateAABox( 0.2, 0.2, 0.2 );
            box_root.color = COLOR_BLACK;
            box_root.transform = t_root * rot_y;
            objs.Add(box_root);

            // this gray box rotates with the center:
            /*
            Wireframe box1 = CreateAABox( 0.2, 0.2, 0.2 );
            box1.color = COLOR_GRAY;
            box1.transform = t_root * rot_y * t_arm;
            objs.Add(box1);
            */

            Wireframe box2 = CreateAABox( 0.2, 0.2, 0.2 );
            box2.color = COLOR_BLUE;
            box2.transform = t_root * rot_y * t_arm * rot_y;
            objs.Add(box2);

            Wireframe box3 = CreateAABox( 0.2, 0.2, 0.2 );
            box3.color = COLOR_GREEN;
            box3.transform = t_root * rot_y * t_arm * rot_y * t_hand * rot_y2;
            objs.Add(box3);

            Wireframe box4 = CreateAABox( 0.2, 0.2, 0.2 );
            box4.color = COLOR_RED;
            box4.transform = t_root * rot_y * t_arm * rot_y * t_hand * rot_y2 * t_finger;
            objs.Add(box4);
        }

        // end 
        Array<Vector3f> segments = WireframeLineSegments(cbui.ctx->a_tmp, objs);
        RenderLineSegmentList(cbui.image_buffer, cam.view, persp.proj, cbui.plf.width, cbui.plf.height, objs, segments);

        CbuiFrameEnd();
    }
    CbuiExit();
}


void TestRotParentIsDifferent() {
    printf("TestRotParentIsDifferent\n");

    CbuiInit("TestSceneGraph", false);
    Perspective persp = ProjectionInit(cbui.plf.width, cbui.plf.height);
    OrbitCamera cam = OrbitCameraInit(persp.aspect);
    Array<Wireframe> objs = InitArray<Wireframe>(cbui.ctx->a_pers, 100);

    SceneGraphInit();
    Transform *ta = SceneGraphAlloc();
    Transform *tb = SceneGraphAlloc();
    Transform *tc = SceneGraphAlloc(tb);
    Transform *td = SceneGraphAlloc(tc);

    while (cbui.running) {
        CbuiFrameStart();
        OrbitCameraUpdate(&cam, cbui.plf.cursorpos.dx, cbui.plf.cursorpos.dy, cbui.plf.left.ended_down, cbui.plf.scroll.yoffset_acc);
        OrbitCameraPan(&cam, persp.fov, persp.aspect, cbui.plf.cursorpos.x_frac, cbui.plf.cursorpos.y_frac, MouseRight().pushed, MouseRight().released);
        // start

        float dtheta = 0.5f;
        Matrix4f rot_y = TransformBuildRotateY(dtheta * cbui.frameno * deg2rad);
        Wireframe box = CreateAABox(0.2f, 0.2f, 0.2f);

        if (true) {
            objs.len = 0;
            objs.Add(CreatePlane(10));

            ta->t_loc = TransformBuildTranslation( { 0, 0.5, -1 } ) * rot_y;
            tb->t_loc = TransformBuildTranslation( { 0.5, 0.5, -1 } );
            tc->t_loc = TransformBuildTranslation( { 0, 0, 1 } );
            td->t_loc = TransformBuildTranslation( { 0, 0, 1 } ) * TransformBuildRotateX(dtheta * 60 * deg2rad);
            SceneGraphUpdate();


            // TODO: Imagine td has rot_rel with ta, the rotating cube.
            //      We want td to stay in place, but also rotate with ta.


            // our world translation matrix
            Vector3f our_w_transl_v3 = TransformGetTranslation(td->t_world);
            Matrix4f our_w_transl = TransformBuildTranslation(our_w_transl_v3);

            // our local rotation matrix
            Matrix4f our_l_rot = TransformBuildRotateX(dtheta * 60 * deg2rad);

            // the parent's world rotation matrix: This is its world matrix with translation removed
            Matrix4f rotparent_w_rot = ta->t_world;
            rotparent_w_rot = TransformSetTranslation(rotparent_w_rot, {0, 0, 0} );

            // our world rotation matrix
            Matrix4f our_w_rot = rotparent_w_rot * our_l_rot;

            // we can now set our world matrix, by combining our trans and rot matrices: 
            Matrix4f our_w = our_w_transl * our_w_rot;

            // set this world matrix to see if things are working: 
            td->t_world = our_w;

            // recover our local matrix wrt. the primary "at-rel" parent
            Matrix4f w_to_atparent = TransformGetInverse( td->Parent()->t_world );
            td->t_loc = w_to_atparent * td->t_world;

            // re-create the world matrices from the loc's
            SceneGraphUpdate();


            Wireframe box_a = box;
            box_a.color = COLOR_BLACK;
            box_a.transform = ta->t_world;
            objs.Add(box_a);

            Wireframe box_b = box;
            box_b.color = COLOR_BLUE;
            box_b.transform = tb->t_world;
            objs.Add(box_b);

            Wireframe box_c = box;
            box_c.color = COLOR_GREEN;
            box_c.transform = tc->t_world;
            objs.Add(box_c);

            Wireframe box_d = box;
            box_d.color = COLOR_RED;
            box_d.transform = td->t_world;
            objs.Add(box_d);
        }

        // end 
        Array<Vector3f> segments = WireframeLineSegments(cbui.ctx->a_tmp, objs);
        RenderLineSegmentList(cbui.image_buffer, cam.view, persp.proj, cbui.plf.width, cbui.plf.height, objs, segments);

        CbuiFrameEnd();
    }
    CbuiExit();
}


void Test_03() {

    //TestUILayoutFeatures();
    //TestSceneGraph();
    TestRotParentIsDifferent();
}
