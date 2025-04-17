#ifndef __SHADERS_H__
#define __SHADERS_H__


#include <GL/glew.h>
#include "../../lib/jg_baselayer.h"


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

    void SetSize(u8* imgbuffer, u32 width, u32 height) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, imgbuffer);
        glViewport(0, 0, width, height);
    }

    void Draw(u8* imgbuffer, u32 width, u32 height) {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f );
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(program);
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);

        glBindTexture(GL_TEXTURE_2D, texture_id);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, imgbuffer);

        u32 nverts = 4;
        glDrawArrays(GL_TRIANGLE_STRIP, 0, nverts);
        glBindVertexArray(0);
    }
};


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
    prog.SetSize(imgbuffer, width, height);

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


#endif
