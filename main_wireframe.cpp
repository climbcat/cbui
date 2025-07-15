#define ENABLE_GLFW


#include <math.h>
#include "lib/jg_baselayer.h"

#include "src/geometry/wireframe.h"
#include "src/platform/platform_glfw.h"
#include "src/geometry/camera.h"

#include "src/geometry/gtypes.h"
#include "src/geometry/geometry.h"
#include "src/geometry/scenegraph.h"

#include "src/imui/color.h"
#include "src/imui/sprite.h"
#include "src/imui/resource.h"
#include "src/imui/font.h"
#include "src/imui/imui.h"
#include "src/imui/raster.h"

#include "test/test_02.cpp"

#define WF_VERSION_MAJOR 0
#define WF_VERSION_MINOR 0
#define WF_VERSION_PATCH 7


struct WireframeAppState {
    Perspective persp;
    OrbitCamera cam;
    Matrix4f v;
    u32 w;
    u32 h;
    MArena *a_tmp;
};


static WireframeAppState app;
void AppStateUpdate(Matrix4f v, u32 w, u32 h) {
    PerspectiveSetAspectAndP(&app.persp, w, h);
    app.v = v;
    app.w = w;
    app.h = h;
}

void AppInit(MArena *a_tmp, u32 w, u32 h) {
    app.a_tmp = a_tmp;
    app.persp = ProjectionInit(w, h);
    app.cam = OrbitCameraInit(app.persp.aspect);
}

inline
s32 _GetNextNonDisabledWireframeIndex(u32 idx_prev, Array<Wireframe> wireframes) {
    idx_prev++;
    if (wireframes.len <= idx_prev) {
        return -1;
    }

    while ((wireframes.arr + idx_prev)->disabled == true) {
        idx_prev++;

        if (wireframes.len <= idx_prev) {
            return -1;
        }
    }
    return idx_prev;
}
void RenderLineSegmentList(u8 *image_buffer, Matrix4f view, Matrix4f proj, u32 w, u32 h, Array<Wireframe> wireframes, Array<Vector3f> segments) {
    if (wireframes.len == 0) {
        return;
    }    
    Ray view_plane = { Vector3f { 0, 0, 0.1 }, Vector3f { 0, 0, 1 } };

    // set up wireframe properties
    s32 wf_segs_idx = 0;
    s32 wf_idx = -1;
    wf_idx = _GetNextNonDisabledWireframeIndex(wf_idx, wireframes);
    Color wf_color = wireframes.arr[wf_idx].color;
    WireFrameRenderStyle wf_style = wireframes.arr[wf_idx].style;
    u32 wf_nsegments = wireframes.arr[wf_idx].nsegments;

    for (u32 i = 0; i < segments.len / 2; ++i) {
        Vector3f p1_cam = TransformInversePoint(view, segments.arr[2*i]);
        Vector3f p2_cam = TransformInversePoint(view, segments.arr[2*i + 1]);

        bool visible1 = PointSideOfPlane(p1_cam, view_plane);
        bool visible2 = PointSideOfPlane(p2_cam, view_plane);

        if (visible1 == true || visible2 == true) {
            if (visible1 == false && visible2 == true) {
                Ray segment = { p2_cam, p1_cam - p2_cam };
                f32 t = 0;
                p1_cam = RayPlaneIntersect(segment, view_plane.pos, view_plane.dir, &t);
            }
            else if (visible1 == true && visible2 == false) {
                Ray segment = { p1_cam, p2_cam - p1_cam };
                f32 t = 0;
                p2_cam = RayPlaneIntersect(segment, view_plane.pos, view_plane.dir, &t);
            }
            Vector3f p1_ndc = TransformPerspective(proj, p1_cam);
            Vector3f p2_ndc = TransformPerspective(proj, p2_cam);

            Vector2f a = {};
            a.x = (p1_ndc.x + 1) / 2 * w;
            a.y = (p1_ndc.y + 1) / 2 * h;
            Vector2f b = {};
            b.x = (p2_ndc.x + 1) / 2 * w;
            b.y = (p2_ndc.y + 1) / 2 * h;

            if (wf_style == WFR_SLIM) {
                RenderLineRGBA(image_buffer, w, h, a.x, a.y, b.x, b.y, wf_color);
            }
            else if (wf_style == WFR_FAT) {
                RenderLineRGBA(image_buffer, w, h, a.x, a.y, b.x, b.y, wf_color);
                RenderLineRGBA(image_buffer, w, h, a.x+1, a.y, b.x+1, b.y, wf_color);
                RenderLineRGBA(image_buffer, w, h, a.x, a.y+1, b.x, b.y+1, wf_color);
            }
        }

        // update object-specific properties s.a. colour, style
        wf_segs_idx++;
        if (wf_segs_idx == wf_nsegments) {
            wf_idx = _GetNextNonDisabledWireframeIndex(wf_idx, wireframes);
            if (wf_idx == -1) {
                continue;
            }

            wf_segs_idx = 0;
            wf_nsegments = wireframes.arr[wf_idx].nsegments;
            wf_color = wireframes.arr[wf_idx].color;
            wf_style = wireframes.arr[wf_idx].style;
        }
    }
}


