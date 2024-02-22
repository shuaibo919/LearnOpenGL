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
#include <gtc/matrix_transform.hpp>
#include "stb_image.h"
#include <iostream>
#include <random>

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
void tprocessInput(GLFWwindow *window);
void renderScene(GLSingleShader &shader);
void renderCube();
void renderQuad();
std::vector<glm::mat4> getLightSpaceMatrices();
std::vector<glm::vec4> getFrustumCornersWorldSpace(const glm::mat4 &projview);
void drawCascadeVolumeVisualizers(const std::vector<glm::mat4> &lightMatrices, GLSingleShader *shader);

class BufferBase
{
    using Ptr = std::shared_ptr<BufferBase>;

protected:
    GLuint id;
    virtual void InitBuffer() = 0;

public:
    BufferBase() : id(0) {}
    virtual ~BufferBase() = default;

public:
    virtual void Bind() = 0;
    virtual void Unbind() = 0;

public:
    inline GLuint GetID() const { return id; }
};

class UnifromBuffer : public BufferBase
{
public:
    using Ptr = std::shared_ptr<UnifromBuffer>;
    void SubData(GLintptr offset, GLsizeiptr size, const void *data)
    {
        glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data);
    }
    void Bind() override
    {
        glBindBuffer(GL_UNIFORM_BUFFER, id);
    }
    void Unbind() override
    {
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }

private:
    size_t length;

public:
    UnifromBuffer() = delete;
    UnifromBuffer(GLsizeiptr size) : BufferBase(), m_size(size)
    {
        InitBuffer();
    }

    ~UnifromBuffer() = default;

private:
    GLsizeiptr m_size;
    void InitBuffer() override
    {
        glGenBuffers(1, &id);
        glBindBuffer(GL_UNIFORM_BUFFER, id);
        glBufferData(GL_UNIFORM_BUFFER, m_size, nullptr, GL_STATIC_DRAW);
        glBindBufferBase(GL_UNIFORM_BUFFER, 0, id);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }
};

class FrameBuffer : public BufferBase
{
public:
    using Ptr = std::shared_ptr<FrameBuffer>;

    FrameBuffer();
    ~FrameBuffer();

protected:
    void InitBuffer() override;

public:
    void ToTexture(int attachment, int texturePrimitive, int textureID);
    void BlitFrom(FrameBuffer::Ptr &frameBuffer, unsigned int width, unsigned int height);
    void SetRenderBuffer(int attachment, int renderBufferID);
    void SetTexture2D(int attachment, int textureId, GLenum textarget = GL_TEXTURE_2D, int level = 0);
    void SetTexture(int attachment, int textureId, int level = 0);
    void Bind() override;
    void Unbind() override;
    bool IsComplete();
};

class RenderBuffer : public BufferBase
{
public:
    using Ptr = std::shared_ptr<RenderBuffer>;

    RenderBuffer(unsigned int width, unsigned int height, int internalFormat = GL_DEPTH_COMPONENT);
    RenderBuffer() = default;
    virtual ~RenderBuffer();

protected:
    unsigned int m_width, m_height;
    int m_internalFormat;

protected:
    void InitBuffer() override;

public:
    void Bind() override;
    void Unbind() override;
    virtual void Rescale(unsigned int width, unsigned int height);

public:
    inline unsigned int GetWidth() const { return m_width; }
    inline unsigned int GetHeight() const { return m_height; }
    inline int GetInternalFormat() const { return m_internalFormat; }
};

RenderBuffer::RenderBuffer(unsigned int width, unsigned int height, int internalFormat)
    : BufferBase(), m_width(width), m_height(height), m_internalFormat(internalFormat)
{
    InitBuffer();
}

RenderBuffer::~RenderBuffer()
{
    Unbind();
    glDeleteRenderbuffers(1, &id);
}

void RenderBuffer::InitBuffer()
{
    glGenRenderbuffers(1, &id);
    glBindRenderbuffer(GL_RENDERBUFFER, id);
    glRenderbufferStorage(GL_RENDERBUFFER, m_internalFormat, m_width, m_height);
}

void RenderBuffer::Rescale(unsigned int width, unsigned int height)
{
    m_width = width;
    m_height = height;
    glBindRenderbuffer(GL_RENDERBUFFER, id);
    glRenderbufferStorage(GL_RENDERBUFFER, m_internalFormat, m_width, m_height);
}

void RenderBuffer::Bind()
{
    glBindRenderbuffer(GL_RENDERBUFFER, id);
}

