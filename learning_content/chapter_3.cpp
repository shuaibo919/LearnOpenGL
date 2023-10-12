// self header
#include "glshape.h"
#include "glcamera.h"
#include "glutils.h"
#include "glmesh.h"
#include <string>
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
    if (glfwgladInitialization(&window, SRC_WIDTH, SRC_HEIGHT) == -1)
        return -1;

    // 设置回调函数
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    glfwSetCursorPosCallback(window, mouseCallback);
    glfwSetScrollCallback(window, mouseScrollCallback);
    glfwSetMouseButtonCallback(window, nullptr);

    // 加载shader
    GLSingleShader shader("resource/shader/chapter_3/model");
    // 加载模型
    GLBasicModel nanosuit("resource/nanosuit/nanosuit.obj");
    // 创建光源
    GLLighterCube lighter = GLLighterCube("resource/shader/chapter_2/lighter");
    glm::vec3 pointLightPositions[] = {glm::vec3(0.7f, 0.7f, 0.0f)};
    // 设置光照
    shader.setUniform("dirLight.direction", glm::vec3(-0.2f, -1.0f, -0.3f));
    shader.setUniform("dirLight.ambient", glm::vec3(0.05f, 0.05f, 0.05f));
    shader.setUniform("dirLight.diffuse", glm::vec3(0.4f, 0.4f, 0.4f));
    shader.setUniform("dirLight.specular", glm::vec3(0.5f, 0.5f, 0.5f));
    shader.setUniform("material.diffuse", 0);
    shader.setUniform("material.specular", 1);
    shader.setUniform("material.shininess", 32.0f);
    shader.setUniform("pointLight.position", pointLightPositions[0]);
    shader.setUniform("pointLight.ambient", glm::vec3(0.15f, 0.15f, 0.15f));
    shader.setUniform("pointLight.diffuse", glm::vec3(0.8f, 0.8f, 0.8f));
    shader.setUniform("pointLight.specular", glm::vec3(1.0f, 1.0f, 1.0f));
    shader.setUniform("pointLight.constant", 1.0f);
    shader.setUniform("pointLight.linear", 0.009f);
    shader.setUniform("pointLight.quadratic", 0.0032f);

    float angle = 0.0f;
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
        lighter.setUniform("projection", projection);
        lighter.setUniform("view", view);

        glm::mat4 model = glm::translate(glm::mat4(1.0f), pointLightPositions[0]);
        model = glm::scale(model, glm::vec3(0.05f));
        lighter.setUniform("model", model);
        lighter.draw();
        // model(nanosuit) draw
        model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
        model = glm::rotate(model, glm::radians(angle += 0.025f), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
        shader.setUniform("projection", projection);
        shader.setUniform("view", view);
        shader.setUniform("model", model);
        shader.setUniform("viewPos", camera.positionVector());
        nanosuit.draw(shader);
        // swap cache
        glfwSwapBuffers(window);
        glfwPollEvents();
        if (angle >= 360.0f)
            angle = 0.0f;
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

    if ((glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_CONTROL)) && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
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
    else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
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
