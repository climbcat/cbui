//#include "../release/0.2.0/jg_baselayer.h"
#include "../../baselayer/src/baselayer.h"

#include "../src/cbui.h"
#include "../src/imui/color.h"
#include "../src/imui/sprite.h"
#include "../src/imui/resource.h"
#include "../src/imui/font.h"
#include "../src/imui/imui.h"
#include "../src/init.h"



void TestUILayoutFeatures() {

    CbuiInit("TestUILayoutFeatures", false);
    s32 TB_mode = 0;

    f32 time = 0;
    UI_SetFontSize(FS_24);
    UI_DebugMode(true);
    UI_DebugNames(true);

    while (cbui.running) {
        CbuiFrameStart();

        switch (TB_mode) {
        case 0: {
            UI_Center();

            UI_LayoutVertical();
            UI_Label("Test: Align Left");
            UI_Label("A couple");
            UI_Label("lines");
            UI_Label("of");
            UI_Label("text");
        } break;

        case 1: {
            UI_Center();

            Widget *w = UI_LayoutVertical(0);
            w->SetFlag(WF_DRAW_BACKGROUND_AND_BORDER);
            w->col_bckgrnd = COLOR_BLUE;
            Widget *l = UI_Label("Test: Align Left");
            l->col_text = COLOR_GRAY_50;
            UI_Label("A couple");
            UI_Label("lines");
            UI_Label("of");
            UI_Label("text");
        } break;

        case 2: {
            UI_Center();

            UI_LayoutVertical(-1);
            UI_Label("Test: Alight right");
            UI_Label("A couple");
            UI_Label("lines");
            UI_Label("of");
            UI_Label("text");

        } break;

        case 3: {
            UI_Center();

            UI_LayoutHorizontal();

            UI_Label("Hori");
            UI_Label("zon");
            UI_Label("tal");

            Widget *s0 = UI_Sibling();
            s0->w = 50;
            s0->h = 400;

            UI_LayoutVertical(1);
            UI_Label("Vert-");
            UI_Label("ical");
            UI_Label("");
            UI_Label("layout");
            UI_Pop();

            Widget *s1 = UI_Sibling();
            s1->w = 50;
            s1->h = 400;

            UI_Label("layout");
        } break;

        case 4: {
            UI_Center();

            Widget *vert = UI_LayoutVertical(-1);
            vert->DBG_tag = StrL("vert");

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
            UI_Center();

            Widget *vert = UI_LayoutVertical(0);
            vert->DBG_tag = StrL("vert");

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

        case 6: {
            UI_Center();

            Widget *vert = UI_LayoutVertical();
            vert->DBG_tag = StrL("vert");

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

        case 7: {
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

        case 8: {
            UI_Center();

            Widget *vert = UI_LayoutHorizontal(0);
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

        case 9: {
            UI_Center();

            Widget *vert = UI_LayoutHorizontal(1);
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

        case 10: {
            UI_Center();

            Widget *vert = UI_LayoutVertical(0);
            vert->DBG_tag = StrL("vert");
            vert->w = 500;
            vert->h = 320;

            Widget *s0 = UI_Sibling();
            s0->DBG_tag = StrL("s0");
            s0->w = 400;
            s0->h = 100;

            Widget *exp = UI_ExpanderH();
            exp->features_flg |= WF_EXPAND_VERTICAL;
            exp->DBG_tag = StrL("exp");
            exp->h = 50;

            Widget *s1 = UI_Sibling();
            s1->DBG_tag = StrL("s1");
            s1->w = 300;
            s1->h = 50;

        } break;

        default: break; }

        if (GetSpace()) {
            TB_mode = (TB_mode + 1) % 11;
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