inline
void RenderFatPoint3x3(Vector3f point, Color color = COLOR_RED) {
    RenderFatPoint3x3(g_image_buffer, app.v, app.persp.proj, point, app.w, app.h, color);
}

inline
void RenderLineSegment(Vector3f p1, Vector3f p2, Color color) {
    RenderLineSegment(g_image_buffer, app.v, app.persp.proj, p1, p2, app.w, app.h, color);
}

void RenderWireframes(Array<Wireframe> wireframes) {
    Array<Vector3f> segments = WireframeLineSegments(app.a_tmp, wireframes);

    // insert the globals
    RenderLineSegmentList(g_image_buffer, app.v, app.persp.proj, app.w, app.h, wireframes, segments);
}

struct DragState {
    Wireframe *selected;
    Wireframe *selected_prev;
    bool drag_enabled;
    Vector3f drag_push;
    Vector3f drag_push_objzero;
    Vector3f drag;
    Vector3f hit;
};

Vector3f DragStateUpdate(DragState *sd, Array<Wireframe> objs, Matrix4f view, Vector3f campos, f32 fov, f32 aspect, f32 x_frac, f32 y_frac) {
    Wireframe *selected = sd->selected;
    Wireframe *selected_prev = sd->selected_prev;
    bool drag_enabled = sd->drag_enabled;
    Vector3f drag_push = sd->drag_push;
    Vector3f drag_push_objzero = sd->drag_push_objzero;
    Vector3f drag = sd->drag;
    Vector3f hit = sd->hit;

    if (MouseLeft().released) {
        drag_enabled = false;
        drag = {};
        hit = {};
    }

    Vector3f delta = Vector3f_Zero();
    if (drag_enabled && MouseLeft().ended_down) {
        Vector3f drag_nxt = CameraGetPointAtDepth(view, fov, aspect, drag_push, x_frac, y_frac);
        delta = drag_nxt - drag;
        drag = drag_nxt;
    }

    Ray shoot = CameraGetRay(view, fov, aspect, x_frac, y_frac);

    bool collided = false;
    f32 dist = 0;
    Wireframe *clicked = NULL;
    for (u32 i = 0; i < objs.len; ++i) {
        Wireframe *obj = objs.arr + i;

        if (WireFrameCollide(shoot, *obj, &hit)) {
            if (MouseLeft().pushed) {
                if ((collided == false) || (Vector3f::NormSquared(hit - campos) < dist)) {
                    // first hit; then closer to cam pos
                    drag = hit;
                    dist = Vector3f::NormSquared(campos - hit);
                    clicked = obj;
                }
            }

            collided = true;
        }
    }

    // select
    if (clicked) {
        selected = clicked;

        drag_push = hit;
        drag_push_objzero = TransformPoint(clicked->transform, {});
        drag_enabled = true;
    }

    // de-selection
    if (collided == false && MouseLeft().pushed) {
        selected = NULL;
    }

    // selection changed
    if (selected != selected_prev) {
        if (selected_prev) {
            selected_prev->style = WFR_SLIM;

            selected_prev = NULL;
        }
        if (selected) {
            selected->style = WFR_FAT;

            selected_prev = selected;
        }
    }

    if (selected && (GetBackspace() || GetDelete())) {
        selected->disabled = true;
        selected = NULL;
    }

    sd->selected = selected;
    sd->selected_prev = selected_prev;
    sd->drag_enabled = drag_enabled;
    sd->drag_push = drag_push;
    sd->drag_push_objzero = drag_push_objzero;
    sd->drag = drag;
    sd->hit = hit;

    return delta;
}
inline
Vector3f DragStateUpdate(DragState *sd, Array<Wireframe> objs, Vector3f campos, f32 x_frac, f32 y_frac) {
    return DragStateUpdate(sd, objs, app.v, campos, app.persp.fov, app.persp.aspect, x_frac, y_frac);
}

