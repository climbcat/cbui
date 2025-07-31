#define ENABLE_GLFW


#include "../../../baselayer/src/baselayer.h"
#include "../../cbui_includes.h"


void LoadInvaders(MArena *a_dest, HashMap *map) {
    SS_Sheet(a_dest, map, "invaders_03.raw", "invaders_03", 176, 592, 6);

    SS_Animation(a_dest, "invader_01", 16, 16, 4);
    SS_FrameDuration(200);
    SS_FrameDuration(200);
    SS_FrameDuration(200);
    SS_FrameDuration(200);

    SS_Animation(a_dest, "invader_02", 16, 16, 4);
    SS_FrameDuration(200);
    SS_FrameDuration(200);
    SS_FrameDuration(200);
    SS_FrameDuration(200);

    SS_Animation(a_dest, "invader_03", 16, 16, 5);
    SS_FrameDuration(200);
    SS_FrameDuration(200);
    SS_FrameDuration(200);
    SS_FrameDuration(200);
    SS_FrameDuration(200);

    SS_Animation(a_dest, "invader_04", 16, 16, 3);
    SS_FrameDuration(200);
    SS_FrameDuration(200);
    SS_FrameDuration(200);

    SS_Animation(a_dest, "invader_05", 16, 16, 3);
    SS_FrameDuration(200);
    SS_FrameDuration(200);
    SS_FrameDuration(200);

    SS_Animation(a_dest, "invader_06", 16, 16, 3);
    SS_FrameDuration(200);
    SS_FrameDuration(200);
    SS_FrameDuration(200);

    SS_CloseSheet();
}



void RunProgram(bool start_in_fullscreen) {
    cbui = CbuiInit("projname", start_in_fullscreen);

    HashMap map = InitMap(GetContext()->a_life);
    LoadInvaders(GetContext()->a_life, &map);

    {
        // seems good now:
        MapIter iter = {};
        SpriteSheet *sheet = (SpriteSheet*) MapNextVal(&map, &iter);
        SS_Print(sheet);
        

        f32 d;
        Frame f;

        f = GetAnimationFrame(&map, StrL("invaders_03"), 0, 0, &d);
        printf("Frame 0, 0: sz: %d %d, tex: %f %f %f %f, dur: %f\n", f.h, f.w, f.u0, f.u1, f.v0, f.v1, d);

        f = GetAnimationFrame(&map, StrL("invaders_03"), 1, 2, &d);
        printf("Frame 1, 2: sz: %d %d, tex: %f %f %f %f, dur: %f\n", f.h, f.w, f.u0, f.u1, f.v0, f.v1, d);

        f = GetAnimationFrame(&map, StrL("invaders_03"), 2, 4, &d);
        printf("Frame 2, 4: sz: %d %d, tex: %f %f %f %f, dur: %f\n", f.h, f.w, f.u0, f.u1, f.v0, f.v1, d);
    }


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