void RenderBuffer::Unbind()
{
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

FrameBuffer::FrameBuffer()
    : BufferBase()
{
    InitBuffer();
}

FrameBuffer::~FrameBuffer()
{
    Unbind();
    glDeleteFramebuffers(1, &id);
}

void FrameBuffer::InitBuffer()
{
    glGenFramebuffers(1, &id);
    glBindFramebuffer(GL_FRAMEBUFFER, id);
}

void FrameBuffer::ToTexture(int attachment, int texturePrimitive, int textureID)
{
    glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, texturePrimitive, textureID, 0);
}

void FrameBuffer::BlitFrom(FrameBuffer::Ptr &frameBuffer, unsigned int width, unsigned int height)
{
    glBindFramebuffer(GL_READ_FRAMEBUFFER, id);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, frameBuffer->GetID());
    glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
}

void FrameBuffer::SetRenderBuffer(int attachment, int renderBufferID)
{
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachment, GL_RENDERBUFFER, renderBufferID);
}

void FrameBuffer::SetTexture2D(int attachment, int textureId, GLenum textarget, int level)
{
    glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, textarget, textureId, level);
}
void FrameBuffer::SetTexture(int attachment, int textureId, int level)
{
    glFramebufferTexture(GL_FRAMEBUFFER, attachment, textureId, level);
}

void FrameBuffer::Bind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, id);
}

void FrameBuffer::Unbind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

bool FrameBuffer::IsComplete()
{
    return glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
}

// settings
const unsigned int SCR_WIDTH = 1024;
const unsigned int SCR_HEIGHT = 720;

// framebuffer size
int fb_width;
int fb_height;

// camera
GLBasicCamera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = (float)SCR_WIDTH / 2.0;
float lastY = (float)SCR_HEIGHT / 2.0;
bool firstMouse = true;
float cameraNearPlane = 0.1f;
float cameraFarPlane = 100.0f;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

std::vector<float> shadowCascadeLevels{cameraFarPlane / 50.0f, cameraFarPlane / 25.0f, cameraFarPlane / 10.0f, cameraFarPlane / 2.0f};
int debugLayer = 0;

// meshes
unsigned int planeVAO;

// lighting info
// -------------
const glm::vec3 lightDir = glm::normalize(glm::vec3(20.0f, 50, 20.0f));
unsigned int lightFBO;
unsigned int lightDepthMaps;
constexpr unsigned int depthMapResolution = 4096;

bool showQuad = false;

std::random_device device;
std::mt19937 generator = std::mt19937(device());

std::vector<glm::mat4> lightMatricesCache;

