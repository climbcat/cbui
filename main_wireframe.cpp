#include "lib/jg_baselayer.h"

#include "src/geometry/wireframe.h"
#include "src/platform/platform_glfw.h"
#include "src/geometry/camera.h"

#include "test/test_02.cpp"

#define WF_VERSION_MAJOR 0
#define WF_VERSION_MINOR 0
#define WF_VERSION_PATCH 3


struct WireframeApp {
    Matrix4f v;
    Matrix4f p;
    u32 w;
    u32 h;
    MArena *a_tmp;
};
static WireframeApp app;
void AppUpdate(Matrix4f v, Matrix4f p, u32 w, u32 h) {
    app.v = v;
    app.p = p;
    app.w = w;
    app.h = h;
}
void AppInit(MArena *a_tmp) {
    app.a_tmp = a_tmp;
}


inline
bool _CullScreenCoords(u32 pos_x, u32 pos_y, u32 w, u32 h) {
    // returns true if the coordinate is out of range
    bool not_result = pos_x >= 0 && pos_x < w && pos_y >= 0 && pos_y < h;
    return !not_result;
}
void RenderLineRGBA(u8* image_buffer, u16 w, u16 h, s16 ax, s16 ay, s16 bx, s16 by, Color color) {

    // initially working from a to b
    // there are four cases:
    // 1: slope <= 1, ax < bx
    // 2: slope <= 1, ax > bx 
    // 3: slope > 1, ay < by
    // 4: slope > 1, ay > by 

    f32 slope_ab = (f32) (by - ay) / (bx - ax);

    if (abs(slope_ab) <= 1) {
        // draw by x
        f32 slope = slope_ab;

        // swap?
        if (ax > bx) {
            u16 swapx = ax;
            u16 swapy = ay;

            ax = bx;
            ay = by;
            bx = swapx;
            by = swapy;
        }

        s16 x, y;
        u32 pix_idx;
        for (s32 i = 0; i <= bx - ax; ++i) {
            x = ax + i;
            y = ay + (s16) floor(slope * i);

            if (_CullScreenCoords(x, y, w, h)) {
                continue;
            }

            pix_idx = x + y*w;
            image_buffer[4 * pix_idx + 0] = color.r;
            image_buffer[4 * pix_idx + 1] = color.g;
            image_buffer[4 * pix_idx + 2] = color.b;
            image_buffer[4 * pix_idx + 3] = color.a;
        }
    }
    else {
        // draw by y
        f32 slope_inv = 1 / slope_ab;

        // swap a & b ?
        if (ay > by) {
            u16 swapx = ax;
            u16 swapy = ay;

            ax = bx;
            ay = by;
            bx = swapx;
            by = swapy;
        }

        s16 x, y;
        u32 pix_idx;
        for (u16 i = 0; i <= by - ay; ++i) {
            y = ay + i;
            x = ax + (s16) floor(slope_inv * i);

            if (_CullScreenCoords(x, y, w, h)) {
                continue;
            }

            pix_idx = x + y*w;
            image_buffer[4 * pix_idx + 0] = color.r;
            image_buffer[4 * pix_idx + 1] = color.g;
            image_buffer[4 * pix_idx + 2] = color.b;
            image_buffer[4 * pix_idx + 3] = color.a;
        }
    }
}

inline
u32 GetXYIdx(f32 x, f32 y, u32 stride) {
    u32 idx = floor(x) + stride * floor(y);
    return idx;
}

void RenderPoint(u8 *image_buffer, Vector3f point_ndc, u32 w, u32 h, Color color = COLOR_RED) {
    f32 x = (point_ndc.x + 1) / 2 * w;
    f32 y = (point_ndc.y + 1) / 2 * h;
    ((Color*) image_buffer)[ GetXYIdx(x, y, w) ] = color;
}

f32 PointPlaneSign(Vector3f point, Ray plane) {
    Vector3f diff = (plane.position - point);
    diff.Normalize();
    f32 result = diff.Dot(plane.direction);

    return result;
}

void RenderFatPoint3x3(u8 *image_buffer, Matrix4f view, Matrix4f proj, Vector3f point, u32 w, u32 h, Color color = COLOR_RED) {
    Vector3f point_cam = TransformInversePoint(view, point);

    Ray view_plane = { Vector3f { 0, 0, 0.1 }, Vector3f { 0, 0, 1 } };
    Ray view_plane_far = { Vector3f { 0, 0, 1 }, Vector3f { 0, 0, 1 } };

    f32 sign = PointPlaneSign(point_cam, view_plane);
    if (sign > 0) {
        return;
    }

    Vector3f point_ndc = TransformPerspective(proj, point_cam);

    f32 x = (point_ndc.x + 1) / 2 * w;
    f32 y = (point_ndc.y + 1) / 2 * h;

    for (s32 i = -1; i < 2; ++i) {
        for (s32 j = -1; j < 2; ++j) {
            if (x + i < 0 || y + j < 0) {
                continue;;
            }
            if (x + i >= w || y + j >= h) {
                continue;;
            }
            ((Color*) image_buffer)[ GetXYIdx(x + i, y + j, w) ] = color;
        }
    }
}

