#ifndef HSB_GL_UTIL_H
#define HSB_GL_UTIL_H
// std-lib
#include <iostream>
#include <cmath>
// opengl
#include "glad/glad.h"
#include "KHR/khrplatform.h"
#include "glfw3.h"
// 3rdparty header
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

int glfwgladInitialization(GLFWwindow **window, int SRC_WIDTH, int SRC_HEIGHT, const char *title = "LearningOpenGL");
int glfwgladInitialization(GLFWwindow **window, int SRC_WIDTH, int SRC_HEIGHT, const char *title, int sampleN);
void processInput(GLFWwindow *window);
void framebufferSizeCallback(GLFWwindow *window, int width, int height);
GLuint loadTexture(const char *file_path, GLuint internalFormat);
GLuint autoLoadTexture(const char *file_path, GLint mode = GL_REPEAT, bool gamma = false);
GLuint loadCubemap(const std::string file_path, const std::string default_fix_string = ".jpg");
#endif
