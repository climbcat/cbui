// #define ENABLE_GLFW

#include "jg_cbui.h"


int main (int argc, char **argv) {
    printf("Compile and version check for cbui %u.%u.%u\n", CBUI_VERSION_MAJOR, CBUI_VERSION_MINOR, CBUI_VERSION_PATCH);

    BaselayerAssertVersion(0, 2, 3);
    CbuiAssertVersion(0, 2, 0);
}
