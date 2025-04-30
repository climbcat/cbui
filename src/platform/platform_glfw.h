#ifndef __PLATFORM_GLFW_H__
#define __PLATFORM_GLFW_H__


#include <assert.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "shaders.h"


struct MousePosition {
    f32 x;
    f32 y;
    f32 dx;
    f32 dy;
    f32 x_frac; // [-1, 1]
    f32 y_frac; // [-1, 1]
    f32 dx_frac;
    f32 dy_frac;
};

struct Button {
    bool pushed;
    bool released;
    bool ended_down;
    u32 pushes;
};

struct Scroll {
    double yoffset_acc;
    u32 steps_down;
    u32 steps_up;
};

struct AsciiKeys {
    u8 keys_cnt;
    u8 keys_idx;
    char keys[32]; // max 32 keystrokes per frame ...

    void Put(char c) {
        if (keys_cnt < 16) {
            keys[keys_cnt++] = c;
        }
    }
    char Get(s32 *mods = NULL) {
        if (keys_cnt && (keys_idx < keys_cnt)) {
            char c = keys[keys_idx++];
            return c;
        }
        else {
            return 0;
        }
    }
};

struct ActionKeys {
    bool esc;
    bool enter;
    bool backspace;
    bool del;
    bool space;
    bool mod_ctrl;
    bool mod_shift;
    bool mod_alt;
    u8 fkey;

    void ResetButKeepMods() {
        bool _mod_ctrl = mod_ctrl;
        bool _mod_shift = mod_shift;
        bool _mod_alt = mod_alt;
        *this = {};
        this->mod_ctrl = _mod_ctrl;
        this->mod_shift = _mod_shift;
        this->mod_alt = _mod_alt;
    }
};


struct PlafGlfw {
    GLFWwindow* window;
    bool fullscreen;

    Button left;
    Button right;
    Scroll scroll;
    AsciiKeys keys;
    ActionKeys akeys;

    MousePosition cursorpos;

    ScreenProgram screen;
    u32 width;
    u32 height;
    u32 width_cache;
    u32 height_cache;
    s32 window_xpos;
    s32 window_ypos;
    u8 *image_buffer;
};


inline PlafGlfw *_GlfwWindowToUserPtr(GLFWwindow* window) {
    PlafGlfw *plaf = (PlafGlfw*) glfwGetWindowUserPointer(window);
    return plaf;
}

void MouseButtonCallBack(GLFWwindow* window, int button, int action, int mods) {
    PlafGlfw *plaf = _GlfwWindowToUserPtr(window);

    // get button
    Button *btn = NULL;
    if (button == GLFW_MOUSE_BUTTON_1) {
        btn = &plaf->left;
    }
    else if (button == GLFW_MOUSE_BUTTON_2) {
        btn = &plaf->right;
    }

    // set event
    if (action == GLFW_PRESS) {
        btn->pushed = true;
        btn->ended_down = true;
    }
    else if (action == GLFW_RELEASE) {
        btn->released = true;
        btn->pushes++;
    }
}

void MouseScrollCallBack(GLFWwindow* window, double xoffset, double yoffset) {
    PlafGlfw *plaf = _GlfwWindowToUserPtr(window);

    plaf->scroll.yoffset_acc += yoffset;
    if (yoffset > 0) {
        plaf->scroll.steps_up++;
    }
    else if (yoffset < 0) {
        plaf->scroll.steps_down++;
    }
}

void CharCallBack(GLFWwindow* window, u32 codepoint) {
    PlafGlfw *plf = _GlfwWindowToUserPtr(window);

    if (codepoint >= 0 && codepoint < 128) {
        char c = (u8) codepoint;
        plf->keys.Put(c);
    }
}

