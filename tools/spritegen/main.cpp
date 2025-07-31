#define ENABLE_GLFW


#include "../../../baselayer/src/baselayer.h"
#include "../../cbui_includes.h"


void LoadInvaders(MArena *a_dest) {

    SpriteSheet *sheet_01 = SS_Sheet(a_dest, StrL("invaders_03.raw"), StrL("invaders_03"), 176, 592, 6);

    SS_Animation(a_dest, StrL("invader_01"), 16, 16, 4);
    SS_FrameDuration(200);
    SS_FrameDuration(200);
    SS_FrameDuration(200);
    SS_FrameDuration(200);

    SS_Animation(a_dest, StrL("invader_02"), 16, 16, 4);
    SS_FrameDuration(200);
    SS_FrameDuration(200);
    SS_FrameDuration(200);
    SS_FrameDuration(200);

    SS_Animation(a_dest, StrL("invader_03"), 16, 16, 5);
    SS_FrameDuration(200);
    SS_FrameDuration(200);
    SS_FrameDuration(200);
    SS_FrameDuration(200);
    SS_FrameDuration(200);

    SS_Animation(a_dest, StrL("invader_04"), 16, 16, 3);
    SS_FrameDuration(200);
    SS_FrameDuration(200);
    SS_FrameDuration(200);

    SS_Animation(a_dest, StrL("invader_05"), 16, 16, 3);
    SS_FrameDuration(200);
    SS_FrameDuration(200);
    SS_FrameDuration(200);

    SS_Animation(a_dest, StrL("invader_06"), 16, 16, 3);
    SS_FrameDuration(200);
    SS_FrameDuration(200);
    SS_FrameDuration(200);

    SS_CloseSheet();

    SS_Print(sheet_01);
}



void RunProgram(bool start_in_fullscreen) {
    cbui = CbuiInit("projname", start_in_fullscreen);

    LoadInvaders(GetContext()->a_life);

    return;

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
    BaselayerAssertVersion(0, 2, 4);
    CbuiAssertVersion(0, 2, 2);

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
        exit(0);
    }
    else {
        InitBaselayer();
        RunProgram(fullscreen);
    }
}
