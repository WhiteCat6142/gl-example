//Copyright (c) 2023 Whitecat6142
//BSD 3-clause license

//Original zlib/libpng license
//========================================================================
// OpenGL triangle example
// Copyright (c) Camilla Löwy <elmindreda@glfw.org>
//
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would
//    be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such, and must not
//    be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source
//    distribution.
//
//========================================================================

//for linux complie glfw with make install_glfw

//M1 Mac
//download arm-64 binary
//https://github.com/microsoft/vcpkg/issues/9822
//https://www.glfw.org/download

//or homebrew install glfw

//add opengl & /opt/homebrew/Cellar/glfw libglfw3.dylib to "links with" in buildphase
//add  /opt include to "Header search path" in buildsetting
//https://rikei-tawamure.com/entry/2022/07/09/195840

#ifdef __APPLE__

#define GL_SILENCE_DEPRECATION
//https://twitter.com/tokoik/status/1328842874696318978

#define GLFW_INCLUDE_GLCOREARB
//glfw load default OpenGL headers
//https://www.glfw.org/docs/3.3/build_guide.html#build_include
//#include <OpenGL/gl3.h>
//https://stackoverflow.com/questions/37736829/glgenvertexarrays-not-available-in-c
//https://applech2.com/archives/20200624-opengl-is-deprecated-but-available-on-apple-silicon.html
//https://github.com/MauriceGit/Simple_GLSL_Shader_Example/issues/1
//#include <GL/glew.h>
#else

#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>
//for MacOS download following
//https://dev.to/giovannicodes/opengl-setup-in-macos-48cl
//https://gen.glad.sh/
#endif
#include <GLFW/glfw3.h>

#include "linmath.h"

#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>

typedef struct Vertex
{
    vec2 pos;
} Vertex;

static const Vertex vertices[3] =
{
    { { -0.9f, -0.52f } },
    { {  0.9f, -0.52f } },
    { {   0.f,  1.04f } }
};

//https://shizenkarasuzon.hatenablog.com/entry/2020/05/24/183340
//https://wlog.flatlib.jp/2013/08/13/n1633/
//https://stackoverflow.com/questions/51459596/using-gl-fragcolor-vs-out-vec4-color
//https://www.khronos.org/opengl/wiki/Fragment_Shader#Outputs
static const char* vertex_shader_text =
"#version 410 core\n"
"uniform mat4 mvp;\n"
"in vec2 vPos;\n"
"void main(){\n"
"    gl_Position = mvp * vec4(vPos, 0.0, 1.0);\n"
"}";

//based on
//https://glslsandbox.com/e#99732.2
//imagine graph min{1/|x-r|,1}
static const char* fragment_shader_text =
"#version 410 core\n"
"uniform float time;\n"
"uniform vec2 resolution;\n"
"in vec4 gl_FragCoord;\n"
"layout (location = 0) out vec4 fragcolor;\n"
"const float PI = 3.14;\n"
"const float P0= (PI * 2.) / 5.;\n"
"\n"
"void drawCircle(vec2 position, float radius, inout float t, vec2 p){\n"
"    t += .0078125 / (abs(length(p + position) - radius));\n"
"}\n"
"void drawFlash(vec2 position, inout float t, vec2 p){\n"
"    t += .0001220703125 / (abs(p.x + position.x) * abs(p.y + position.y)) * (1.25- sin(time*.125));\n"
"}\n"
"void main(){\n"
"    vec2 p = (gl_FragCoord.xy * 2. - resolution) / min(resolution.x, resolution.y);\n"
"    vec3 destColor = vec3(p, .125);\n"
"    float t = 0.;\n"
"    \n"
"    for(int j=1; j<3; j++){\n"
"        for(int i=0; i<5; i++){\n"
"            float phi = time + (float(i) + float(j) * sin(time*.90625)) * P0;\n"
"            float s = sin(phi);\n"
"            float c = cos(phi);\n"
"            \n"
"            drawCircle(vec2(c, s)*.15625, .15625, t, p);\n"
"            drawFlash(vec2(c, s)*.78125, t, p);\n"
"        }\n"
"    }\n"
"    fragcolor = vec4(destColor * t,  1.);\n"
"}";

