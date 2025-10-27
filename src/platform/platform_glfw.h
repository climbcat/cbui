#ifndef __PLATFORM_GLFW_H__
#define __PLATFORM_GLFW_H__


#include <GL/glew.h>
#include <GLFW/glfw3.h>


//
//  OpenGL


void CheckShaderCompilationErrors(GLuint shader, const char *header_info) {
    int success;
    char info_log[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, NULL, info_log);
        printf("%s:%s\n", header_info, info_log);
    }
}

void CheckShaderLinkErrors(GLuint program, const char *header_info) {
    int success;
    char info_log[512];
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program, 512, NULL, info_log);
        printf("%s:%s\n", header_info, info_log);
    }
}

void ShaderProgramLink(GLuint *program, const GLchar *vsh_src, const GLchar *fsh_src, const GLchar *frag_data_loc = "o_color") {
    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vsh_src, NULL);
    glCompileShader(vs);
    CheckShaderCompilationErrors(vs, "vertex shader compilation error");

    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fsh_src, NULL);
    glCompileShader(fs);
    CheckShaderCompilationErrors(fs, "fragment shader compilation error");

    *program = glCreateProgram();
    glAttachShader(*program, vs);
    glAttachShader(*program, fs);
    glBindFragDataLocation(*program, 0, frag_data_loc);
    glLinkProgram(*program);
    CheckShaderLinkErrors(*program, "shader program link error");

    glDeleteShader(vs);
    glDeleteShader(fs);
    glUseProgram(*program);
}

struct ScreenProgram {
    // draws a texture to the screen
    GLuint program;
    GLuint vao;
    GLuint vbo;
    GLuint texture_id;

    const GLchar* vert_src = R"glsl(
        #version 330 core

        in vec2 position;
        in vec2 tex_coord;
        out vec2 coord;

        void main()
        {
            gl_Position = vec4(position, 0.0, 1.0);
            coord = tex_coord;
        }
    )glsl";
    const GLchar* frag_src = R"glsl(
        #version 330 core

        in vec2 coord;
        out vec4 o_color;
        uniform sampler2D sampler;

        void main()
        {
            o_color = texture(sampler, coord);
        }
    )glsl";

};

void ScreeProgramDraw(ScreenProgram *p, u8* imgbuffer, u32 width, u32 height) {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f );
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(p->program);
    glBindVertexArray(p->vao);
    glBindBuffer(GL_ARRAY_BUFFER, p->vbo);

    glBindTexture(GL_TEXTURE_2D, p->texture_id);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, imgbuffer);

    u32 nverts = 4;
    glDrawArrays(GL_TRIANGLE_STRIP, 0, nverts);
    glBindVertexArray(0);
}

void ScreenProgramSetSize(u8* imgbuffer, u32 width, u32 height) {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, imgbuffer);
    glViewport(0, 0, width, height);
}

ScreenProgram ScreenProgramInit(u8* imgbuffer, u32 width, u32 height) {
    ScreenProgram prog = {};

    ShaderProgramLink(&prog.program, prog.vert_src, prog.frag_src);
    glGenVertexArrays(1, &prog.vao);
    glBindVertexArray(prog.vao);

    // texture
    glGenTextures(1, &prog.texture_id);
    glBindTexture(GL_TEXTURE_2D, prog.texture_id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glUseProgram(prog.program);
    glBindVertexArray(prog.vao);
    glBindBuffer(GL_ARRAY_BUFFER, prog.vbo);
    ScreenProgramSetSize(imgbuffer, width, height);

    // quad
    float sqreen_quad_verts[] = {
        1.0f,  1.0f, 1.0f, 0.0f,
        -1.0f,  1.0f, 0.0f, 0.0f,
        1.0f, -1.0f, 1.0f, 1.0f,
        -1.0f, -1.0f, 0.0f, 1.0f
    };
    u32 stride = 4;
    u32 nverts = 4;
    glGenBuffers(1, &prog.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, prog.vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * stride * nverts, &sqreen_quad_verts, GL_STATIC_DRAW);

    GLint pos_attr = glGetAttribLocation(prog.program, "position");
    glVertexAttribPointer(pos_attr, 2, GL_FLOAT, GL_FALSE, stride * sizeof(float), 0);
    glEnableVertexAttribArray(pos_attr);
    GLint tex_attr = glGetAttribLocation(prog.program, "tex_coord");
    glVertexAttribPointer(tex_attr, 2, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*) (2 * sizeof(float)));
    glEnableVertexAttribArray(tex_attr);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    return prog;
}


