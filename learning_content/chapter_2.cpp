// self header
#include "glshape.h"
#include "glcamera.h"
#include "glutils.h"
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

    // 光源
    GLLighterCube lighter = GLLighterCube("resource/shader/chapter_2/lighter");
    glm::vec3 pointLightPositions[] = {
        glm::vec3( 0.7f,  0.2f,  2.0f),
        glm::vec3( 2.3f, -3.3f, -4.0f),
        glm::vec3(-4.0f,  2.0f, -12.0f),
        glm::vec3( 0.0f,  0.0f, -3.0f)
    };

    // 纹理
    GLuint wood_texture = loadTexture("resource/img/advance_container.png", GL_RGBA);
    GLuint steer_texture = loadTexture("resource/img/container_specular.png", GL_RGBA);
    std::vector<GLuint> textures({wood_texture, steer_texture});

    // cube
    glm::vec3 cubePositions[] = {
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(2.0f, 5.0f, -3.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -1.3f)
    };

    GLLightingCube cube = GLLightingCube("resource/shader/chapter_2/multi_lighter_cube");
    cube.setUniform("dirLight.direction", glm::vec3(-0.2f, -1.0f, -0.3f));
    cube.setUniform("dirLight.ambient", glm::vec3(0.05f, 0.05f, 0.05f));
    cube.setUniform("dirLight.diffuse", glm::vec3(0.4f, 0.4f, 0.4f));
    cube.setUniform("dirLight.specular", glm::vec3(0.5f, 0.5f, 0.5f));
    for(int i =0; i< 4; ++i)
    {
        //cube.setVec3("pointLights[3].position", pointLightPositions[3]);
        cube.setUniform("pointLights[" + std::to_string(i) + "].ambient",glm::vec3(0.05f, 0.05f, 0.05f));
        cube.setUniform("pointLights[" + std::to_string(i) + "].diffuse", glm::vec3(0.8f, 0.8f, 0.8f));
        cube.setUniform("pointLights[" + std::to_string(i) + "].specular", glm::vec3(1.0f, 1.0f, 1.0f));
        cube.setUniform("pointLights[" + std::to_string(i) + "].constant", 1.0f);
        cube.setUniform("pointLights[" + std::to_string(i) + "].linear", 0.09f);
        cube.setUniform("pointLights[" + std::to_string(i) + "].quadratic", 0.032f);
    }
    cube.setUniform("material.diffuse", 0);
    cube.setUniform("material.specular", 1);
    cube.setUniform("material.shininess", 32.0f);

    cube.setUniform("spotLight.ambient", glm::vec3(0.0f, 0.0f, 0.0f));
    cube.setUniform("spotLight.diffuse", glm::vec3(1.0f, 1.0f, 1.0f));
    cube.setUniform("spotLight.specular", glm::vec3(1.0f, 1.0f, 1.0f));
    cube.setUniform("spotLight.constant", 1.0f);
    cube.setUniform("spotLight.linear", 0.09f);
    cube.setUniform("spotLight.quadratic", 0.032f);
    cube.setUniform("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
    cube.setUniform("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));    

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
        for(int i = 0; i< 4; ++i){
            glm::mat4 model = glm::translate(glm::mat4(1.0f), pointLightPositions[i]);
            model = glm::scale(model, glm::vec3(0.2f));
            lighter.setUniform("model", model);
            lighter.draw();
        }
        // cube
        cube.setUniform("projection", projection);
        cube.setUniform("view", view);
        float timeValue = glfwGetTime();
        for (unsigned int i = 0; i < 4; i++)
        {
            glm::mat4 model = glm::translate(glm::mat4(1.0f), cubePositions[i]);
            float angle = 20.0f * i;
            model = glm::rotate(model, angle + timeValue * glm::radians(50.0f) * (i != 0), glm::vec3(1.0f, 0.3f, 0.5f));
            // 摄像机
            cube.setUniform("model", model);
            for(int i =0;i<4;++i){
                cube.setUniform("pointLights[" + std::to_string(i) + "].position", pointLightPositions[i]);
            }
            cube.setUniform("viewPos", camera.positionVector());
            cube.setUniform("spotLight.position",camera.positionVector());
            cube.setUniform("spotLight.direction",camera.frontVector());
            cube.draw(textures);
        }
        // swap cache
        glfwSwapBuffers(window);
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