static void error_callback(int error, const char* description){
    fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods){
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

//https://qiita.com/jasmingirl@github/items/135a58ff27e3c934d15c
//http://www.opengl-tutorial.org/jp/beginners-tutorials/tutorial-2-the-first-triangle/
void debug_shader(GLuint vertex_shader){
    GLint success = 0;
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
    if(success == GL_FALSE){
        printf("Failed to initialize OpenGL shader\n");
        GLsizei bufSize;
        GLsizei length;
        glGetShaderiv(vertex_shader, GL_INFO_LOG_LENGTH , &bufSize);
        GLchar *infoLog = (GLchar *)malloc(bufSize);
        glGetShaderInfoLog(vertex_shader, bufSize, &length, infoLog);
        //glGetProgramInfoLog(vertex_shader);
        printf("%s", infoLog);
    }
}

int main(void){
    glfwSetErrorCallback(error_callback);

    if (!glfwInit())
        exit(EXIT_FAILURE);
    //https://qiita.com/jamaican/items/6dddb3d963bef179b24b

    //https://www.glfw.org/docs/latest/window_guide.html

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //https://marina.sys.wakayama-u.ac.jp/~tokoi/?date=20120908

    GLFWwindow* window = glfwCreateWindow(640, 640, "Triangle++", NULL, NULL);
    if (!window){
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    //https://www.glfw.org/docs/latest/input_guide.html
    glfwSetKeyCallback(window, key_callback);

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

#ifndef __APPLE__
    const int version = gladLoadGL(glfwGetProcAddress);
    if (version == 0) {
        printf("Failed to initialize OpenGL context\n");
        return -1;
    }
    int major = GLAD_VERSION_MAJOR(version);
    int minor = GLAD_VERSION_MINOR(version);
    printf("Loaded OpenGL version %d.%d\n", major, minor);
    //https://www.reddit.com/r/GraphicsProgramming/comments/rkz0mg/c_glad_glfw_question_about_gladloadgl_vs/
    //https://github.com/kcat/d3dgl/blob/master/glew.c
    //https://github.com/Dav1dde/glad
    //https://stackoverflow.com/questions/70550520/xcode-thread-1-exc-bad-access-code-1-address-0x0-in-glfw-glad-default-c
    
    /*glewExperimental = true;
    //glewInit();
    if (glewInit() != GLEW_OK) {
        return -1;
    }*/
#endif
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    //glEnable(GL_LIGHTING);
    //glEnable(GL_LIGHT0);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    //glEnable(GL_COLOR_MATERIAL);

    const GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_text, NULL);
    glCompileShader(vertex_shader);
    debug_shader(vertex_shader);


    const GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_shader_text, NULL);
    glCompileShader(fragment_shader);
    debug_shader(fragment_shader);

    const GLuint program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);

    const GLint mvp_location = glGetUniformLocation(program, "mvp");
    const GLint time_location = glGetUniformLocation(program, "time");
    const GLint resolution_location = glGetUniformLocation(program, "resolution");
    const GLint vpos_location = glGetAttribLocation(program, "vPos");

    GLuint vertex_buffer;
    glGenBuffers(1, &vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    GLuint vertex_array;
    glGenVertexArrays(1, &vertex_array);
    glBindVertexArray(vertex_array);
    glEnableVertexAttribArray(vpos_location);
    glVertexAttribPointer(vpos_location, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, pos));

    while (!glfwWindowShouldClose(window)){
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        //fprintf(stderr, "%d\n", height);
        // 1280 in Retina
        //https://stackoverflow.com/questions/44719635/what-is-the-difference-between-glfwgetwindowsize-and-glfwgetframebuffersize
        const float ratio = width / (float) height;

        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        mat4x4 m, p, mvp;
        mat4x4_identity(m);
        mat4x4_rotate_Z(m, m, (float) glfwGetTime()*0.90625);
        mat4x4_ortho(p, -ratio, ratio, -1.f, 1.f, 1.f, -1.f);
        mat4x4_mul(mvp, p, m);

        glUseProgram(program);
        glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat*) &mvp);
        glUniform1f(time_location, (float) glfwGetTime());
        glUniform2f(resolution_location, (float) width, (float) height);
        glBindVertexArray(vertex_array);
        //https://www.khronos.org/opengl/wiki/Geometry_Shader
        //https://www.khronos.org/opengl/wiki/Primitive
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);

    glfwTerminate();
    exit(EXIT_SUCCESS);
}


