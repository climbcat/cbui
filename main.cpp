#define ENABLE_GLFW


#include <math.h>
#include <assert.h>
#include <signal.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "lib/jg_baselayer.h"

#include "src/geometry/gtypes.h"
#include "src/geometry/geometry.h"
#include "src/geometry/scenegraph.h"

#include "src/cbui.h"
#include "src/imui/color.h"
#include "src/imui/sprite.h"
#include "src/imui/resource.h"
#include "src/imui/font.h"
#include "src/imui/imui.h"
#include "src/archive/init_arc.h"

#include "test/test_01.cpp"


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


int main (int argc, char **argv) {
    TimeProgram;
    BaselayerAssertVersion(0, 2, 2);

    bool force_testing = false;

    if (CLAContainsArg("--help", argc, argv) || CLAContainsArg("-h", argc, argv)) {
        printf("--help:          display help (this text)\n");
        printf("--version:       print library version\n");
        printf("--release:       output a header-only concatenation\n");
        printf("--test:          run available test functions\n");
    }

    else if (CLAContainsArg("--release", argc, argv) || force_testing) {
        
        MArena *a_files = GetContext()->a_life;
        StrInit();

        StrLst *f_sources = NULL;
        f_sources = StrLstPush("../src/cbui.h", f_sources);
        f_sources = StrLstPush("../src/geometry/gtypes.h", f_sources);
        f_sources = StrLstPush("../src/geometry/geometry.h", f_sources);
        f_sources = StrLstPush("../src/geometry/camera.h", f_sources);
        f_sources = StrLstPush("../src/geometry/scenegraph.h", f_sources);
        f_sources = StrLstPush("../src/imui/color.h", f_sources);
        f_sources = StrLstPush("../src/imui/sprite.h", f_sources);
        f_sources = StrLstPush("../src/imui/resource.h", f_sources);
        f_sources = StrLstPush("../src/imui/font.h", f_sources);
        f_sources = StrLstPush("../src/imui/imui.h", f_sources);
        f_sources = StrLstPush("../src/imui/raster.h", f_sources);
        f_sources = StrLstPush("../src/platform/platform_glfw.h", f_sources);
        //f_sources = StrLstPush("../indices.h", f_sources);
        //f_sources = StrLstPush("../octree.h", f_sources);
        f_sources = f_sources->first;

        StrBuff buff = StrBuffInit();
        StrBuffPrint1K(&buff, "/*\n", 0);
        StrBuffAppend(&buff, LoadTextFile(a_files, "../LICENSE"));
        StrBuffPrint1K(&buff, "*/\n\n\n", 0);
        StrBuffPrint1K(&buff, "#ifndef __JG_CBUI_H__\n", 0);
        StrBuffPrint1K(&buff, "#define __JG_CBUI_H__\n\n\n", 0);

        StrBuffPrint1K(&buff, "#include <math.h>\n", 0);
        StrBuffPrint1K(&buff, "#include \"jg_baselayer.h\"\n", 0);

        while (f_sources) {
            StrBuffAppend(&buff, LoadTextFile(a_files, f_sources->GetStr()));
            StrBuffPrint1K(&buff, "\n\n", 0);

            f_sources = f_sources->next;
        }

        StrBuffPrint1K(&buff, "#endif // __JG_CBUI_H__\n", 0);
        SaveFile("jg_cbui.h_OUT", buff.str, buff.len);
    }

    else if (CLAContainsArg("--test", argc, argv) || force_testing) {
        Test();
    }

    else if (CLAContainsArg("--version", argc, argv)) {
        printf("%d.%d.%d\n", CBUI_VERSION_MAJOR, CBUI_VERSION_MINOR, CBUI_VERSION_PATCH);
        exit(0);
    }

    else {
        BoxesAndPointClouds();
    }

    return 0;
}
