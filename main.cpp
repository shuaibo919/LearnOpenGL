#include <iostream>
#include "glad/glad.h"
#include "KHR/khrplatform.h"
#include "glfw3.h"

using namespace std;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
int main()

{

    glfwInit();
    // 设置主版本号 3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    // 设置副版本号 3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    // GLFW_OPENGL_CORE_PROFILE 对应核心模式 
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;  
    }
    // 设置当前上下文
    glfwMakeContextCurrent(window); 
    // 设置FramebufferSize的回调函数
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // 初始化GLAD, GLAD回将所有设备的地址绑定到对应的指针上以供我们使用
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    // Render Loop

    // 
    float vertices[] = {
        -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        0.0f,  0.5f, 0.0f
    };
    // 顶点缓冲对象(vertex buffer object)
    unsigned int VBO;
    glGenBuffers(1, &VBO);

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);
        glClearColor(0.2f, 0.6f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);   
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow* window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    std::cout<<"here be called!"<<std::endl;
    glViewport(0, 0, width, height);
}