void KeyCallBack(GLFWwindow* window,  int key, int scancode, int action, int mods) {
    PlafGlfw *plf = _GlfwWindowToUserPtr(window);

    if (key == GLFW_KEY_LEFT_CONTROL || key == GLFW_KEY_LEFT_CONTROL) {
        plf->akeys.mod_ctrl = (action == GLFW_PRESS);
    }
    if (key == GLFW_KEY_LEFT_ALT || key == GLFW_KEY_RIGHT_ALT) {
        plf->akeys.mod_alt = (action == GLFW_PRESS);
    }
    if (key == GLFW_KEY_LEFT_SHIFT || key == GLFW_KEY_RIGHT_SHIFT) {
        plf->akeys.mod_shift = (action == GLFW_PRESS);
    }

    if (action == GLFW_PRESS) {
        if (key == 256) {
            plf->akeys.esc = true;
        }
        else if (key == 257) {
            plf->akeys.enter = true;
        }
        else if (key == 259) {
            plf->akeys.backspace = true;
        }
        else if (key == 261) {
            plf->akeys.del = true;
        }
        else if (key == ' ') {
            plf->akeys.space = true;
        }
        else if (key >= 290 && key <= 301) {
            // 290-301: F1 through F12
            plf->akeys.fkey = key - 289;
        }

        else if (key == 'C' && mods == GLFW_MOD_CONTROL) {
            printf("ctr-C\n");
        }
        else if (key == 'X' && mods == GLFW_MOD_CONTROL) {
            printf("ctr-X\n");
        }
        else if (key == 'Z' && mods == GLFW_MOD_CONTROL) {
            printf("ctr-Z\n");
        }
    }
}

void WindowResizeCallBack(GLFWwindow* window, int width, int height) {
    PlafGlfw *plf = _GlfwWindowToUserPtr(window);

    plf->width = width;
    plf->height = height;
    plf->screen.SetSize(plf->image_buffer, width, height);
}


static u8 *g_image_buffer;
#define IMG_BUFF_CHANNELS 4
#define IMG_BUFF_MAX_WIDTH 3840
#define IMG_BUFF_MAX_HEIGHT 2160
u8* ImageBufferGet() {
    return g_image_buffer;
}
void ImageBufferInit(MArena *a_dest) {
    g_image_buffer = (u8*) ArenaAlloc(a_dest, IMG_BUFF_CHANNELS * IMG_BUFF_MAX_WIDTH * IMG_BUFF_MAX_HEIGHT);
}
void ImageBufferClear(u32 width, u32 height) {
    if (g_image_buffer) {
        memset(g_image_buffer, 255, IMG_BUFF_CHANNELS * width * height);
    }
}


static PlafGlfw g_plaf_glfw;
PlafGlfw* PlafGlfwInit(u32 window_width = 640, u32 window_height = 480) {
    g_plaf_glfw = {};
    PlafGlfw *plf = &g_plaf_glfw;
    plf->width = window_width;
    plf->height = window_height;

    glfwInit();

    // opengl window & context
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    plf->window = glfwCreateWindow(plf->width, plf->height, "glew_flfw_window_title", NULL, NULL);
    glfwMakeContextCurrent(plf->window);

    // glew
    glewExperimental = GL_TRUE;
    glewInit();

    // alpha blending
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);

    // input
    glfwSetCharCallback(plf->window, CharCallBack); // NOTE: potentially use glfwSetCharModsCallback to additionally get the mods
    glfwSetKeyCallback(plf->window, KeyCallBack);
    glfwSetMouseButtonCallback(plf->window, MouseButtonCallBack);
    glfwSetScrollCallback(plf->window, MouseScrollCallBack);
    glfwSetWindowUserPointer(plf->window, plf);

    // window resize
    glfwSetFramebufferSizeCallback(g_plaf_glfw.window, WindowResizeCallBack);

    // shader
    plf->image_buffer = ImageBufferGet();
    plf->screen = ScreenProgramInit(plf->image_buffer, plf->width, plf->height);

    // initialize mouse position values (dx and dy are initialized to zero)
    f64 mouse_x;
    f64 mouse_y;
    glfwGetCursorPos(plf->window, &mouse_x, &mouse_y);
    plf->cursorpos.x = (f32) mouse_x;
    plf->cursorpos.y = (f32) mouse_y;
    plf->cursorpos.x_frac = ((f32) mouse_x - (plf->width * 0.5f)) / plf->width;
    plf->cursorpos.y_frac = ((f32) mouse_y - (plf->height * 0.5f)) / plf->height;

    return plf;
}

void PlafGlfwTerminate(PlafGlfw* plf) {
    glfwDestroyWindow(plf->window);
    glfwTerminate();
}