//
//  glfw


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
    u64 t_pushed; // used for calculating "click" timeout @ release
    u64 t_pushed_prev; // used for calculating "double-click" timeout @ release
    u32 pushes;
    bool dblclicked;
    bool clicked;
    bool pushed;
    bool released;
    bool ended_down;

    void FrameReset() {
        u64 t_pushed_cpy = t_pushed;
        u64 t_pushed_prev_cpy = t_pushed_prev;
        *this = {};
        t_pushed = t_pushed_cpy;
        t_pushed_prev = t_pushed_prev_cpy;
    }
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
    bool left;
    bool right;
    bool up;
    bool down;
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
    char *title;

    MousePosition cursorpos;
    Button left;
    Button right;
    Scroll scroll;

    AsciiKeys keys;
    ActionKeys akeys;

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


#define T_CLICK_TIMEOUT_MYS 500000 // 500 ms
#define T_DBLCLICK_TIMEOUT_MYS 500000 // 500 ms
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

        // double-click @ mouse-down
        btn->t_pushed_prev = btn->t_pushed;
        btn->t_pushed = ReadSystemTimerMySec();
        u64 t_since_last_mdown = btn->t_pushed - btn->t_pushed_prev;
        assert(btn->t_pushed_prev == 0 || (btn->t_pushed_prev < btn->t_pushed));

        if (btn->t_pushed_prev && (t_since_last_mdown < T_DBLCLICK_TIMEOUT_MYS)) {
            btn->dblclicked = true;
            btn->t_pushed = 0;
            btn->t_pushed_prev = 0;
        }
    }
    else if (action == GLFW_RELEASE) {
        btn->released = true;
        btn->pushes++;

        // click @ mouse-up
        u64 t_released = ReadSystemTimerMySec();
        u64 t_since_last_mdown = t_released - btn->t_pushed;
        assert(btn->t_pushed_prev == 0 || (btn->t_pushed_prev < btn->t_pushed));

        if (btn->t_pushed && (t_since_last_mdown < T_CLICK_TIMEOUT_MYS)) {
            btn->clicked = true;
        }
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
        else if (key == GLFW_KEY_LEFT) {
            plf->akeys.left = true;
        }
        else if (key == GLFW_KEY_RIGHT) {
            plf->akeys.right = true;
        }
        else if (key == GLFW_KEY_UP) {
            plf->akeys.up = true;
        }
        else if (key == GLFW_KEY_DOWN) {
            plf->akeys.down = true;
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
    ScreenProgramSetSize(plf->image_buffer, width, height);
}


static PlafGlfw *g_plaf_glfw;
void PlafGlfwInit(PlafGlfw *plf, const char *title, u32 window_width, u32 window_height, u8* image_buffer) {
    *plf = {};
    plf->width = window_width;
    plf->height = window_height;
    plf->title = (char*) title;

    glfwInit();

    // opengl window & context
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    plf->window = glfwCreateWindow(plf->width, plf->height, title, NULL, NULL);
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
    glfwSetFramebufferSizeCallback(plf->window, WindowResizeCallBack);

    // shader
    plf->image_buffer = image_buffer;
    plf->screen = ScreenProgramInit(plf->image_buffer, plf->width, plf->height);

    // initialize mouse position values (dx and dy are initialized to zero)
    f64 mouse_x;
    f64 mouse_y;
    glfwGetCursorPos(plf->window, &mouse_x, &mouse_y);
    plf->cursorpos.x = (f32) mouse_x;
    plf->cursorpos.y = (f32) mouse_y;
    plf->cursorpos.x_frac = ((f32) mouse_x - (plf->width * 0.5f)) / plf->width;
    plf->cursorpos.y_frac = ((f32) mouse_y - (plf->height * 0.5f)) / plf->height;

    g_plaf_glfw = plf;
}

void PlafGlfwTerminate(PlafGlfw* plf) {
    glfwDestroyWindow(plf->window);
    glfwTerminate();
}

void PlafGlfwToggleFullscreen(PlafGlfw* plf) {
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
        PlafGlfwInit(plf, plf->title, plf->width, plf->height, plf->image_buffer);
    }

    ScreenProgramSetSize(plf->image_buffer, plf->width, plf->height);
}

