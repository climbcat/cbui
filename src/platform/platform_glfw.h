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
    u8 fkey; 
};

struct PlafGlfw {
    u64 frame_no;
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
    u32 max_width;
    u32 max_height;
    u8 *image_buffer;
};


f32 PlafGlfwGetAspect(PlafGlfw *plf) {
    f32 aspect = ((f32) plf->width) / ((f32) plf->height);
    return aspect;
}


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
    }
    else if (action == GLFW_RELEASE) {
        btn->pushed = true;
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


static PlafGlfw g_plaf_glfw;
PlafGlfw* PlafGlfwInit(MArena *a_dest, u32 window_width = 640, u32 window_height = 480, bool start_in_fullscreen = false) {
    g_plaf_glfw = {};
    PlafGlfw *plf = &g_plaf_glfw;
    plf->width = window_width;
    plf->height = window_height;
    plf->max_width = 3840;
    plf->max_height = 2160;

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
    plf->image_buffer = (u8*) ArenaAlloc(a_dest, 4 * plf->max_width * plf->max_height);
    memset(plf->image_buffer, 255, 4 * plf->max_width * plf->max_height);
    plf->screen = ScreenProgramInit(plf->image_buffer, plf->width, plf->height);

    // initialize mouse position values (dx and dy are initialized to zero)
    f64 mouse_x;
    f64 mouse_y;
    glfwGetCursorPos(plf->window, &mouse_x, &mouse_y);
    plf->cursorpos.x = (f32) mouse_x;
    plf->cursorpos.y = (f32) mouse_y;

    return plf;
}

void PlafGlfwTerminate(PlafGlfw* plf) {
    glfwDestroyWindow(plf->window);
    glfwTerminate();
}

void PlafGlfwUpdate(PlafGlfw* plf) {
    if (plf->akeys.fkey == 10) {
        const GLFWvidmode *mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

        printf("F10 pressed\n");

        plf->fullscreen = !plf->fullscreen;

        GLFWmonitor *monitor = NULL;
        if (plf->fullscreen) {
            monitor = glfwGetWindowMonitor(plf->window);

            plf->width = mode->width;
            plf->height = mode->height;
        }

        glfwSetWindowMonitor(plf->window, monitor, 0, 0, plf->width, plf->height, GLFW_DONT_CARE);
        plf->screen.SetSize(plf->image_buffer, plf->width, plf->height);
    }

    plf->screen.Draw(plf->image_buffer, plf->width, plf->height);
    glfwSwapBuffers(plf->window);

    plf->left = {};
    plf->right = {};
    plf->scroll = {};
    plf->keys = {};
    plf->akeys = {};

    plf->left.ended_down = (glfwGetMouseButton(plf->window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS);
    plf->right.ended_down = (glfwGetMouseButton(plf->window, GLFW_MOUSE_BUTTON_2) == GLFW_PRESS);

    f64 mouse_x;
    f64 mouse_y;
    glfwGetCursorPos(plf->window, &mouse_x, &mouse_y);
    
    plf->cursorpos.dx = (f32) mouse_x - plf->cursorpos.x;
    plf->cursorpos.dy = (f32) mouse_y - plf->cursorpos.y;
    plf->cursorpos.x = (f32) mouse_x;
    plf->cursorpos.y = (f32) mouse_y;

    glfwPollEvents();

    plf->frame_no++;
}


Button MouseLeft() {
    Button left = g_plaf_glfw.left;
    return left;
}

Button MouseRight() {
    Button right = g_plaf_glfw.right;
    return right;
}

Scroll MouseScroll() {
    Scroll result = g_plaf_glfw.scroll;
    return result;
}

char GetChar() {
    char c = g_plaf_glfw.keys.Get();
    return c;
}

bool GetEscape() {
    bool was = g_plaf_glfw.akeys.esc;
    return was;
}

bool GetEnter() {
    bool was = g_plaf_glfw.akeys.enter;
    return was;
}

bool GetBackspace() {
    bool was = g_plaf_glfw.akeys.backspace;
    return was;
}

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


#endif
