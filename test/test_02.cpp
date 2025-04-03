#include "../../baselayer/baselayer.h"
#include "../src/platform/platform_glfw.h"


void TestPlatformGlfw() {
    printf("TestPlatformGlfw\n");

    MContext *ctx = InitBaselayer();
    ImageBufferCreate(ctx->a_life);
    PlafGlfw *plf = PlafGlfwInit();


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

void Test() {
    TestPlatformGlfw();
}
