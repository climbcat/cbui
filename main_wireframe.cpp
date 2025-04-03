

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
    objs.Add(CreateAAAxes());
    objs.Add(CreateAABox(0.5, 0.5, 0.5));

    return objs;
}


List<Vector3f> UpdateLineSegments(MArena *a_dest, Array<Wireframe> objs) {
    List<Vector3f> segments = InitList<Vector3f>(a_dest, 0);

    for (u32 i = 0; i < objs.len; ++i) {
        Array<Vector3f> wf_segs = WireframeLineSegments(a_dest, objs.arr[i]);
        segments.len += wf_segs.len;
    }

    return segments;
}


void CreateScene() {
    // TODO: we need a view matrix
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
        List<Vector3f> segments_all = UpdateLineSegments(ctx->a_tmp, objs);


        // TODO: render the segments using cam.vp
        ImageBufferClear(plf->width, plf->height);


        // usr frame end
        cam.Update(plf->cursorpos.dx, plf->cursorpos.dy, plf->left.ended_down, plf->right.ended_down, plf->scroll.yoffset_acc);

        // system frame end
        running = running && !GetEscape();
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
