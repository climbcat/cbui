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


int main (int argc, char **argv) {
    TimeProgram;
    BaselayerAssertVersion(0, 2, 3);

    bool force_testing = false;

    if (CLAContainsArg("--help", argc, argv) || CLAContainsArg("-h", argc, argv)) {
        printf("--help:          display help (this text)\n");
        printf("--version:       print library version\n");
        printf("--release:       output a header-only concatenation\n");
        printf("--test:          run enabled test functions\n");
        exit(0);
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
        printf("Use:\n");
        printf("./cbui --help\n");
        exit(0);
    }

    return 0;
}
