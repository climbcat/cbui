#include "../baselayer/baselayer.h"

#include "src/geometry/wireframe.h"
#include "src/platform/platform_glfw.h"
#include "src/geometry/camera.h"

#include "test/test_02.cpp"


#define COLOR_RED (( Color { RGBA_RED } ))


Wireframe CreateAABox(f32 w, f32 h, f32 d) {
    Wireframe box = {};
    box.transform = Matrix4f_Identity();
    box.type = WFT_BOX;
    box.dimensions = { 0.5f*w, 0.5f*h, 0.5f*d };
    box.color = COLOR_RED;

    return box;
}

Wireframe CreateAAAxes(f32 len = 1.0f) {
    Wireframe axis = {};
    axis.transform = Matrix4f_Identity();
    axis.type = WFT_BOX;
    axis.dimensions = { len, len, len };
    axis.color = COLOR_RED;

    return axis;
}

Array<Wireframe> CreateSceneObjects(MArena *a_dest) {
    Array<Wireframe> objs = InitArray<Wireframe>(a_dest, 2);
    //objs.Add(CreateAAAxes());
    objs.Add(CreateAABox(0.5, 0.5, 0.5));

    return objs;
}


inline
Vector2_s16 NDC2Screen(u32 w, u32 h, Vector3f ndc) {
    Vector2_s16 pos;

    pos.x = (s16) ((ndc.x + 1) / 2 * w);
    pos.y = (s16) ((ndc.y + 1) / 2 * h);

    return pos;
}

Array<Vector2_s16> LineSegmentsToScreenCoords(MArena *a_dest, Array<Vector3f> segments_ndc, u32 w, u32 h) {
    Array<Vector2_s16> segments_screen = InitArray<Vector2_s16>(a_dest, segments_ndc.len);

    for (u32 i = 0; i < segments_ndc.len; ++i) {
        Vector2_s16 sc = NDC2Screen(w, h, segments_ndc.arr[i]);
        segments_screen.Add(sc);
    }

    return segments_screen;
}

inline
bool CullScreenCoords(u32 pos_x, u32 pos_y, u32 w, u32 h) {
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

            if (CullScreenCoords(x, y, w, h)) {
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
        float slope_inv = 1 / slope_ab;

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

            if (CullScreenCoords(x, y, w, h)) {
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


void RunWireframe() {
    printf("Running wireframe program ...\n");

    MContext *ctx = InitBaselayer();
    ImageBufferCreate(ctx->a_life);
    PlafGlfw *plf = PlafGlfwInit();

    OrbitCamera cam = InitOrbitCamera( PlafGlfwGetAspect(plf) );
    Array<Wireframe> objs = CreateSceneObjects(ctx->a_life);

    bool running = true;
    while (running) {
        Array<Vector3f> segments_ndc = WireframeLineSegments(ctx->a_tmp, objs, cam.vp);
        Array<Vector2_s16> segments_screen = LineSegmentsToScreenCoords(ctx->a_tmp, segments_ndc, plf->width, plf->height);


        ImageBufferClear(plf->width, plf->height);
        for (u32 i = 0; i < segments_screen.len / 2; ++i) {
            Vector2_s16 a = segments_screen.arr[2*i];
            Vector2_s16 b = segments_screen.arr[2*i + 1];
            RenderLineRGBA(plf->image_buffer, plf->width, plf->height, a.x, a.y, b.x, b.y, ColorBlue());
        }


        // usr frame end
        cam.Update(plf->cursorpos.dx, plf->cursorpos.dy, plf->left.ended_down, plf->right.ended_down, plf->scroll.yoffset_acc);

        // system frame end
        running = running && !GetEscape() && !GetWindowShouldClose(plf);
        PlafGlfwUpdate(plf);
        ArenaClear(ctx->a_tmp);
        XSleep(1);
    }

    PlafGlfwTerminate(plf);
}


int main (int argc, char **argv) {
    TimeProgram;

    if (CLAContainsArg("--help", argc, argv) || CLAContainsArg("-h", argc, argv)) {
        printf("--help:          display help (this text)\n");
        printf("--test:          run available test functions\n");
    }
    else if (CLAContainsArg("--test", argc, argv)) {
        Test();
    }
    else {
        RunWireframe();
    }

    return 0;
}
