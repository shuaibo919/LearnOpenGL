#ifndef HSB_GL_UTIL_H
#define HSB_GL_UTIL_H
#include "glad/glad.h"
#include "KHR/khrplatform.h"
#include "glfw3.h"
// 3rdparty header
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

int glfwgladInitialization(GLFWwindow **window, const char *title = "LearningOpenGL");
void processInput(GLFWwindow *window);
void framebufferSizeCallback(GLFWwindow *window, int width, int height);
GLuint loadTexture(const char *file_path, GLuint internalFormat);

#endif
