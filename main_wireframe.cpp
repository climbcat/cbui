#include "../baselayer/baselayer.h"

#include "src/geometry/wireframe.h"
#include "src/platform/platform_glfw.h"
#include "src/geometry/camera.h"

#include "test/test_02.cpp"


inline
bool _CullScreenCoords(u32 pos_x, u32 pos_y, u32 w, u32 h) {
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
    u32 idx = x + stride * y;
    return idx;
}

void RenderPoint(u8 *image_buffer, Vector3f point, u32 w, u32 h, Color color = COLOR_RED) {
    f32 x = (point.x + 1) / 2 * w;
    f32 y = (point.y + 1) / 2 * h;
    ((Color*) image_buffer)[ GetXYIdx(x, y, w) ] = color;
}

void RenderFat3x3(u8 *image_buffer, Vector3f point, u32 w, u32 h, Color color = COLOR_RED) {
    f32 x = (point.x + 1) / 2 * w;
    f32 y = (point.y + 1) / 2 * h;

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
void RenderLineSegment(u8 *image_buffer, Vector3f anchor_a, Vector3f anchor_b, u32 w, u32 h, Color color) {
    Vector2f a = {};
    a.x = (anchor_a.x + 1) / 2 * w;
    a.y = (anchor_a.y + 1) / 2 * h;
    Vector2f b = {};
    b.x = (anchor_b.x + 1) / 2 * w;
    b.y = (anchor_b.y + 1) / 2 * h;

    RenderLineRGBA(image_buffer, w, h, a.x, a.y, b.x, b.y, color);
}

void RenderLineSegmentList(u8 *image_buffer, Array<Wireframe> wireframes, Array<Vector3f> segments_ndc, u32 w, u32 h) {
    u32 wf_segs_idx = 0;
    u32 wf_idx = 0;
    Color wf_color = wireframes.arr[wf_idx].color;
    u32 wf_nsegments = wireframes.arr[wf_idx].nsegments;

    for (u32 i = 0; i < segments_ndc.len / 2; ++i) {
        RenderLineSegment(image_buffer, segments_ndc.arr[2*i], segments_ndc.arr[2*i + 1], w, h, wf_color);

        // update color to match the current wireframe
        wf_segs_idx++;
        if (wf_segs_idx == wf_nsegments) {
            wf_segs_idx = 0;
            wf_idx++;

            wf_nsegments = wireframes.arr[wf_idx].nsegments;
            wf_color = wireframes.arr[wf_idx].color;
        }
    }
}



void RunWireframe() {
    printf("Running wireframe program ...\n");

    MContext *ctx = InitBaselayer();
    ImageBufferInit(ctx->a_life);
    PlafGlfw *plf = PlafGlfwInit();

    OrbitCamera cam = OrbitCameraInit( PlafGlfwGetAspect(plf) );

    Array<Wireframe> objs = InitArray<Wireframe>(ctx->a_pers, 2);
    objs.Add(CreateAAAxes());
    objs.Add(CreateAABox(0.5, 0.5, 0.5));


    bool running = true;
    while (running) {
        ImageBufferClear(plf->width, plf->height);


        Ray shoot = cam.GetRay(plf->cursorpos.x_frac, plf->cursorpos.y_frac);
        Vector3f first;
        for (u32 i = 0; i < objs.len; ++i) {
            Wireframe *box = objs.arr + i;

            Vector3f hit;
            if (WireFrameCollide(shoot, *box, &hit)) {
                box->color = COLOR_BLACK;
                Vector3f hit_ndc = TransformPerspective(cam.vp, hit);
                RenderFat3x3(plf->image_buffer, hit_ndc, plf->width, plf->height);
            }
            else {
                box->color = COLOR_BLUE;
            }
        }


        // update and render wireframe objects
        Array<Vector3f> segments_ndc = WireframeLineSegments(ctx->a_tmp, objs, cam.vp);
        RenderLineSegmentList(plf->image_buffer, objs, segments_ndc, plf->width, plf->height);

        // usr frame end
        cam.SetAspect(plf->width, plf->height);
        OrbitCameraUpdate(&cam, plf->cursorpos.dx, plf->cursorpos.dy, plf->left.ended_down, plf->right.ended_down, plf->scroll.yoffset_acc);

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
