// std-lib
#include <iostream>
#include <cmath>
// gl-lib
#include "glad/glad.h"
#include "KHR/khrplatform.h"
#include "glfw3.h"
// 3rdparty header
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
// seld header
#include "glshape.h"
#include "glcamera.h"

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);
GLuint loadTexture(const char *file_path, GLuint internalFormat);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
void mouse_callback(GLFWwindow *window, double xposIn, double yposIn);
// settings
const unsigned int SRC_WIDTH = 800;
const unsigned int SRC_HEIGHT = 600;

bool firstMouse = true;

// timing
float deltaTime = 0.0f; // time between current frame and last frame
float lastFrame = 0.0f;
float lastX = (float)SRC_WIDTH / 2.0;
float lastY = (float)SRC_HEIGHT / 2.0;
// camera
GLBasicCamera camera(glm::vec3(0.0f, 0.0f, 3.0f));

int main()
{
    // 初始化
    glfwInit();
    // 设置主版本号 3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    // 设置副版本号 3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    // GLFW_OPENGL_CORE_PROFILE 对应核心模式
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow *window = glfwCreateWindow(SRC_WIDTH, SRC_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    // 设置当前上下文
    glfwMakeContextCurrent(window);
    // 设置回调函数
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // 初始化GLAD, GLAD回将所有设备的地址绑定到对应的指针上以供我们使用
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    // cube1
    GLLighterCube cube = GLLighterCube();
    GLLighterCube lighter = GLLighterCube();
    cube.setUniform("objectColor", glm::vec3(1.0f, 0.5f, 0.31f));
    cube.setUniform("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));
    // 光照位置
    glm::vec3 lightPos(1.2f, 1.0f, 2.0f);
    lighter.setUniform("objectColor", glm::vec3(1.0f, 1.0f, 1.0f)); // 光照体颜色为白色
    lighter.setUniform("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));  // 偷懒复用fs

    // Render Loop
    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        processInput(window);
        glClearColor(0.1f, 0.f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // view
        glm::mat4 view = camera.viewMatrix();
        glm::mat4 projection = camera.projectionMatrix(SRC_WIDTH, SRC_HEIGHT);
        // cube
        cube.setUniform("projection", projection);
        cube.setUniform("view", view);
        glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
        cube.setUniform("model", model);
        cube.draw();
        // lighter
        lighter.setUniform("projection", projection);
        lighter.setUniform("view", view);
        model = glm::translate(glm::mat4(1.0f), lightPos);
        model = glm::scale(model, glm::vec3(0.2f));
        lighter.setUniform("model", model);
        lighter.draw();
        // swap cache
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.processKeyboard(CameraMovement::FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.processKeyboard(CameraMovement::BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.processKeyboard(CameraMovement::LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.processKeyboard(CameraMovement::RIGHT, deltaTime);
}
void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}
// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow *window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_CONTROL))
    {
        if (firstMouse)
        {
            lastX = xpos;
            lastY = ypos;
            firstMouse = false;
        }
        float xoffset = xpos - lastX;
        float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
        camera.processMouseMovement(xoffset, yoffset);
    }
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
    camera.processMouseScroll(static_cast<float>(yoffset));
}
