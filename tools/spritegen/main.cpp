#define ENABLE_GLFW


#include "../../../baselayer/src/baselayer.h"
//#include "../../chui_includes.h"
#include "../../cbui_includes.h"


void RunProgram(bool start_in_fullscreen) {
    cbui = CbuiInit("projname", start_in_fullscreen);

    // TODO: init

    while (cbui->running) {
        CbuiFrameStart();

        /*
        switch (mode) {
            case TM_TITLE : {
                DoTitleScreen();
            } break;

            case TM_MAIN : {
                DoMainScreen();
            } break;

            case TM_GAMEOVER : {
                DoGameOver();
            } break;

            default: break;
        }
        */

        CbuiFrameEnd();
    }
    CbuiExit();
}

int main (int argc, char **argv) {
    TimeProgram;
    BaselayerAssertVersion(0, 2, 3);
    CbuiAssertVersion(0, 2, 1);

    bool fullscreen = false;
    if (CLAContainsArg("--fullscreen", argc, argv)) {
        fullscreen = true;
    }
    if (CLAContainsArg("--input", argc, argv)) {
        s32 cnt = ParseInt( CLAGetArgValue("--input", argc, argv) );
        printf("input: %d\n", cnt);
        exit(0);
    }
    else if (CLAContainsArg("--help", argc, argv) || CLAContainsArg("-h", argc, argv)) {
        printf("--help:          display help (this text)\n");
        printf("--test:          run test functions\n");
        exit(0);
    }
    else if (CLAContainsArg("--test", argc, argv)) {
        printf("No available tests ...\n");
    }
    else {
        InitBaselayer();
        RunProgram(fullscreen);
    }
}