int main()
{
    // generator.seed(2);
    //  glfw: initialize and configure
    //  ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwGetFramebufferSize(window, &fb_width, &fb_height);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // build and compile shaders
    // -------------------------
    GLSingleShader shader("resource/shader/CSM/shadowmap");
    GLSingleShader simpleDepthShader("resource/shader/CSM/shadowdepth", true);
    GLSingleShader debugDepthQuad("resource/shader/CSM/debug");
    GLSingleShader debugCascadeShader("resource/shader/CSM/csmdebug");

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float planeVertices[] = {
        // positions            // normals         // texcoords
        25.0f, -2.0f, 25.0f, 0.0f, 1.0f, 0.0f, 25.0f, 0.0f,
        -25.0f, -2.0f, 25.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
        -25.0f, -2.0f, -25.0f, 0.0f, 1.0f, 0.0f, 0.0f, 25.0f,
        25.0f, -2.0f, 25.0f, 0.0f, 1.0f, 0.0f, 25.0f, 0.0f,
        -25.0f, -2.0f, -25.0f, 0.0f, 1.0f, 0.0f, 0.0f, 25.0f,
        25.0f, -2.0f, -25.0f, 0.0f, 1.0f, 0.0f, 25.0f, 25.0f};
    // plane VAO
    unsigned int planeVBO;
    glGenVertexArrays(1, &planeVAO);
    glGenBuffers(1, &planeVBO);
    glBindVertexArray(planeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(6 * sizeof(float)));
    glBindVertexArray(0);

    // load textures
    // -------------
    unsigned int woodTexture = autoLoadTexture("resource/img/wood.png");

    // configure light FBO
    // -----------------------
    glGenFramebuffers(1, &lightFBO);

    glGenTextures(1, &lightDepthMaps);
    glBindTexture(GL_TEXTURE_2D_ARRAY, lightDepthMaps);
    glTexImage3D(
        GL_TEXTURE_2D_ARRAY, 0, GL_DEPTH_COMPONENT32F, depthMapResolution, depthMapResolution, int(shadowCascadeLevels.size()) + 1,
        0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    constexpr float bordercolor[] = {1.0f, 1.0f, 1.0f, 1.0f};
    glTexParameterfv(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BORDER_COLOR, bordercolor);

    glBindFramebuffer(GL_FRAMEBUFFER, lightFBO);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, lightDepthMaps, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    int status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!";
        throw 0;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // configure UBO
    // --------------------
    auto ubo = std::make_shared<UnifromBuffer>(16 * sizeof(glm::mat4));
    // shader configuration
    // --------------------
    shader.use();
    shader.setUniform("diffuseTexture", 0);
    shader.setUniform("shadowMap", 1);
    debugDepthQuad.use();
    debugDepthQuad.setUniform("depthMap", 0);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        tprocessInput(window);

        // change light position over time
        // lightPos.x = sin(glfwGetTime()) * 3.0f;
        // lightPos.z = cos(glfwGetTime()) * 2.0f;
        // lightPos.y = 5.0 + cos(glfwGetTime()) * 1.0f;

        // render
        // ------
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // 0. UBO setup
        const auto lightMatrices = getLightSpaceMatrices();
        lightMatricesCache = lightMatrices;
        ubo->Bind();
        for (size_t i = 0; i < lightMatrices.size(); ++i)
        {
            ubo->SubData(i * sizeof(glm::mat4), sizeof(glm::mat4), &lightMatrices[i]);
        }
        ubo->Unbind();

        // 1. render depth of scene to texture (from light's perspective)
        // --------------------------------------------------------------
        // lightProjection = glm::perspective(glm::radians(45.0f), (GLfloat)SHADOW_WIDTH / (GLfloat)SHADOW_HEIGHT, near_plane, far_plane); // note that if you use a perspective projection matrix you'll have to change the light position as the current light position isn't enough to reflect the whole scene
        // render scene from light's point of view
        simpleDepthShader.use();

        glBindFramebuffer(GL_FRAMEBUFFER, lightFBO);
        glViewport(0, 0, depthMapResolution, depthMapResolution);
        glClear(GL_DEPTH_BUFFER_BIT);
        glCullFace(GL_FRONT); // peter panning
        renderScene(simpleDepthShader);
        glCullFace(GL_BACK);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // reset viewport
        glViewport(0, 0, fb_width, fb_height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // 2. render scene as normal using the generated depth/shadow map
        // --------------------------------------------------------------
        glViewport(0, 0, fb_width, fb_height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        shader.use();
        const glm::mat4 projection = camera.projectionMatrix(SCR_WIDTH, SCR_HEIGHT);
        const glm::mat4 view = camera.viewMatrix();
        shader.setUniform("projection", projection);
        shader.setUniform("view", view);
        // set light uniforms
        shader.setUniform("viewPos", camera.positionVector());
        shader.setUniform("lightDir", lightDir);
        shader.setUniform("farPlane", cameraFarPlane);
        shader.setUniform("cascadeCount", (int)shadowCascadeLevels.size());
        for (size_t i = 0; i < shadowCascadeLevels.size(); ++i)
        {
            shader.setUniform("cascadePlaneDistances[" + std::to_string(i) + "]", shadowCascadeLevels[i]);
        }
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, woodTexture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D_ARRAY, lightDepthMaps);
        renderScene(shader);

        if (lightMatricesCache.size() != 0)
        {
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            debugCascadeShader.use();
            debugCascadeShader.setUniform("projection", projection);
            debugCascadeShader.setUniform("view", view);
            drawCascadeVolumeVisualizers(lightMatricesCache, &debugCascadeShader);
            glDisable(GL_BLEND);
        }

        // render Depth map to quad for visual debugging
        // ---------------------------------------------
        debugDepthQuad.use();
        debugDepthQuad.setUniform("layer", 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D_ARRAY, lightDepthMaps);
        if (true)
        {
            renderQuad();
        }

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &planeVAO);
    glDeleteBuffers(1, &planeVBO);

    glfwTerminate();
    return 0;
}

// renders the 3D scene
// --------------------
void renderScene(GLSingleShader &shader)
{
    // floor
    glm::mat4 model = glm::mat4(1.0f);
    shader.setUniform("model", model);
    glBindVertexArray(planeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    static std::vector<glm::mat4> modelMatrices;
    if (modelMatrices.size() == 0)
    {
        for (int i = 0; i < 10; ++i)
        {
            static std::uniform_real_distribution<float> offsetDistribution = std::uniform_real_distribution<float>(-10, 10);
            static std::uniform_real_distribution<float> scaleDistribution = std::uniform_real_distribution<float>(1.0, 2.0);
            static std::uniform_real_distribution<float> rotationDistribution = std::uniform_real_distribution<float>(0, 180);

            auto model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(offsetDistribution(generator), offsetDistribution(generator) + 10.0f, offsetDistribution(generator)));
            model = glm::rotate(model, glm::radians(rotationDistribution(generator)), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
            model = glm::scale(model, glm::vec3(scaleDistribution(generator)));
            modelMatrices.push_back(model);
        }
    }

    for (const auto &model : modelMatrices)
    {
        shader.setUniform("model", model);
        renderCube();
    }
}

// renderCube() renders a 1x1 3D cube in NDC.
// -------------------------------------------------
unsigned int cubeVAO = 0;
unsigned int cubeVBO = 0;
void renderCube()
{
    // initialize (if necessary)
    if (cubeVAO == 0)
    {
        float vertices[] = {
            // back face
            -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
            1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,   // top-right
            1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f,  // bottom-right
            1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,   // top-right
            -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
            -1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,  // top-left
            // front face
            -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom-left
            1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,  // bottom-right
            1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,   // top-right
            1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,   // top-right
            -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,  // top-left
            -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom-left
            // left face
            -1.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,   // top-right
            -1.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f,  // top-left
            -1.0f, -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-left
            -1.0f, -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-left
            -1.0f, -1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,  // bottom-right
            -1.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,   // top-right
                                                                // right face
            1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,     // top-left
            1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,   // bottom-right
            1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,    // top-right
            1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,   // bottom-right
            1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,     // top-left
            1.0f, -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,    // bottom-left
            // bottom face
            -1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, // top-right
            1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f,  // top-left
            1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,   // bottom-left
            1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,   // bottom-left
            -1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f,  // bottom-right
            -1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, // top-right
            // top face
            -1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, // top-left
            1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,   // bottom-right
            1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,  // top-right
            1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,   // bottom-right
            -1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, // top-left
            -1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f   // bottom-left
        };
        glGenVertexArrays(1, &cubeVAO);
        glGenBuffers(1, &cubeVBO);
        // fill buffer
        glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        // link vertex attributes
        glBindVertexArray(cubeVAO);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(6 * sizeof(float)));
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
    // render Cube
    glBindVertexArray(cubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}

// renderQuad() renders a 1x1 XY quad in NDC
// -----------------------------------------
unsigned int quadVAO = 0;
unsigned int quadVBO;
void renderQuad()
{
    if (quadVAO == 0)
    {
        float quadVertices[] = {
            // positions        // texture Coords
            -1.0f,
            1.0f,
            0.0f,
            0.0f,
            1.0f,
            -1.0f,
            -1.0f,
            0.0f,
            0.0f,
            0.0f,
            1.0f,
            1.0f,
            0.0f,
            1.0f,
            1.0f,
            1.0f,
            -1.0f,
            0.0f,
            1.0f,
            0.0f,
        };
        // setup plane VAO
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
    }
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}

std::vector<GLuint> visualizerVAOs;
std::vector<GLuint> visualizerVBOs;
std::vector<GLuint> visualizerEBOs;
void drawCascadeVolumeVisualizers(const std::vector<glm::mat4> &lightMatrices, GLSingleShader *shader)
{
    visualizerVAOs.resize(8);
    visualizerEBOs.resize(8);
    visualizerVBOs.resize(8);

    const GLuint indices[] = {
        0, 2, 3,
        0, 3, 1,
        4, 6, 2,
        4, 2, 0,
        5, 7, 6,
        5, 6, 4,
        1, 3, 7,
        1, 7, 5,
        6, 7, 3,
        6, 3, 2,
        1, 5, 4,
        0, 1, 4};

    const glm::vec4 colors[] = {
        {1.0, 0.0, 0.0, 0.5f},
        {0.0, 1.0, 0.0, 0.5f},
        {0.0, 0.0, 1.0, 0.5f},
    };

    for (int i = 0; i < lightMatrices.size(); ++i)
    {
        const auto corners = getFrustumCornersWorldSpace(lightMatrices[i]);
        std::vector<glm::vec3> vec3s;
        for (const auto &v : corners)
        {
            vec3s.push_back(glm::vec3(v));
        }

        glGenVertexArrays(1, &visualizerVAOs[i]);
        glGenBuffers(1, &visualizerVBOs[i]);
        glGenBuffers(1, &visualizerEBOs[i]);

        glBindVertexArray(visualizerVAOs[i]);

        glBindBuffer(GL_ARRAY_BUFFER, visualizerVBOs[i]);
        glBufferData(GL_ARRAY_BUFFER, vec3s.size() * sizeof(glm::vec3), &vec3s[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, visualizerEBOs[i]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, 36 * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void *)0);

        glBindVertexArray(visualizerVAOs[i]);
        shader->setUniform("color", colors[i % 3]);
        glDrawElements(GL_TRIANGLES, GLsizei(36), GL_UNSIGNED_INT, 0);

        glDeleteBuffers(1, &visualizerVBOs[i]);
        glDeleteBuffers(1, &visualizerEBOs[i]);
        glDeleteVertexArrays(1, &visualizerVAOs[i]);

        glBindVertexArray(0);
    }

    visualizerVAOs.clear();
    visualizerEBOs.clear();
    visualizerVBOs.clear();
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void tprocessInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.processDirection(CameraMovement::FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.processDirection(CameraMovement::BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.processDirection(CameraMovement::LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.processDirection(CameraMovement::RIGHT, deltaTime);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow *window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.processMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
    camera.processMouseScroll(static_cast<float>(yoffset));
}

// utility function for loading a 2D texture from file

std::vector<glm::vec4> getFrustumCornersWorldSpace(const glm::mat4 &projview)
{
    const auto inv = glm::inverse(projview);

    std::vector<glm::vec4> frustumCorners;
    for (unsigned int x = 0; x < 2; ++x)
    {
        for (unsigned int y = 0; y < 2; ++y)
        {
            for (unsigned int z = 0; z < 2; ++z)
            {
                const glm::vec4 pt = inv * glm::vec4(2.0f * x - 1.0f, 2.0f * y - 1.0f, 2.0f * z - 1.0f, 1.0f);
                frustumCorners.push_back(pt / pt.w);
            }
        }
    }

    return frustumCorners;
}

std::vector<glm::vec4> getFrustumCornersWorldSpace(const glm::mat4 &proj, const glm::mat4 &view)
{
    return getFrustumCornersWorldSpace(proj * view);
}

glm::mat4 getLightSpaceMatrix(const float nearPlane, const float farPlane)
{
    const auto proj = camera.projectionMatrix(SCR_WIDTH, SCR_HEIGHT);
    const auto corners = getFrustumCornersWorldSpace(proj, camera.viewMatrix());

    glm::vec3 center = glm::vec3(0, 0, 0);
    for (const auto &v : corners)
    {
        center += glm::vec3(v);
    }
    center /= corners.size();

    const auto lightView = glm::lookAt(center + lightDir, center, glm::vec3(0.0f, 1.0f, 0.0f));

    float minX = std::numeric_limits<float>::max();
    float maxX = std::numeric_limits<float>::lowest();
    float minY = std::numeric_limits<float>::max();
    float maxY = std::numeric_limits<float>::lowest();
    float minZ = std::numeric_limits<float>::max();
    float maxZ = std::numeric_limits<float>::lowest();
    for (const auto &v : corners)
    {
        const auto trf = lightView * v;
        minX = std::min(minX, trf.x);
        maxX = std::max(maxX, trf.x);
        minY = std::min(minY, trf.y);
        maxY = std::max(maxY, trf.y);
        minZ = std::min(minZ, trf.z);
        maxZ = std::max(maxZ, trf.z);
    }

    // Tune this parameter according to the scene
    constexpr float zMult = 10.0f;
    if (minZ < 0)
    {
        minZ *= zMult;
    }
    else
    {
        minZ /= zMult;
    }
    if (maxZ < 0)
    {
        maxZ /= zMult;
    }
    else
    {
        maxZ *= zMult;
    }

    const glm::mat4 lightProjection = glm::ortho(minX, maxX, minY, maxY, minZ, maxZ);
    return lightProjection * lightView;
}

std::vector<glm::mat4> getLightSpaceMatrices()
{
    std::vector<glm::mat4> ret;
    for (size_t i = 0; i < shadowCascadeLevels.size() + 1; ++i)
    {
        if (i == 0)
        {
            ret.push_back(getLightSpaceMatrix(cameraNearPlane, shadowCascadeLevels[i]));
        }
        else if (i < shadowCascadeLevels.size())
        {
            ret.push_back(getLightSpaceMatrix(shadowCascadeLevels[i - 1], shadowCascadeLevels[i]));
        }
        else
        {
            ret.push_back(getLightSpaceMatrix(shadowCascadeLevels[i - 1], cameraFarPlane));
        }
    }
    return ret;
}