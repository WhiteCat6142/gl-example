//Copyright (c) 2023 Whitecat6142
//BSD 3-clause license

//Original
//Copyright (c) Camilla Löwy <elmindreda@glfw.org>
//BSD 3-clause license

#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>
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

static const char* vertex_shader_text =
"#version 330\n"
"uniform mat4 MVP;\n"
"in vec2 vPos;\n"
"void main(){\n"
"    gl_Position = MVP * vec4(vPos, 0.0, 1.0);\n"
"};

//based on
//https://glslsandbox.com/e#99732.2
//imagine graph min{1/|x-r|,1}
static const char* fragment_shader_text =
"#version 330\n"
"uniform float time;\n"
"uniform vec2 resolution;\n"
"in vec4 gl_FragCoord;\n"
"const float PI = 3.14;\n"
"const float P0= (PI * 2.) / 5.;\n"
"\n"
"vec2 p = vec2(1.);\n"
"void drawCircle(vec2 position, float radius, inout float t){\n"
"	t += .0078125 / (abs(length(p + position) - radius));\n"
"}\n"
"void drawFlash(vec2 position, inout float t){\n"
"	t += .0001220703125 / (abs(p.x + position.x) * abs(p.y + position.y)) * (1.125 - sin(time*.125));\n"
"}\n"
"void main(){\n"
"	p = (gl_FragCoord.xy * 2. - resolution) / min(resolution.x, resolution.y);\n"
"	vec3 destColor = vec3(p, .125);\n"
"	float t = 0.;\n"
"	\n"
"	for(int j=1; j<3; j++){\n"
"		for(int i=0; i<5; i++){\n"
"			float phi = time + (float(i) + float(j) * sin(time)) * P0;\n"
"			float s = sin(phi);\n"
"			float c = cos(phi);\n"
"			\n"
"			drawCircle(vec2(c, s)*.15625, .15625, t);\n"
"			drawFlash(vec2(c, s)*.78125, t);\n"
"		}\n"
"	}\n"
"	gl_FragColor = vec4(destColor * t, 1.);\n"
"}";

static void error_callback(int error, const char* description){
    fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods){
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

int main(void){
    glfwSetErrorCallback(error_callback);

    if (!glfwInit())
        exit(EXIT_FAILURE);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_CONTEXT_CREATION_API, GLFW_EGL_CONTEXT_API);

    GLFWwindow* window = glfwCreateWindow(640, 640, "Triangle++", NULL, NULL);
    if (!window){
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwSetKeyCallback(window, key_callback);

    glfwMakeContextCurrent(window);
    gladLoadGL(glfwGetProcAddress);
    glfwSwapInterval(1);
    
    const GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_text, NULL);
    glCompileShader(vertex_shader);

    const GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_shader_text, NULL);
    glCompileShader(fragment_shader);

    const GLuint program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);

    const GLint mvp_location = glGetUniformLocation(program, "MVP");
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
        const float ratio = width / (float) height;

        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT);

        mat4x4 m, p, mvp;
        mat4x4_identity(m);
        mat4x4_rotate_Z(m, m, (float) glfwGetTime());
        mat4x4_ortho(p, -ratio, ratio, -1.f, 1.f, 1.f, -1.f);
        mat4x4_mul(mvp, p, m);

        glUseProgram(program);
        glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat*) &mvp);
        glUniform1f(time_location, (float) glfwGetTime());
        glUniform2f(resolution_location, (float) width, (float) height);
        glBindVertexArray(vertex_array);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);

    glfwTerminate();
    exit(EXIT_SUCCESS);
}
