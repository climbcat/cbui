#include "../release/0.2.0/jg_baselayer.h"

#include "../src/cbui.h"
#include "../src/imui/color.h"
#include "../src/imui/sprite.h"
#include "../src/imui/resource.h"
#include "../src/imui/font.h"
#include "../src/imui/imui.h"
#include "../src/init.h"


void TB_Drawn04() {
    UI_Center();

    UI_LayoutHorizontal();

    UI_Label("l1");
    UI_Label("l2");
    UI_Label("l3");

    Widget *s1 = UI_Sibling();
    s1->w = 100;
    s1->h = 400;

    UI_LayoutVertical(1);
    UI_Label("Test 4");
    UI_Label("Et");
    UI_Label("par");
    UI_Label("linjers");
    UI_Label("tekst.");
    UI_Pop();

    UI_Label("l4");
}

void TB_Drawn03() {
    UI_Center();

    UI_LayoutVertical(-1);
    UI_Label("Test 3: Alight right");
    UI_Label("Et");
    UI_Label("par");
    UI_Label("linjers");
    UI_Label("tekst.");
}

void TB_Drawn02() {
    UI_Center();

    UI_LayoutVertical(0);
    UI_Label("Test 2: Center");
    UI_Label("Et");
    UI_Label("par");
    UI_Label("linjers");
    UI_Label("tekst.");
}

void TB_Drawn01() {
    UI_Center();

    UI_LayoutVertical();
    UI_Label("Test 1: Align Left");
    UI_Label("Et");
    UI_Label("par");
    UI_Label("linjers");
    UI_Label("tekst.");
}


void TestUILayoutFeatures() {

    cbui = CbuiInit("TestUILayoutFeatures", false);
    s32 TB_mode = 0;

    f32 time = 0;
    UI_SetFontSize(FS_24);
    UI_DebugMode(true);

    while (cbui->running) {
        CbuiFrameStart();

        switch (TB_mode) {
        case 0: TB_Drawn01(); break;
        case 1: TB_Drawn02(); break;
        case 2: TB_Drawn03(); break;
        case 3: TB_Drawn04(); break;
        default: break; }

        if (GetSpace()) {
            TB_mode = (TB_mode + 1) % 4;
        }
        if (GetFKey(2)) {
            UI_DebugMode(!g_ui_debugmode);
        }

        CbuiFrameEnd();
    }
    CbuiExit();



}


void Test_03() {
    TestUILayoutFeatures();
}
