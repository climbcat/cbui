#include "lib/jg_baselayer.h"
#include "src/archive/init.h"
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
    bool force_testing = false;

    if (CLAContainsArg("--help", argc, argv) || CLAContainsArg("-h", argc, argv)) {
        printf("--help:          display help (this text)\n");
        printf("--test:          run available test functions\n");
    }
    else if (CLAContainsArg("--test", argc, argv) || force_testing) {
        Test();
    }
    else {
        BoxesAndPointClouds();
    }

    return 0;
}