void PlafGlfwPushBuffer(PlafGlfw* plf) {
    ScreeProgramDraw(&plf->screen, plf->image_buffer, plf->width, plf->height);
    glfwSwapBuffers(plf->window);
}

void PlafGlfwUpdate(PlafGlfw* plf) {
    if (plf->akeys.fkey == 10) {
        // toggle fullscreen

        PlafGlfwToggleFullscreen(plf);
    }

    plf->left.FrameReset();
    plf->right.FrameReset();
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


inline Button MouseLeft() { return g_plaf_glfw->left; }
inline Button MouseRight() { return g_plaf_glfw->right; }
inline Scroll MouseScroll() { return g_plaf_glfw->scroll; }
inline Vector2f MouseFrac() { return { g_plaf_glfw->cursorpos.x_frac, g_plaf_glfw->cursorpos.y_frac }; }
inline Vector2f CurserPos() { return { g_plaf_glfw->cursorpos.x, g_plaf_glfw->cursorpos.y }; }
inline Vector2f MouseFracDelta() { return { (f32) g_plaf_glfw->cursorpos.dx / g_plaf_glfw->width, (f32) g_plaf_glfw->cursorpos.dy / g_plaf_glfw->height }; }
inline char GetChar() { return g_plaf_glfw->keys.Get(); }
inline bool GetEscape() { return g_plaf_glfw->akeys.esc; }
inline bool GetEnter() { return g_plaf_glfw->akeys.enter; }
inline bool GetSpace() { return g_plaf_glfw->akeys.space; }
inline bool GetBackspace() { return g_plaf_glfw->akeys.backspace; }
inline bool GetDelete() { return g_plaf_glfw->akeys.del; }
inline bool GetLeft() { return g_plaf_glfw->akeys.left; }
inline bool GetRight() { return g_plaf_glfw->akeys.right; }
inline bool GetUp() { return g_plaf_glfw->akeys.up; }
inline bool GetDown() { return g_plaf_glfw->akeys.down; }

inline bool GetFKey(u32 *fval) {
    assert(fval != NULL);

    u8 fkey = g_plaf_glfw->akeys.fkey;
    if (fkey == 0) {
        return false;
    }
    else {
        *fval = fkey;
        return true;
    }
}

inline bool GetFKey(u32 fval) {
    if (g_plaf_glfw->akeys.fkey == fval) {
        return true;
    }
    else {
        return false;
    }
}

inline bool GetChar(char c) {
    for (s32 i = 0; i < g_plaf_glfw->keys.keys_cnt; ++i) {
        char key = g_plaf_glfw->keys.keys[i];
        if (key == c) {
            return true;
        }
    }
    return false;
}

inline bool ModCtrl() { return g_plaf_glfw->akeys.mod_ctrl; }
inline bool ModShift() { return g_plaf_glfw->akeys.mod_shift; }
inline bool ModAlt() { return g_plaf_glfw->akeys.mod_alt; }

bool GetWindowShouldClose(PlafGlfw *plf) { return glfwWindowShouldClose(plf->window); }


#endif