void InitResources(MArena *a_tmp, MArena *a_life, u32 w, u32 h, u8* image_buffer, u64 *frameno) {
    InitImUi(w, h, frameno);
    ImageRGBA render_target = { (s32) w, (s32) h, (Color*) image_buffer };
    SpriteRender_Init(a_life);

    g_texture_map = InitMap(a_life, MAX_RESOURCE_CNT);
    g_resource_map = InitMap(a_life, MAX_RESOURCE_CNT);

    // load & check resource file
    ResourceStreamHandle hdl = ResourceStreamLoadAndOpen(a_tmp, a_life, "all.res");
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
}

void RunWireframe() {
    printf("Running wireframe program ...\n");

    // init
    MContext *ctx = InitBaselayer();
    ImageBufferInit(ctx->a_life);
    PlafGlfw *plf = PlafGlfwInit("Wireframe Demo");
    AppInit(ctx->a_tmp, plf->width, plf->height);
    app.cam.radius = 10;
    app.cam.theta = 50;
    app.cam.phi = -40;
    DragState drag = {};
    u64 frameno = 0;


    // LEGACY init graphics 
    InitResources(ctx->a_tmp, ctx->a_life, plf->width, plf->height, g_image_buffer, &frameno);


    // scene objects
    Array<Wireframe> objs = InitArray<Wireframe>(ctx->a_pers, 100);

    objs.Add(CreateAAAxes());
    objs.Add(CreatePlane(10));

    Wireframe box = CreateAABox( 0.5, 0.5, 0.5 );
    box.transform = TransformBuildTranslation({ 0.7, 0.5, -0.7 });
    box.color = COLOR_RED;
    objs.Add(box);

    Wireframe ball = CreateSphere( 0.4 );
    ball.transform = TransformBuildTranslation({ 0.7, 0.5, 0.7 });
    ball.color = COLOR_BLUE;
    objs.Add(ball);

    Wireframe cylinder = CreateCylinder( 0.3, 0.6 );
    cylinder.transform = TransformBuildTranslation({ -0.7, 0.5, 0 });
    cylinder.color = COLOR_GREEN;
    objs.Add(cylinder);


    // app mode
    s32 mode = 0;
    bool dbg_tpush = false;
    bool dbg_tpush2 = false;


    // graphics loop
    bool running = true;
    while (running) {
        // frame start
        ArenaClear(ctx->a_tmp);
        frameno++;
        PlafGlfwUpdate(plf);
        ImageBufferClear(plf->width, plf->height);
        running = running && !GetEscape() && !GetWindowShouldClose(plf);
        AppStateUpdate(app.cam.view, plf->width, plf->height);

        // frame body
        if (mode == 0) {
            Vector3f drag_delta = DragStateUpdate(&drag, objs, app.cam.position, plf->cursorpos.x_frac, plf->cursorpos.y_frac);
            if (drag.selected && drag.drag_enabled) {

                if (ModCtrl()) {
                    drag.selected->transform.m[0][3] += 0;
                    drag.selected->transform.m[1][3] += drag_delta.y;
                    drag.selected->transform.m[2][3] += 0;
                }
                else if (ModAlt()) {
                    drag.selected->transform.m[0][3] += drag_delta.x;
                    drag.selected->transform.m[1][3] += drag_delta.y;
                    drag.selected->transform.m[2][3] += drag_delta.z;
                }
                else {
                    Vector3f plane_origo = { 0, drag.drag_push.y, 0 };
                    Vector3f plane_normal = y_hat;

                    Vector3f proj = RayPlaneIntersect(RayFromTo(app.cam.position_world, drag.drag), plane_origo, plane_normal); 
                    Vector3f new_pos = proj + (drag.drag_push_objzero - drag.drag_push);

                    drag.selected->transform.m[0][3] = new_pos.x;
                    drag.selected->transform.m[1][3] = new_pos.y;
                    drag.selected->transform.m[2][3] = new_pos.z;
                }
            }

            if (drag.drag_enabled == false) {
                OrbitCameraUpdate(&app.cam, plf->cursorpos.dx, plf->cursorpos.dy, plf->left.ended_down, plf->scroll.yoffset_acc);
                OrbitCameraPan(&app.cam, app.persp.fov, app.persp.aspect, plf->cursorpos.x_frac, plf->cursorpos.y_frac, MouseRight().pushed, MouseRight().released);
            }

            // render objects
            RenderWireframes(objs);

            // render DBG
            RenderFatPoint3x3(drag.drag, COLOR_BLACK);

            if (drag.drag.IsNonZero()) {
                RenderFatPoint3x3(drag.drag, COLOR_RED);
                RenderLineSegment(drag.drag - drag_delta, drag.drag, COLOR_BLACK);
            }


            g_mouse_x = plf->cursorpos.x;
            g_mouse_y = plf->cursorpos.y;
            g_mouse_down = MouseLeft().ended_down;
            g_mouse_pushed = MouseLeft().pushed;

            Widget *x;
            if (UI_CrossButton("?", &x)) {
                mode = 1;
            }
            x->w = 25;
            x->w = 25;
            x->x0 = -2;
            x->y0 = 2;
            UI_FrameEnd(app.a_tmp, app.w, app.h);
            SpriteRender_BlitAndCLear(InitImageRGBA(app.w, app.h, g_image_buffer));

            if (GetFKey(1)) { mode = 1; }
        }

        else if (mode == 1) {
            g_mouse_x = plf->cursorpos.x;
            g_mouse_y = plf->cursorpos.y;
            g_mouse_down = MouseLeft().ended_down;
            g_mouse_pushed = MouseLeft().pushed;

            UI_LayoutExpandCenter();

            bool close;
            UI_CoolPopUp(400, 170, 20, &close);
            if (close) {
                mode = 0;
            }

            UI_Label("Rotate         Mouse Left");
            UI_Label("Pan            Mouse Right");
            UI_Label("Drag Object    Mouse Left");
            UI_Label("Drag Vertical  Hold Ctrl");
            UI_Label("F1             Help Menu");
            UI_Label("Esc            Quit");

            UI_FrameEnd(app.a_tmp, app.w, app.h);
            SpriteRender_BlitAndCLear(InitImageRGBA(app.w, app.h, g_image_buffer));

            if (GetFKey(1)) { mode = 0; }
        }

        else {
            mode = 0;
        }

        //
        // frame end
        XSleep(1);
    }

    PlafGlfwTerminate(plf);
}


int main (int argc, char **argv) {
    BaselayerAssertVersion(0, 2, 2);

    TimeProgram;

    if (CLAContainsArg("--help", argc, argv) || CLAContainsArg("-h", argc, argv)) {
        printf("--help:          display help (this text)\n");
        printf("--test:          run available test functions\n");
        printf("--version:       print the application version\n");
    }
    else if (CLAContainsArg("--test", argc, argv)) {
        Test_02();
    }
    else if (CLAContainsArg("--version", argc, argv)) {
        printf("%d.%d.%d\n", WF_VERSION_MAJOR, WF_VERSION_MINOR, WF_VERSION_PATCH);
        exit(0);
    }
    else {
        RunWireframe();
    }

    return 0;
}
