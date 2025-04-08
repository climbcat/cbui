#include "../baselayer/baselayer.h"

#include "src/geometry/wireframe.h"
#include "src/platform/platform_glfw.h"
#include "src/geometry/camera.h"

#include "test/test_02.cpp"


Array<Wireframe> CreateSceneObjects(MArena *a_dest) {
    Array<Wireframe> objs = InitArray<Wireframe>(a_dest, 2);
    objs.Add(CreateAAAxes());
    objs.Add(CreateAABox(0.5, 0.5, 0.5));

    return objs;
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


void RenderLineSegment(u8 *image_buffer, Vector3f anchor_a, Vector3f anchor_b, u32 w, u32 h, Color color = COLOR_RED) {
    Vector2f a = {};
    a.x = (anchor_a.x + 1) / 2 * w;
    a.y = (anchor_a.y + 1) / 2 * h;
    Vector2f b = {};
    b.x = (anchor_b.x + 1) / 2 * w;
    b.y = (anchor_b.y + 1) / 2 * h;

    RenderLineRGBA(image_buffer, w, h, a.x, a.y, b.x, b.y, color);
}

void RenderLineSegments(u8 *image_buffer, Array<Wireframe> wireframes, Array<Vector3f> segments_ndc, u32 w, u32 h) {

    u32 wf_segs_idx = 0;
    u32 wf_idx = 0;
    Color wf_color = wireframes.arr[wf_idx].color;
    u32 wf_nsegments = wireframes.arr[wf_idx].nsegments;

    for (u32 i = 0; i < segments_ndc.len / 2; ++i) {

        Vector2f a = {};
        a.x = (segments_ndc.arr[2*i].x + 1) / 2 * w;
        a.y = (segments_ndc.arr[2*i].y + 1) / 2 * h;
        Vector2f b = {};
        b.x = (segments_ndc.arr[2*i + 1].x + 1) / 2 * w;
        b.y = (segments_ndc.arr[2*i + 1].y + 1) / 2 * h;
        RenderLineRGBA(image_buffer, w, h, a.x, a.y, b.x, b.y, wf_color);


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
    Array<Wireframe> objs = CreateSceneObjects(ctx->a_life);

    bool shoot_enabled = true;
    Ray shoot = {};


    shoot = cam.GetRay(plf->cursorpos.x_frac, plf->cursorpos.y_frac);
    printf("shoot! %f %f %f %f %f %f\n", shoot.position.x, shoot.position.y, shoot.position.z, shoot.direction.x, shoot.direction.y, shoot.direction.z);

    bool running = true;
    while (running) {
        ImageBufferClear(plf->width, plf->height);


        if (GetSpace()) {
            shoot = cam.GetRay(plf->cursorpos.x_frac, plf->cursorpos.y_frac);

            printf("shoot! %f %f %f %f %f %f\n", shoot.position.x, shoot.position.y, shoot.position.z, shoot.direction.x, shoot.direction.y, shoot.direction.z);
        }


        if (shoot_enabled) {
            // visualize the shot's position
            RenderLineSegment(
                plf->image_buffer,
                TransformPerspective(cam.vp, Vector3f{ 0, 0, 0 }),
                TransformPerspective(cam.vp, shoot.position),
                plf->width,
                plf->height,
                COLOR_RED);
            // visualize the shot's direction
            RenderLineSegment(
                plf->image_buffer,
                TransformPerspective(cam.vp, shoot.position),
                TransformPerspective(cam.vp, shoot.position + shoot.direction),
                plf->width,
                plf->height,
                COLOR_BLUE);
        }

        if (GetFKey(3)) {
            shoot_enabled = !shoot_enabled;
        }



        // update and render wireframe objects
        Array<Vector3f> segments_ndc = WireframeLineSegments(ctx->a_tmp, objs, cam.vp);
        RenderLineSegments(plf->image_buffer, objs, segments_ndc, plf->width, plf->height);

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
