#include <math.h>
#include <assert.h>

#include "../../../baselayer/src/baselayer.h"


#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"


void RawImage() {

}


int main (int argc, char **argv) {
    TimeProgram;

    if (argc == 2) {
        char *filename_in  = argv[1];

        // attempt to load the given word as an image file
        s32 bitmap_w;
        s32 bitmap_h;
        s32 channels_in_file;
        u8 *data = stbi_load(filename_in, &bitmap_w, &bitmap_h, &channels_in_file, 4);

        if (data == NULL) {
            printf("could not load: %s\n", filename_in);
        }
        else {
            // create the output filename
            StrInit();

            Str f_in = StrL(filename_in);
            Str f_out = StrPathBuild( StrL("."), StrBasename(f_in), StrL("raw"));
            StrPrint("input: ", f_in, "\n");
            StrPrint("output: ", f_out, "\n");

            SaveFile(StrZ(f_out), data, bitmap_w * bitmap_h);
        }
    }
    else {
        printf("Usage: png2raw [filename]\n");
    }

}

