#include "../baselayer/baselayer.h"

//#include "gtypes.h"
//#include "geometry.h"

#include "wireframe.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <assert.h>
#include "ui.h"


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
    char keys[32]; // max 32 keystrokes per frame ...
    u8 keys_cnt;
    u8 keys_idx;

    void Put(char c) {
        if (keys_cnt < 16) {
            keys[keys_cnt++] = c;
        }
    }
    char Get() {
        if (keys_cnt && (keys_idx < keys_cnt)) {
            return keys[keys_idx++];
        }
        else {
            return 0;
        }
    }
};

struct PlafGlfw {
    GLFWwindow* window;
    Button left;
    Button right;
    f64 mouse_x_f64;
    f64 mouse_y_f64;
    s32 mouse_x;
    s32 mouse_y;
    Scroll scroll;
    AsciiKeys keys;
};

inline PlafGlfw *_GlfwWindowToUserPtr(GLFWwindow* window) {
    PlafGlfw *plaf = (PlafGlfw*) glfwGetWindowUserPointer(window);
    return plaf;
}

void MouseButtonCallBack(GLFWwindow* window, int button, int action, int mods)
{
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
void MouseScrollCallBack(GLFWwindow* window, double xoffset, double yoffset)
{
    PlafGlfw *plaf = _GlfwWindowToUserPtr(window);
    plaf->scroll.yoffset_acc += yoffset;
    if (yoffset > 0) {
        plaf->scroll.steps_up++;
    }
    else if (yoffset < 0) {
        plaf->scroll.steps_down++;
    }
}
void KeyCallBack(GLFWwindow* window, int key, int scancode, int action, int mods) {
    PlafGlfw *plf = _GlfwWindowToUserPtr(window);
    bool db_print = false;

    // key is basically ascii for the common characters
    char c = 0;
    if (key >= 0 && key <= 255) {
        c = (char) key;
    }
    if (mods != GLFW_MOD_SHIFT) {
        if (db_print) printf("shift-");
        c += 32;
    }
    else if (mods == GLFW_MOD_CONTROL) {
        if (db_print) printf("control-");
    }

    if (action == GLFW_PRESS) {
        if (db_print) printf("press: ");
    }
    else if (action == GLFW_RELEASE) {
        if (db_print) printf("relase: ");
    }
    else if (action == GLFW_REPEAT) {
        if (db_print) printf("repeat: ");
    }
    if (db_print) printf("%c \n", c);

    plf->keys.Put(c);
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
Scroll MouseScrollConsume() {
    Scroll result = g_plaf_glfw.scroll;
    g_plaf_glfw.scroll = {};
    return result;
}
char GetChar() {
    char c = g_plaf_glfw.keys.Get();
    return c;
}


void RunWireframeProgram() {
    printf("RunWireframeProgram\n");

    MContext *ctx = InitBaselayer();
    PlafGlfw *plf = PlafGlfwInit();


    bool running = true;
    while (running) {


        if (MouseLeft().pushed) {
            printf("click\n");
        }
        if (MouseRight().pushed) {
            printf("right\n");
        }
        if (MouseScrollConsume().yoffset_acc != 0) {
            printf("scroll\n");
        }

        char c = 0;
        while(c = GetChar()) {
            printf("%c", c);
        }
        fflush(stdout);

        PlafGlfwUpdate(plf);
        XSleep(10);
    }


    PlafGlfwTerminate(plf);
}


int main (int argc, char **argv) {
    TimeProgram;

    if (CLAContainsArg("--help", argc, argv) || CLAContainsArg("-h", argc, argv)) {
        printf("--help:          display help (this text)\n");
    }

    RunWireframeProgram();

    return 0;
}
