#include "../release/0.2.0/jg_baselayer.h"

#include "../src/cbui.h"
#include "../src/imui/color.h"
#include "../src/imui/sprite.h"
#include "../src/imui/resource.h"
#include "../src/imui/font.h"
#include "../src/imui/imui.h"
#include "../src/init.h"



void TestUILayoutFeatures() {

    cbui = CbuiInit("TestUILayoutFeatures", false);
    s32 TB_mode = 6;

    f32 time = 0;
    UI_SetFontSize(FS_24);
    UI_DebugMode(true);
    UI_DebugNames(true);

    while (cbui->running) {
        CbuiFrameStart();

        switch (TB_mode) {
        case 0: {
            UI_Center();

            UI_LayoutVertical();
            UI_Label("Test 1: Align Left");
            UI_Label("Et");
            UI_Label("par");
            UI_Label("linjers");
            UI_Label("tekst.");
        } break;

        case 1: {
            UI_Center();

            UI_LayoutVertical(0);
            UI_Label("Test 2: Center");
            UI_Label("Et");
            UI_Label("par");
            UI_Label("linjers");
            UI_Label("tekst.");
        } break;

        case 2: {
            UI_Center();

            UI_LayoutVertical(-1);
            UI_Label("Test 3: Alight right");
            UI_Label("Et");
            UI_Label("par");
            UI_Label("linjers");
            UI_Label("tekst.");

        } break;

        case 3: {
            UI_Center();

            UI_LayoutHorizontal();

            UI_Label("Hori");
            UI_Label("zon");
            UI_Label("tal");

            Widget *s1 = UI_Sibling();
            s1->w = 100;
            s1->h = 400;

            UI_LayoutVertical(1);
            UI_Label("Test 4");
            UI_Label("Vert");
            UI_Label("ical");
            UI_Label("layout");
            UI_Pop();

            UI_Label("layout");
        } break;

        case 4: {
            // vertical layout sizing

            UI_Center();

            Widget *vert = UI_LayoutVertical(-1);
            vert->DBG_tag = StrL("vert");

            // NOTE: out-comment to check behavior
            vert->w = 500;
            vert->h = 120;

            Widget *s0 = UI_Sibling();
            s0->DBG_tag = StrL("s0");
            s0->w = 400;
            s0->h = 100;

            Widget *s1 = UI_Sibling();
            s1->DBG_tag = StrL("s1");
            s1->w = 300;
            s1->h = 50;
        } break;

        case 5: {
            // horizontal layout sizing

            UI_Center();

            Widget *vert = UI_LayoutHorizontal(-1);
            vert->DBG_tag = StrL("horiz");

            // NOTE: out-comment to check behavior
            vert->w = 120;
            vert->h = 450;

            Widget *s0 = UI_Sibling();
            s0->DBG_tag = StrL("s0");
            s0->w = 100;
            s0->h = 400;

            Widget *s1 = UI_Sibling();
            s1->DBG_tag = StrL("s1");
            s1->w = 50;
            s1->h = 300;
        } break;

        case 6: {
            UI_Center();

            Widget *vert = UI_LayoutVertical(0);
            vert->DBG_tag = StrL("vert");
            vert->w = 500;
            //vert->h = 320;

            Widget *s0 = UI_Sibling();
            s0->DBG_tag = StrL("s0");
            s0->w = 400;
            s0->h = 100;

            Widget *exp = UI_ExpanderH();
            exp->DBG_tag = StrL("exp");
            exp->h = 50;

            Widget *s1 = UI_Sibling();
            s1->DBG_tag = StrL("s1");
            s1->w = 300;
            s1->h = 50;

            /*
            Widget *h = UI_LayoutHorizontal();
            h->w = 400;

            Widget *t = UI_Sibling();
            t->features_flg |= WF_DRAW_BACKGROUND_AND_BORDER;
            t->sz_border = 1;
            t->col_bckgrnd = COLOR_RED;
            t->w = 100;
            t->h = 100;

            Widget *e = UI_ExpanderH();
            e->h = 100;

            // TODO: try this as well:
            //UI_Center();
            //UI_Pop();

            Widget *u = UI_Sibling();
            u->features_flg |= WF_DRAW_BACKGROUND_AND_BORDER;
            u->sz_border = 1;
            u->col_bckgrnd = COLOR_GREEN;
            u->w = 100;
            u->h = 100;
            */


        } break;

        default: break; }

        if (GetSpace()) {
            TB_mode = (TB_mode + 1) % 7;
        }
        if (GetFKey(2)) {
            UI_DebugMode(!g_ui_debugmode);
        }
        if (GetFKey(3)) {
            UI_DebugNames(!g_ui_debugnames);
        }

        CbuiFrameEnd();
    }
    CbuiExit();
}


void Test_03() {

    TestUILayoutFeatures();
}
