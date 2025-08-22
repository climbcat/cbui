// #define ENABLE_GLFW

#include "jg_cbui.h"


int main (int argc, char **argv) {
    printf("Check: compile and version for cbui header-only release %u.%u.%u\n", CBUI_VERSION_MAJOR, CBUI_VERSION_MINOR, CBUI_VERSION_PATCH);

    BaselayerAssertVersion(0, 2, 4);
    CbuiAssertVersion(0, 2, 2);
}
