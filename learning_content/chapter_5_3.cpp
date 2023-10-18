// advance opengl
// self header
#include "glshape.h"
#include "glcamera.h"
#include "glutils.h"
#include "glmesh.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "glvertices.hpp"
#include "glshadow.hpp"
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
GLBasicCamera camera(glm::vec3(0.0f, 0.0f, 3.0f));
ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
// render func
GLuint woodTexture;
void render_scene(GLSingleShader &shader);

int main()
{
    // 初始化
    GLFWwindow *window = nullptr;
    if (glfwgladInitialization(&window, SRC_WIDTH, SRC_HEIGHT, "Shadow Mapping") == -1)
        return -1;
    glfwSwapInterval(1);

    // 设置回调函数
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    glfwSetCursorPosCallback(window, mouseCallback);
    glfwSetScrollCallback(window, mouseScrollCallback);

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

    // 启用深度测试
    glEnable(GL_DEPTH_TEST);
    // 创建阴影对象
    const GLuint SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
    auto lightShadow = GLBasicShadow<SHADOW_WIDTH, SHADOW_HEIGHT>();
    // 创建shader
    GLSingleShader shader("resource/shader/chapter_5/shadow/shadow_mapping");
    GLSingleShader simpleDepthShader("resource/shader/chapter_5/shadow/shadow_mapping_depth");
    shader.use();
    shader.setUniform("diffuseTexture", 0);
    shader.setUniform("shadowMap", 1);
    glm::vec3 lightPos(-2.0f, 4.0f, -1.0f);
    // 加载纹理
    woodTexture = autoLoadTexture("resource/img/wood.png");

    // Render Loop
    while (!glfwWindowShouldClose(window))
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::Begin("Parameter Configuration");
        ImGui::ColorEdit3("clear color", (float *)&clear_color);
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
        ImGui::End();
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        processInput(window);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // Check and call events
        glfwPollEvents();

        float near_plane = 1.0f, far_plane = 7.5f;
        glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
        glm::mat4 lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
        glm::mat4 lightSpaceMatrix = lightProjection * lightView;
        // render scene from light's point of view
        simpleDepthShader.use();
        simpleDepthShader.setUniform("lightSpaceMatrix", lightSpaceMatrix);
        lightShadow.caputureRenderingToTexture(simpleDepthShader, render_scene, SRC_WIDTH, SRC_HEIGHT);

        // set light uniforms
        shader.setUniform("projection", camera.projectionMatrix(SRC_WIDTH, SRC_HEIGHT));
        shader.setUniform("view", camera.viewMatrix());
        shader.setUniform("viewPos", camera.positionVector());
        shader.setUniform("lightPos", lightPos);
        shader.setUniform("lightSpaceMatrix", lightSpaceMatrix);
        // bind texture
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, woodTexture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, lightShadow.getDepthMap());
        render_scene(shader);
        /* render Depth map to quad for visual debugging */
        // glUseProgram(debugDepthShader.getShaderProgram());
        // debugDepthShader.setUniform("near_plane", near_plane);
        // debugDepthShader.setUniform("far_plane", far_plane);
        // shadow.draw();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}

float cube_vertices[] = {
    // Back face
    -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, // Bottom-left
    0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,   // top-right
    0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f,  // bottom-right
    0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,   // top-right
    -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
    -0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,  // top-left
    // Front face
    -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom-left
    0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,  // bottom-right
    0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,   // top-right
    0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,   // top-right
    -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,  // top-left
    -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom-left
    // Left face
    -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,   // top-right
    -0.5f, 0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f,  // top-left
    -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-left
    -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-left
    -0.5f, -0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,  // bottom-right
    -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,   // top-right
    // Right face
    0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,   // top-left
    0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-right
    0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,  // top-right
    0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-right
    0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,   // top-left
    0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,  // bottom-left
    // Bottom face
    -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, // top-right
    0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f,  // top-left
    0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,   // bottom-left
    0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,   // bottom-left
    -0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f,  // bottom-right
    -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, // top-right
    // Top face
    -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, // top-left
    0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,   // bottom-right
    0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,  // top-right
    0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,   // bottom-right
    -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, // top-left
    -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f   // bottom-left
};

void render_cube(GLSingleShader &shader)
{
    static GLBasicVerticesObj<GLBasicPNTVertex> cube(fromCStylePNTVertices(cube_vertices, 288));
    cube.draw(shader);
}

GLfloat planeVertices[] = {
    // Positions          // Normals         // Texture Coords
    25.0f, -0.5f, 25.0f, 0.0f, 1.0f, 0.0f, 25.0f, 0.0f,
    -25.0f, -0.5f, -25.0f, 0.0f, 1.0f, 0.0f, 0.0f, 25.0f,
    -25.0f, -0.5f, 25.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,

    25.0f, -0.5f, 25.0f, 0.0f, 1.0f, 0.0f, 25.0f, 0.0f,
    25.0f, -0.5f, -25.0f, 0.0f, 1.0f, 0.0f, 25.0f, 25.0f,
    -25.0f, -0.5f, -25.0f, 0.0f, 1.0f, 0.0f, 0.0f, 25.0f};

void render_plane(GLSingleShader &shader)
{
    static GLBasicVerticesObj<GLBasicPNTVertex> plane(fromCStylePNTVertices(planeVertices, 48));
    plane.draw(shader);
}

void render_scene(GLSingleShader &shader)
{
    // floor
    glm::mat4 model = glm::mat4(1.0f);
    shader.setUniform("model", model);
    render_plane(shader);
    // cubes
    model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 1.5f, 0.0));
    shader.setUniform("model", model);
    render_cube(shader);
    model = glm::translate(glm::mat4(1.0f), glm::vec3(2.0f, 0.0f, 1.0));
    shader.setUniform("model", model);
    render_cube(shader);
    model = glm::translate(glm::mat4(1.0f), glm::vec3(-1.0f, 0.0f, 2.0));
    model = glm::rotate(model, glm::radians(60.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
    model = glm::scale(model, glm::vec3(0.5));
    shader.setUniform("model", model);
    render_cube(shader);
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
