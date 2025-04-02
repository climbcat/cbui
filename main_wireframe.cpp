

#include "../baselayer/baselayer.h"

#include "src/geometry/wireframe.h"
#include "src/platform/platform_glfw.h"

#include "test/test_02.cpp"


void RunWireframe() {
    printf("Running wireframe program ...\n");

    MContext *ctx = InitBaselayer();
    PlafGlfw *plf = PlafGlfwInit(ctx->a_life);

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
