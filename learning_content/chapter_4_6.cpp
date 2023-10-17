// advance opengl
// self header
#include "glcamera.h"
#include "glutils.h"
#include "glmesh.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "glvertices.hpp"
#include <string>
#include <map>
#include <ext/matrix_relational.hpp>
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
GLBasicCamera camera(glm::vec3(0.0f, 0.0f, 55.0f));
float skyVertices[] = {
    // positions
    -1.0f, 1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,
    1.0f, 1.0f, -1.0f,
    -1.0f, 1.0f, -1.0f,

    -1.0f, -1.0f, 1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f, 1.0f, -1.0f,
    -1.0f, 1.0f, -1.0f,
    -1.0f, 1.0f, 1.0f,
    -1.0f, -1.0f, 1.0f,

    1.0f, -1.0f, -1.0f,
    1.0f, -1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,

    -1.0f, -1.0f, 1.0f,
    -1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f, -1.0f, 1.0f,
    -1.0f, -1.0f, 1.0f,

    -1.0f, 1.0f, -1.0f,
    1.0f, 1.0f, -1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f, -1.0f,

    -1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f, 1.0f,
    1.0f, -1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f, 1.0f,
    1.0f, -1.0f, 1.0f};
int main()
{
    // 初始化
    GLFWwindow *window = nullptr;
    if (glfwgladInitialization(&window, SRC_WIDTH, SRC_HEIGHT) == -1)
        return -1;
    glfwSwapInterval(1);

    // 设置回调函数
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    glfwSetCursorPosCallback(window, mouseCallback);
    glfwSetScrollCallback(window, mouseScrollCallback);
    glfwSetMouseButtonCallback(window, nullptr);

    // ImGUI Setup
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls
    ImGui::StyleColorsDark();                             // Setup Dear ImGui style
    ImGui_ImplGlfw_InitForOpenGL(window, true);           // Setup Platform/Renderer backends
    ImGui_ImplOpenGL3_Init("#version 130");
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    // 创建模型
    GLBasicModel planet("resource/planet/planet.obj");
    GLBasicModel rock("resource/rock/rock.obj");
    GLBasicVerticesObj<GLBasicPVertex> skyBox(fromCStylePVertices(skyVertices, 108));
    GLSingleShader shader("resource/shader/chapter_4/planet");
    GLSingleShader rock_shader("resource/shader/chapter_4/instancing");
    GLSingleShader sky_shader("resource/shader/chapter_4/skybox");
    sky_shader.setUniform("skybox", 0);
    GLuint skyTexure = loadCubemap("resource/spacebox", ".png");
    int amount = 2000;
    // 启用深度测试
    glEnable(GL_DEPTH_TEST);

    glm::mat4 *modelMatrices;
    modelMatrices = new glm::mat4[amount];
    srand(glfwGetTime()); // 初始化随机种子
    float radius = 50.0;
    float offset = 2.5f;
    for (unsigned int i = 0; i < amount; i++)
    {
        // 1. 位移：分布在半径为 'radius' 的圆形上，偏移的范围是 [-offset, offset]
        float angle = (float)i / (float)amount * 360.0f;
        float displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
        float x = sin(angle) * radius + displacement;
        displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
        float y = displacement * 0.4f; // 让行星带的高度比x和z的宽度要小
        displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
        float z = cos(angle) * radius + displacement;
        glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, z));

        // 2. 缩放：在 0.05 和 0.25f 之间缩放
        float scale = (rand() % 20) / 100.0f + 0.05;
        model = glm::scale(model, glm::vec3(scale));

        // 3. 旋转：绕着一个（半）随机选择的旋转轴向量进行随机的旋转
        float rotAngle = (rand() % 360);
        model = glm::rotate(model, rotAngle, glm::vec3(0.4f, 0.6f, 0.8f));

        // 4. 添加到矩阵的数组中
        modelMatrices[i] = model;
    }
    // 顶点缓冲对象
    unsigned int buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, amount * sizeof(glm::mat4), &modelMatrices[0], GL_STATIC_DRAW);

    for (unsigned int i = 0; i < rock.meshes.size(); i++)
    {
        unsigned int VAO = rock.meshes[i].getVAO();
        glBindVertexArray(VAO);
        // 顶点属性
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(glm::vec4), (void *)0);
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(glm::vec4), (void *)(sizeof(glm::vec4)));
        glEnableVertexAttribArray(5);
        glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(glm::vec4), (void *)(2 * sizeof(glm::vec4)));
        glEnableVertexAttribArray(6);
        glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(glm::vec4), (void *)(3 * sizeof(glm::vec4)));

        glVertexAttribDivisor(3, 1);
        glVertexAttribDivisor(4, 1);
        glVertexAttribDivisor(5, 1);
        glVertexAttribDivisor(6, 1);

        glBindVertexArray(0);
    }

    float angle = 0.0f;
    // Render Loop
    while (!glfwWindowShouldClose(window))
    {
        {
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();
            ImGui::Begin("Monitor");
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
            ImGui::End();
            if (angle >= 360.0f)
                angle = 0.0f;
        }
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        processInput(window);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        {
            glDepthFunc(GL_LEQUAL);
            glBindTexture(GL_TEXTURE_CUBE_MAP, skyTexure);
            sky_shader.setUniform("projection", camera.projectionMatrix(SRC_WIDTH, SRC_HEIGHT));
            sky_shader.setUniform("view", glm::mat4(glm::mat3(camera.viewMatrix())));
            skyBox.draw(sky_shader);
            glDepthFunc(GL_LESS);
            // 绘制行星
            glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -3.0f, 0.0f));
            model = glm::rotate(model, glm::radians(angle += 0.075f), glm::vec3(0.0f, 1.0f, 0.0f));
            model = glm::scale(model, glm::vec3(4.0f, 4.0f, 4.0f));
            shader.setUniform("projection", camera.projectionMatrix(SRC_WIDTH, SRC_HEIGHT));
            shader.setUniform("view", camera.viewMatrix());
            shader.setUniform("model", model);
            planet.draw(shader);

            // 绘制小行星
            rock_shader.setUniform("projection", camera.projectionMatrix(SRC_WIDTH, SRC_HEIGHT));
            rock_shader.setUniform("view", camera.viewMatrix());
            rock.draw(rock_shader, amount);
        }
        ImGui::Render(); // render imgui
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window); // swap buffer
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouseCallback(GLFWwindow *window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if ((glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS) && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
    {
        if (firstMouse)
        {
            lastX = xpos;
            lastY = ypos;
            firstMouse = false;
        }
        float xoffset = xpos - lastX;
        float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
        camera.processMouseMovement(xoffset / 10.0f, yoffset / 10.0f);
    }
    else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS && (glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS))
    {
        if (firstMouse)
        {
            lastX = xpos;
            lastY = ypos;
            firstMouse = false;
        }
        float xoffset = xpos - lastX;
        float yoffset = lastY - ypos;
        camera.processDirection(CameraMovement::LEFT, xoffset * deltaTime);
        camera.processDirection(CameraMovement::DOWN, yoffset * deltaTime);
        lastX = xpos;
        lastY = ypos;
    }
    else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
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