void PlafGlfwUpdate(PlafGlfw* plf) {
    if (plf->akeys.fkey == 10) {
        // toggle fullscreen

        plf->fullscreen = !plf->fullscreen;
        if (plf->fullscreen) {
            assert(plf->width_cache == 0);
            assert(plf->height_cache == 0);

            plf->width_cache = plf->width;
            plf->height_cache = plf->height;
            glfwGetWindowPos(plf->window, &plf->window_xpos, &plf->window_ypos);

            GLFWmonitor *monitor = glfwGetWindowMonitor(plf->window);

            const GLFWvidmode *mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
            plf->width = mode->width;
            plf->height = mode->height;

            glfwSetWindowMonitor(plf->window, monitor, 0, 0, plf->width, plf->height, GLFW_DONT_CARE);
        }
        else {
            plf->width = plf->width_cache;
            plf->height = plf->height_cache;

            plf->width_cache = 0;
            plf->height_cache = 0;

            // doesn't get us back into windowed
            //glfwSetWindowMonitor(plf->window, NULL, 0, 0, 0, 0, GLFW_DONT_CARE);
            // TODO: try creating a "windowed full screen" mode switch

            // destroy and re-create everything (!?!)
            glfwDestroyWindow(plf->window);
            glfwTerminate();
            plf = PlafGlfwInit(plf->width, plf->height);
        }

        plf->screen.SetSize(plf->image_buffer, plf->width, plf->height);
    }

    plf->screen.Draw(plf->image_buffer, plf->width, plf->height);
    glfwSwapBuffers(plf->window);

    plf->left = {};
    plf->right = {};
    plf->scroll = {};
    plf->keys = {};
    plf->akeys.ResetButKeepMods();

    plf->left.ended_down = (glfwGetMouseButton(plf->window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS);
    plf->right.ended_down = (glfwGetMouseButton(plf->window, GLFW_MOUSE_BUTTON_2) == GLFW_PRESS);

    f64 mouse_x;
    f64 mouse_y;
    glfwGetCursorPos(plf->window, &mouse_x, &mouse_y);
    
    plf->cursorpos.dx = (f32) mouse_x - plf->cursorpos.x;
    plf->cursorpos.dy = (f32) mouse_y - plf->cursorpos.y;
    plf->cursorpos.x = (f32) mouse_x;
    plf->cursorpos.y = (f32) mouse_y;

    f32 x_frac = ((f32) mouse_x - plf->width * 0.5f) / plf->width;
    f32 y_frac = ((f32) mouse_y - plf->height * 0.5f) / plf->height;
    plf->cursorpos.dx_frac = plf->cursorpos.x_frac - x_frac;
    plf->cursorpos.dy_frac = plf->cursorpos.y_frac - y_frac;
    plf->cursorpos.x_frac = x_frac;
    plf->cursorpos.y_frac = y_frac;

    glfwPollEvents();
}


inline Button MouseLeft() { return g_plaf_glfw.left; }
inline Button MouseRight() { return g_plaf_glfw.right; }
inline Scroll MouseScroll() { return g_plaf_glfw.scroll; }
inline Vector2f MouseFrac() { return { g_plaf_glfw.cursorpos.x_frac, g_plaf_glfw.cursorpos.y_frac }; }
inline Vector2f MouseFracDelta() { return { (f32) g_plaf_glfw.cursorpos.dx / g_plaf_glfw.width, (f32) g_plaf_glfw.cursorpos.dy / g_plaf_glfw.height }; }
inline char GetChar() { return g_plaf_glfw.keys.Get(); }
inline bool GetEscape() { return g_plaf_glfw.akeys.esc; }
inline bool GetEnter() { return g_plaf_glfw.akeys.enter; }
inline bool GetSpace() { return g_plaf_glfw.akeys.space; }
inline bool GetBackspace() { return g_plaf_glfw.akeys.backspace; }
inline bool GetDelete() { return g_plaf_glfw.akeys.del; }

bool GetFKey(u32 *fval) {
    assert(fval != NULL);

    u8 fkey = g_plaf_glfw.akeys.fkey;
    if (fkey == 0) {
        return false;
    }
    else {
        *fval = fkey;
        return true;
    }
}

bool GetFKey(u32 fval) {
    if (g_plaf_glfw.akeys.fkey == fval) {
        return true;
    }
    else {
        return false;
    }
}

inline bool ModCtrl() { return g_plaf_glfw.akeys.mod_ctrl; }
inline bool ModShift() { return g_plaf_glfw.akeys.mod_shift; }
inline bool ModAlt() { return g_plaf_glfw.akeys.mod_alt; }

bool GetWindowShouldClose(PlafGlfw *plf) { return glfwWindowShouldClose(plf->window); }


#endif
