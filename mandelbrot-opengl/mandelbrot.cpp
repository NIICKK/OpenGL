#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>

#include <GL/glew.h>
#include <sys/stat.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


#include <GLFW/glfw3.h>
static void error_callback(int error, const char *description) {
    std::cerr << "GLFW Error: " << description << std::endl;
}

double cx = 0.0, cy = 0.0, zoom = 0.5;
double expo = 2;

int itr = 100;
int fps = 0;

GLFWwindow *window = nullptr;

int w = 640;
int h = 480;

bool keys[1024] = { 0 };

static void cursor_callback(GLFWwindow* window, double xpos, double ypos)
{
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);

    double xr = 2.0 * (xpos / (double)w - 0.5);
    double yr = 2.0 * (ypos / (double)h - 0.5);

    if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        cx += (xr - cx) / zoom / 2.0;
        cy -= (yr - cy) / zoom / 2.0;
    }
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    zoom += yoffset * 0.1 * zoom;
    if(zoom < 0.1) {
        zoom = 0.1;
    }
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    const double d = 0.1 / zoom;

    if(action == GLFW_PRESS) {
        keys[key] = true;
    } else if(action == GLFW_RELEASE) {
        keys[key] = false;
    }

    if(keys[GLFW_KEY_ESCAPE]) {
        glfwSetWindowShouldClose(window, 1);
    } else if(keys[GLFW_KEY_A]) {
        cx -= d;
    } else if(keys[GLFW_KEY_D]) {
        cx += d;
    } else if(keys[GLFW_KEY_W]) {
        cy += d;
    } else if(keys[GLFW_KEY_S]) {
        cy -= d;
    } else if(keys[GLFW_KEY_MINUS]) {
        itr += 10;
    } else if(keys[GLFW_KEY_EQUAL]) {
        itr -= 10;
        if(itr <= 0) {
            itr = 0;
        }
    } else if(keys[GLFW_KEY_2 ]){
        expo = 2.0;
    } else if(keys[GLFW_KEY_3 ]){
        expo = 3.0;
    } else if(keys[GLFW_KEY_4 ]){
        expo = 4.0;
    } else if(keys[GLFW_KEY_5 ]){
        expo = 5.0;
    } else if(keys[GLFW_KEY_6 ]){
        expo = 6.0;
    } else if(keys[GLFW_KEY_7 ]){
        expo = 7.0;
    } else if(keys[GLFW_KEY_8 ]){
        expo = 8.0;
    }

}

const char* vertex_shader =
"#version 410\n"
"in vec3 vp;"
"void main () {"
 "  gl_Position = vec4 (vp, 1.0);"
 "}";

static void compile_shader(GLuint &prog)
{
    GLuint vs = glCreateShader (GL_VERTEX_SHADER);
    glShaderSource (vs, 1, &vertex_shader, NULL);
    glCompileShader (vs);

    std::ifstream t("shader.glsl");
    if(!t.is_open()) {
        std::cerr << "Cannot open shader.glsl!" << std::endl;
        return;
    }
    std::string str((std::istreambuf_iterator<char>(t)),
                    std::istreambuf_iterator<char>());
    const char *src  = str.c_str();

    GLuint fs = glCreateShader (GL_FRAGMENT_SHADER);
    glShaderSource (fs, 1, &src, NULL);
    glCompileShader (fs);

    int success;
    glGetShaderiv(fs, GL_COMPILE_STATUS, &success);
    if(!success) {
        int s;
        glGetShaderiv(fs, GL_INFO_LOG_LENGTH, &s);

        char *buf = new char[s];
        glGetShaderInfoLog(fs, s, &s, buf);

        std::cerr << buf << std::endl;
        delete [] buf;
        return;
    }

    prog = glCreateProgram ();
    glAttachShader (prog, fs);
    glAttachShader (prog, vs);
    glLinkProgram (prog);

    glGetProgramiv(prog, GL_LINK_STATUS, &success);
    if(!success) {
        int s;
        glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &s);

        char *buf = new char[s];
        glGetProgramInfoLog(prog, s, &s, buf);

        std::cerr << buf << std::endl;
        delete [] buf;
        return;
    }

    glDeleteShader(vs);
    glDeleteShader(fs);
}


int main(int argc, char *argv[])
{
    if(!glfwInit()) {
        std::cerr << "Failed to init GLFW" << std::endl;
        return 1;
    }

    atexit(glfwTerminate);

    glfwSetErrorCallback(error_callback);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(w, h, "Hello World", NULL, NULL);
    if(!window) {
        std::cerr << "Failed to create window" << std::endl;
        return 1;
    }

    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, cursor_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetInputMode(window, GLFW_CURSOR_NORMAL, GLFW_STICKY_KEYS);

    glfwMakeContextCurrent(window);

    glewExperimental = GL_TRUE;
    glewInit();

    std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;

    GLuint prog;
    compile_shader(prog);

    float points[] = {
            -1.0f,  1.0f,  0.0f,
            -1.0f,  -1.0f,  0.0f,
            1.0f,  -1.0f,  0.0f,

            -1.0f,  1.0f,  0.0f,
            1.0f,  -1.0f,  0.0f,
            1.0f,  1.0f,  0.0f,
    };

    // Load textures
    GLuint textures;
    glGenTextures(1, &textures);

    int width, height, nrChannels;
    unsigned char* data;

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textures);
    data = stbi_load("resources/container.jpg", &width, &height, &nrChannels, 0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    stbi_image_free(data);
    glUniform1i(glGetUniformLocation(prog, "tex"), 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    /*--------------------------------------------------------------------------------*/
    GLuint vbo = 0;
    glGenBuffers (1, &vbo);
    glBindBuffer (GL_ARRAY_BUFFER, vbo);
    glBufferData (GL_ARRAY_BUFFER, 2 * 9 * sizeof (float), points, GL_STATIC_DRAW);

    GLuint vao = 0;
    glGenVertexArrays (1, &vao);
    glBindVertexArray (vao);
    glEnableVertexAttribArray (0);
    glBindBuffer (GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

    glUseProgram (prog);
    glBindVertexArray (vao);

    while(!glfwWindowShouldClose(window)) {

        glfwGetWindowSize(window, &w, &h);
        glUniform2d(glGetUniformLocation(prog, "screen_size"), (double)w, (double)h);
        glUniform1d(glGetUniformLocation(prog, "screen_ratio"), (double)w / (double)h);
        glUniform2d(glGetUniformLocation(prog, "center"), cx, cy);
        glUniform1d(glGetUniformLocation(prog, "exponent"), expo);
        glUniform1d(glGetUniformLocation(prog, "zoom"), zoom);
        glUniform1i(glGetUniformLocation(prog, "itr"), itr);

        glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glDrawArrays (GL_TRIANGLES, 0, 6);

        glfwSwapBuffers(window);
        glfwPollEvents();

    }

    glfwDestroyWindow(window);
}