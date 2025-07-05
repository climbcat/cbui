#include "../lib/jg_baselayer.h"
#include "../src/platform/platform_glfw.h"
#include "../src/geometry/geometry.h"
#include "../src/geometry/gtypes.h"
#include "../src/geometry/wireframe.h"
#include "../src/geometry/scenegraph.h"



void TestPlatformGlfw() {
    printf("TestPlatformGlfw\n");

    MContext *ctx = InitBaselayer();
    ImageBufferInit(ctx->a_life);
    PlafGlfw *plf = PlafGlfwInit("TestPlatformGlfw");


    bool running = true;
    while (running) {


        if (MouseLeft().pushed) {
            printf("click\n");
        }
        if (MouseRight().pushed) {
            printf("right\n");
        }
        if (MouseScroll().yoffset_acc != 0) {
            printf("scroll\n");
        }

        char c = 0;
        while(c = GetChar()) {
            printf("%c", c);
        }
        if (GetEnter()) {
            printf("\n");
        }
        else {
            fflush(stdout);
        }
        u32 fkey;
        if (GetFKey(&fkey)) {
            printf("F%d\n", fkey);
        }

        running = running && !GetEscape();
        PlafGlfwUpdate(plf);
        XSleep(1);
    }


    PlafGlfwTerminate(plf);
}


Ray RayFromTo(Vector3f from, Vector3f to) {
    Ray r = { from, to - from };
    r.dir.Normalize();
    return r;
}

bool FClosePN(f32 val, f32 target_pn) {
    bool close_p = abs(val - target_pn) < 0.0001f;
    bool close_n = abs(val + target_pn) < 0.0001f;
    return close_p || close_n;
}

Array<Ray> TestSLAB() {
    printf("TestSLAB\n");

    MArena _a = ArenaCreate();
    MArena *a = &_a;
    Array<Ray> error_rays = InitArray<Ray>(a, 10000);

    Vector3f x_hat = { 1, 0, 0 };
    Vector3f y_hat = { 0, 1, 0 };
    Vector3f z_hat = { 0, 0, 1 };

    f32 side = 1.0f;
    f32 side_2 = side / 2;
    Wireframe box = CreateAABox(side, side, side);
    f32 src_radius = 1;
    Wireframe sphere = CreateSphere(src_radius);

    RandInit();
    Vector3f src;
    Vector3f tgt = { 1, 0, 0 };

    u32 nhits = 0;
    u32 nmiss = 0;
    u32 nerrs = 0;

    u32 iter = 0;
    while (iter < 100000) {
        // select random point sphere

        src.x = src_radius * RandPM1_f32();
        src.y = src_radius * RandPM1_f32();
        src.z = src_radius * RandPM1_f32();

        if (src.Norm() > 1) {
            continue;
        }
        src.Normalize();

        // construct the ray
        Ray r = RayFromTo(tgt, src);

        // get intersection
        Vector3f in;
        Vector3f out;
        bool hit = BoxCollideSLAB(r, box, &in, &out);

        if (hit) {
            nhits++;

            bool b1 = FClosePN(in.x, side_2);
            bool b2 = FClosePN(in.y, side_2);
            bool b3 = FClosePN(in.z, side_2);
            if (!b1 && !b2 && !b3)  {
                error_rays.Add(r);
                nerrs++;
            }
        }
        else {
            nmiss++;
        }

        // next
        iter++;
        tgt = src;
    }
    printf("hits: %u misses: %u, errors: %u\n", nhits, nmiss, nerrs);

    return error_rays;
}


void TestSceneGraph() {
    printf("TestSceneGraph\n");

    MArena *a_tmp = InitBaselayer()->a_tmp;

    s32 cap = 256;
    Array<SGNode*> node_handles = InitArray<SGNode*>(a_tmp, cap);

    SceneGraphInit();
    for (s32 i = 0; i < cap; ++i) {
        SGNode *n = SceneGraphAlloc();
        node_handles.Add(n);
    }

    for (s32 i = 0; i < cap; ++i) {
        SGNode *n = node_handles.arr[i];
        SceneGraphFree(n);
    }
}


void Test_02() {
    //TestPlatformGlfw();
    //TestSLAB();
    TestSceneGraph();
}
