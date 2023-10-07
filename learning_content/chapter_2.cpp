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

int glfwgladInitialization(GLFWwindow **window, const char *title = "LearningOpenGL");
void processInput(GLFWwindow *window);
void framebufferSizeCallback(GLFWwindow *window, int width, int height);
void mouseCallback(GLFWwindow *window, double xposIn, double yposIn);
void mouseScrollCallback(GLFWwindow *window, double xoffset, double yoffset);
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
    GLFWwindow *window = nullptr;
    if(glfwgladInitialization(&window)==-1) return -1;
    // 设置回调函数
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    glfwSetCursorPosCallback(window, mouseCallback);
    glfwSetScrollCallback(window, mouseScrollCallback);
    glfwSetMouseButtonCallback(window,nullptr);

    // cube
    GLLightingCube cube = GLLightingCube("resource/shader/chapter_2/lighter_cube");
    GLLighterCube lighter = GLLighterCube("resource/shader/chapter_2/lighter");
    cube.setUniform("light.ambient",  glm::vec3(0.2f, 0.2f, 0.2f));
    cube.setUniform("light.diffuse",  glm::vec3(0.5f, 0.5f, 0.5f)); // 将光照调暗了一些以搭配场景
    cube.setUniform("light.specular", glm::vec3(1.0f, 1.0f, 1.0f)); 
    cube.setUniform("material.ambient",  glm::vec3(1.0f, 0.5f, 0.31f));
    cube.setUniform("material.diffuse",  glm::vec3(1.0f, 0.5f, 0.31f));
    cube.setUniform("material.specular", glm::vec3(0.5f, 0.5f, 0.5f));
    cube.setUniform("material.shininess", 32.0f);
    float timeValue = glfwGetTime();
    // Render Loop
    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        processInput(window);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // view
        glm::mat4 view = camera.viewMatrix();
        glm::mat4 projection = camera.projectionMatrix(SRC_WIDTH, SRC_HEIGHT);
        // lighter
        glm::vec3 lightPos(sin(timeValue)*2.0f, cos(timeValue)*2.0f, 0.0f);
        lighter.setUniform("projection", projection);
        lighter.setUniform("view", view);
        glm::mat4 model = glm::translate(glm::mat4(1.0f), lightPos);
        model = glm::scale(model, glm::vec3(0.2f));
        lighter.setUniform("model", model);
        lighter.draw();
        // cube
        cube.setUniform("projection", projection);
        cube.setUniform("view", view);
        model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
        cube.setUniform("model", model);
        cube.setUniform("light.position", lightPos);
        cube.draw();
        // swap cache
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

// glfw: init
// -------------------------------------------------------
int glfwgladInitialization(GLFWwindow **window, const char *title)
{
    // 初始化
    glfwInit();
    // 设置主版本号 3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    // 设置副版本号 3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    // GLFW_OPENGL_CORE_PROFILE 对应核心模式
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    *window = glfwCreateWindow(SRC_WIDTH, SRC_HEIGHT, title, NULL, NULL);
    if (*window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    // 设置当前上下文
    glfwMakeContextCurrent(*window);
    // 初始化GLAD, GLAD回将所有设备的地址绑定到对应的指针上以供我们使用
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    // configure global opengl state
    glEnable(GL_DEPTH_TEST);
    // tell GLFW to capture our mouse
    glfwSetInputMode(*window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    return 0;
}

void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}
void framebufferSizeCallback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}
// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouseCallback(GLFWwindow *window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if ((glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_CONTROL)) && glfwGetMouseButton(window,GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
    {
        if (firstMouse)
        {
            lastX = xpos;
            lastY = ypos;
            firstMouse = false;
        }
        float xoffset = xpos - lastX;
        float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
        camera.processMouseMovement(xoffset / 10.0f, yoffset /10.0f);
    }
    else if(glfwGetMouseButton(window,GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
    {
        if (firstMouse)
        {
            lastX = xpos;
            lastY = ypos;
            firstMouse = false;
        }
        float xoffset = xpos - lastX;
        float yoffset = lastY - ypos;
        camera.processDirection(CameraMovement::LEFT, xoffset * deltaTime * 10);
        camera.processDirection(CameraMovement::DOWN, yoffset * deltaTime * 10);
        lastX = xpos;
        lastY = ypos;
    }
    else if(glfwGetMouseButton(window,GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
    {
        firstMouse = true;
    }
}
// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void mouseScrollCallback(GLFWwindow *window, double xoffset, double yoffset)
{
    camera.processMouseScroll(static_cast<float>(yoffset));
}
