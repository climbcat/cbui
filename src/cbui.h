#ifndef __CBUI_H__
#define __CBUI_H__


#define CBUI_VERSION_MAJOR 0
#define CBUI_VERSION_MINOR 2
#define CBUI_VERSION_PATCH 2


void CbuiAssertVersion(u32 major, u32 minor, u32 patch) {
    if (
        CBUI_VERSION_MAJOR != major ||
        CBUI_VERSION_MINOR != minor ||
        CBUI_VERSION_PATCH != patch
    ) {
        assert(1 == 0 && "cbui version check failed");
    }
}

void CbuiPrintVersion() {
    printf("%d.%d.%d\n", CBUI_VERSION_MAJOR, CBUI_VERSION_MINOR, CBUI_VERSION_PATCH);
}


#endif
