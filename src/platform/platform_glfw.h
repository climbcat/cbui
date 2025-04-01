#ifndef __PLATFORM_GLFW_H__
#define __PLATFORM_GLFW_H__


#include <math.h>

#include <assert.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>


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
    GLFWwindow* window;

    Button left;
    Button right;
    Scroll scroll;
    AsciiKeys keys;
    ActionKeys akeys;

    f64 mouse_x_f64;
    f64 mouse_y_f64;
    s32 mouse_x;
    s32 mouse_y;
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


static PlafGlfw g_plaf_glfw;
PlafGlfw* PlafGlfwInit() {
    int width = 140;
    int height = 80;
    const char *title = "glew_flfw_window_title";

    glfwInit();

    // opengl window & context
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    g_plaf_glfw.window = glfwCreateWindow(width, height, title, NULL, NULL);
    glfwMakeContextCurrent(g_plaf_glfw.window);

    // glew
    glewExperimental = GL_TRUE;
    glewInit();

    // alpha blending
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);

    glfwSetCharCallback(g_plaf_glfw.window, CharCallBack); // NOTE: potentially use glfwSetCharModsCallback to additionally get the mods
    glfwSetKeyCallback(g_plaf_glfw.window, KeyCallBack);
    glfwSetMouseButtonCallback(g_plaf_glfw.window, MouseButtonCallBack);
    glfwSetScrollCallback(g_plaf_glfw.window, MouseScrollCallBack);
    glfwSetWindowUserPointer(g_plaf_glfw.window, &g_plaf_glfw);

    return &g_plaf_glfw;
}
void PlafGlfwTerminate(PlafGlfw* plf) {
    glfwDestroyWindow(plf->window);
    glfwTerminate();
}
void PlafGlfwUpdate(PlafGlfw* plf) {
    plf->left = {};
    plf->right = {};
    plf->scroll = {};
    plf->keys = {};
    plf->akeys = {};

    plf->left.ended_down = (glfwGetMouseButton(plf->window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS);
    plf->right.ended_down = (glfwGetMouseButton(plf->window, GLFW_MOUSE_BUTTON_2) == GLFW_PRESS);

    glfwGetCursorPos(plf->window, &plf->mouse_x_f64, &plf->mouse_y_f64);
    plf->mouse_x = (s32) floor(plf->mouse_x_f64);
    plf->mouse_y = (s32) floor(plf->mouse_y_f64);

    glfwPollEvents();
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