inline
void RenderLineSegment(u8 *image_buffer, Matrix4f view, Matrix4f proj, Vector3f p1, Vector3f p2, u32 w, u32 h, Color color) {
    Vector3f p1_cam = TransformInversePoint(view, p1);
    Vector3f p2_cam = TransformInversePoint(view, p2);

    Ray view_plane = { Vector3f { 0, 0, 0.1 }, Vector3f { 0, 0, 1 } };

    f32 sign1 = PointPlaneSign(p1_cam, view_plane);
    f32 sign2 = PointPlaneSign(p2_cam, view_plane);
    if (sign1 > 0 && sign2 > 0) {
        return;
    }
    else if (sign1 > 0 && sign2 < 0) {
        Ray segment = { p2_cam, p1_cam - p2_cam };
        f32 t = 0;
        p1_cam = RayPlaneIntersect(segment, view_plane.position, view_plane.direction, &t);
    }
    else if (sign1 < 0 && sign2 > 0) {
        Ray segment = { p1_cam, p2_cam - p1_cam };
        f32 t = 0;
        p2_cam = RayPlaneIntersect(segment, view_plane.position, view_plane.direction, &t);
    }

    Vector3f p1_ndc = TransformPerspective(proj, p1_cam);
    Vector3f p2_ndc = TransformPerspective(proj, p2_cam);

    Vector2f a = {};
    a.x = (p1_ndc.x + 1) / 2 * w;
    a.y = (p1_ndc.y + 1) / 2 * h;
    Vector2f b = {};
    b.x = (p2_ndc.x + 1) / 2 * w;
    b.y = (p2_ndc.y + 1) / 2 * h;

    RenderLineRGBA(image_buffer, w, h, a.x, a.y, b.x, b.y, color);
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

    // set up wireframe properties
    s32 wf_segs_idx = 0;
    s32 wf_idx = -1;
    wf_idx = _GetNextNonDisabledWireframeIndex(wf_idx, wireframes);
    Color wf_color = wireframes.arr[wf_idx].color;
    WireFrameRenderStyle wf_style = wireframes.arr[wf_idx].style;
    u32 wf_nsegments = wireframes.arr[wf_idx].nsegments;

    for (u32 i = 0; i < segments.len / 2; ++i) {
        Vector3f p1 = segments.arr[2*i];
        Vector3f p2 = segments.arr[2*i + 1];

        RenderLineSegment(image_buffer, view, proj, p1, p2, w, h, wf_color);

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

void RenderWireframes(Array<Wireframe> wireframes) {
    Array<Vector3f> segments = WireframeLineSegments(app.a_tmp, wireframes);

    // insert the globals
    RenderLineSegmentList(g_image_buffer, app.v, app.p, app.w, app.h, wireframes, segments);
}


struct DragState {
    Wireframe *selected;
    Wireframe *selected_prev;
    bool drag_enabled;
    Vector3f drag_push;
    Vector3f drag;
    Vector3f drag_prev;
    Vector3f drag_nxt;
    Vector3f hit;
};

DragState DragStateUpdate(DragState sd, Array<Wireframe> objs, Matrix4f view, Vector3f campos, f32 fov, f32 aspect, f32 x_frac, f32 y_frac) {
    Wireframe *selected = sd.selected;
    Wireframe *selected_prev = sd.selected_prev;
    bool drag_enabled = sd.drag_enabled;
    Vector3f drag_push = sd.drag_push;
    Vector3f drag = sd.drag;
    Vector3f drag_prev = sd.drag_prev;
    Vector3f drag_nxt = sd.drag_nxt;
    Vector3f hit = sd.hit;

    if (MouseLeft().released) {
        drag_enabled = false;
        drag = {};
        drag_nxt = {};
        drag_prev = {};
        hit = {};
    }

    if (drag_enabled && MouseLeft().ended_down) {
        drag_nxt = CameraGetPointAtDepth( view, fov, aspect, drag_push, x_frac, y_frac);

        Vector3f delta = drag_nxt - drag;

        selected->transform.m[0][3] += delta.x;
        selected->transform.m[1][3] += delta.y;
        selected->transform.m[2][3] += delta.z;

        drag_prev = drag;
        drag = drag_nxt;
    }

    Ray shoot = CameraGetRay(view, fov, aspect, x_frac, y_frac);

    bool collided = false;
    f32 dist = 0;
    for (u32 i = 0; i < objs.len; ++i) {
        Wireframe *obj = objs.arr + i;

        if (WireFrameCollide(shoot, *obj, &hit)) {
            if (MouseLeft().pushed) {
                if ((collided == false) || (Vector3f::NormSquared(hit - campos) < dist)) {
                    // first hit; then closer to cam pos
                    drag = hit;
                    dist = Vector3f::NormSquared(campos - hit);

                    selected = obj;
                    drag_push = hit;
                    drag_enabled = true;
                }
            }

            collided = true;
        }
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

    sd.selected = selected;
    sd.selected_prev = selected_prev;
    sd.drag_enabled = drag_enabled;
    sd.drag_push = drag_push;
    sd.drag = drag;
    sd.drag_prev = drag_prev;
    sd.drag_nxt = drag_nxt;
    sd.hit = hit;
    return sd;
}


void RunWireframe() {
    printf("Running wireframe program ...\n");

    // system init
    MContext *ctx = InitBaselayer();
    AppInit(ctx->a_tmp);
    ImageBufferInit(ctx->a_life);
    PlafGlfw *plf = PlafGlfwInit();
    Perspective proj = ProjectionInit(plf->width, plf->height);

    // cameras
    OrbitCamera cam = OrbitCameraInit( proj.aspect );

    // scene objects
    Array<Wireframe> objs = InitArray<Wireframe>(ctx->a_pers, 100);

    Wireframe box = CreateAABox( 0.5, 0.5, 0.5 );
    box.transform = TransformBuildTranslationOnly({ 0.7, 0.7, 0.7 });
    objs.Add(box);

    objs.Add(CreateAAAxes());
    objs.Add(CreatePlane(10));

    Wireframe ball = CreateSphere( 0.5 );
    ball.transform = TransformBuildTranslationOnly({ 0.7, 0.7, -0.7 });
    objs.Add(ball);

    Wireframe cylinder = CreateCylinder( 0.2, 0.7 );
    cylinder.transform = TransformBuildTranslationOnly({ -0.5, 0.5, -0.5 });
    objs.Add(cylinder);

    Wireframe eye = CreateEye( 0.05, 0.1 );
    eye.transform = TransformBuildTranslationOnly({ -0.5, 1, 1 });
    objs.Add(eye);

    DragState drag = {};

    // graphics loop
    bool running = true;
    while (running) {
        drag = DragStateUpdate(drag, objs, cam.view, cam.position, proj.fov, proj.aspect, plf->cursorpos.x_frac, plf->cursorpos.y_frac);

        PerspectiveSetAspectAndP(&proj, plf->width, plf->height);
        if (drag.drag_enabled == false) {
            OrbitCameraUpdate(&cam, plf->cursorpos.dx, plf->cursorpos.dy, plf->left.ended_down, plf->right.ended_down, plf->scroll.yoffset_acc);
        }

        // update and render wireframe objects
        RenderWireframes(objs);

        RenderFatPoint3x3(plf->image_buffer, cam.view, proj.p, drag.drag, plf->width, plf->height, COLOR_BLACK);
        if (drag.drag_prev.x != 0 || drag.drag_prev.y != 0 || drag.drag_prev.z != 0) {
            RenderFatPoint3x3(plf->image_buffer, cam.view, proj.p, drag.drag_nxt, plf->width, plf->height, COLOR_RED);
            RenderLineSegment(plf->image_buffer, cam.view, proj.p, drag.drag_prev, drag.drag, plf->width, plf->height, COLOR_BLACK);
        }

        // frae end
        PlafGlfwUpdate(plf);
        AppUpdate(cam.view, proj.p, plf->width, plf->height);

        ArenaClear(ctx->a_tmp);
        ImageBufferClear(plf->width, plf->height);

        XSleep(1);
        running = running && !GetEscape() && !GetWindowShouldClose(plf);
    }

    PlafGlfwTerminate(plf);
}


int main (int argc, char **argv) {
    BaselayerAssertVersion(0, 1, 0);

    TimeProgram;

    if (CLAContainsArg("--help", argc, argv) || CLAContainsArg("-h", argc, argv)) {
        printf("--help:          display help (this text)\n");
        printf("--test:          run available test functions\n");
        printf("--version:       print the application version\n");
    }
    else if (CLAContainsArg("--test", argc, argv)) {
        Test();
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
