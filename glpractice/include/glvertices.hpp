#ifndef HSB_GL_BASIC_VERTICES
#define HSB_GL_BASIC_VERTICES

#include "glad/glad.h"
#include "KHR/khrplatform.h"
#include "glfw3.h"
#include "glmesh.h"
#include <iostream>
#include <vector>
#include <glm.hpp>
#include <type_traits>

// (PTN) Postion Normal Texcoords
struct GLBasicPTNVertex
{
    glm::vec3 Position;  // 顶点位置
    glm::vec3 Normal;    // 顶点法线
    glm::vec2 TexCoords; // 纹理坐标
    GLBasicPTNVertex(glm::vec3 a, glm::vec3 b, glm::vec2 c) : Position(a), Normal(b), TexCoords(c) {}
};
std::vector<GLBasicPTNVertex> fromCStylePTNVertices(float *arrays, int length)
{
    std::vector<GLBasicPTNVertex> tmps;
    for (int i = 0; i < length; i += 8)
    {
        tmps.push_back(GLBasicPTNVertex(
            glm::vec3(arrays[i], arrays[i + 1], arrays[i + 2]),
            glm::vec3(arrays[i + 3], arrays[i + 4], arrays[i + 5]),
            glm::vec2(arrays[i + 6], arrays[i + 7])));
    }
    return tmps;
}
// (PC) Postion Color
struct GLBasicPCVertex
{
    glm::vec3 Position; // 顶点位置
    glm::vec3 Color;    // 颜色
    GLBasicPCVertex(glm::vec3 a, glm::vec3 b) : Position(a), Color(b) {}
};
std::vector<GLBasicPCVertex> fromCStylePCVertices(float *arrays, int length)
{
    std::vector<GLBasicPCVertex> tmps;
    for (int i = 0; i < length; i += 6)
    {
        tmps.push_back(GLBasicPCVertex(
            glm::vec3(arrays[i], arrays[i + 1], arrays[i + 2]),
            glm::vec3(arrays[i + 3], arrays[i + 4], arrays[i + 5])));
    }
    return tmps;
}
// (PCT) Postion Color Texcoords
struct GLBasicPCTVertex
{
    glm::vec3 Position;  // 顶点位置
    glm::vec3 Color;     // 颜色
    glm::vec2 TexCoords; // 纹理坐标
    GLBasicPCTVertex(glm::vec3 a, glm::vec3 b, glm::vec2 c) : Position(a), Color(b), TexCoords(c) {}
};
std::vector<GLBasicPCTVertex> fromCStylePCTVertices(float *arrays, int length)
{
    std::vector<GLBasicPCTVertex> tmps;
    for (int i = 0; i < length; i += 8)
    {
        tmps.push_back(GLBasicPCTVertex(
            glm::vec3(arrays[i], arrays[i + 1], arrays[i + 2]),
            glm::vec3(arrays[i + 3], arrays[i + 4], arrays[i + 5]),
            glm::vec2(arrays[i + 6], arrays[i + 7])));
    }
    return tmps;
}
// (PT) Postion Texcoords
struct GLBasicPTVertex
{
    glm::vec3 Position;  // 顶点位置
    glm::vec2 TexCoords; // 纹理坐标
    GLBasicPTVertex(glm::vec3 a, glm::vec2 b) : Position(a), TexCoords(b) {}
};
std::vector<GLBasicPTVertex> fromCStylePTVertices(float *arrays, int length)
{
    std::vector<GLBasicPTVertex> tmps;
    for (int i = 0; i < length; i += 5)
    {
        tmps.push_back(GLBasicPTVertex(
            glm::vec3(arrays[i], arrays[i + 1], arrays[i + 2]),
            glm::vec2(arrays[i + 3], arrays[i + 4])));
    }
    return tmps;
}

// 帧缓冲对象
struct FrameCacheObject{
    GLuint id;
    bool use;
};

template <typename T>
class GLBasicVerticesObj
{
public:
    /*  网格数据  */
    std::vector<T> m_vertices;
    std::vector<GLuint> m_indices;
    /*  构造函数  */
    GLBasicVerticesObj(std::vector<T> vertices, std::vector<GLuint> indices)
        : m_vertices(vertices), m_indices(indices)
    {
        setupVerticesObj();
    }
    GLBasicVerticesObj(std::vector<T> vertices)
        : m_vertices(vertices)
    {
        setupVerticesObj();
    }
    /*  渲染  */
    void draw(GLSingleShader &shader, GLenum mode = GL_TRIANGLES)
    {
        // 绘制网格
        glUseProgram(shader.getShaderProgram());
        glBindVertexArray(VAO);
        if (!m_indices.empty())
            glDrawElements(mode, static_cast<GLuint>(m_indices.size()), GL_UNSIGNED_INT, 0);
        else
            glDrawArrays(mode, 0, m_vertices.size());

        glBindVertexArray(0);
    }

private:
    /*  渲染数据  */
    unsigned int VAO, VBO, EBO;


    /*  初始化设置  */
    void setupVerticesObj()
    {
        // 设置顶点缓存和元素索引缓存
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        if (!m_indices.empty())
        {
            glGenBuffers(1, &EBO);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(unsigned int), &m_indices[0], GL_STATIC_DRAW);
        }
        // 顶点位置
        glEnableVertexAttribArray(0);

        // (PTN) Postion Normal Texcoords
        if (std::is_same<T, GLBasicPTNVertex>::value)
        {
            glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(GLBasicPTNVertex), &m_vertices[0], GL_STATIC_DRAW);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLBasicPTNVertex), (void *)0);
            // 顶点法线
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GLBasicPTNVertex), (void *)offsetof(GLBasicPTNVertex, Normal));
            // 顶点纹理坐标
            glEnableVertexAttribArray(2);
            glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(GLBasicPTNVertex), (void *)offsetof(GLBasicPTNVertex, TexCoords));
        }
        // (PC) Postion Color
        else if (std::is_same<T, GLBasicPCVertex>::value)
        {
            // 坐标
            glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(GLBasicPCVertex), &m_vertices[0], GL_STATIC_DRAW);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLBasicPCVertex), (void *)0);
            // 颜色
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GLBasicPCVertex), (void *)offsetof(GLBasicPCVertex, Color));
        }
        // (PCT) Postion Color Texcoords
        else if (std::is_same<T, GLBasicPCTVertex>::value)
        {
            // 坐标
            glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(GLBasicPCTVertex), &m_vertices[0], GL_STATIC_DRAW);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLBasicPCTVertex), (void *)0);
            // 颜色
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GLBasicPCTVertex), (void *)offsetof(GLBasicPCTVertex, Color));
            // 顶点纹理坐标
            glEnableVertexAttribArray(2);
            glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(GLBasicPCTVertex), (void *)offsetof(GLBasicPCTVertex, TexCoords));
        }
        // (PT) Postion Texcoords
        else if (std::is_same<T, GLBasicPTVertex>::value)
        {
            // 坐标
            glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(GLBasicPTVertex), &m_vertices[0], GL_STATIC_DRAW);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLBasicPTVertex), (void *)0);
            // 顶点纹理坐标
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(GLBasicPTVertex), (void *)offsetof(GLBasicPTVertex, TexCoords));
        }
        else
        {
            std::cout << "ERROR! TYPE OF VERTEX NOT SUPPORT! \n";
        }
        glBindVertexArray(0);
    }
};


class GLBasicFrameBufferObj{
    public:
        GLBasicFrameBufferObj(int screen_width, int screen_height)
        {
            // bind screen vertices buffer
            glGenVertexArrays(1, &quadVAO);
            glGenBuffers(1, &quadVBO);
            glBindVertexArray(quadVAO);
            glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
            // 坐标
            static float quadVertices[] = { 
                // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
                // positions   // texCoords
                -1.0f,  1.0f,  0.0f, 1.0f,
                -1.0f, -1.0f,  0.0f, 0.0f,
                1.0f, -1.0f,  1.0f, 0.0f,

                -1.0f,  1.0f,  0.0f, 1.0f,
                1.0f, -1.0f,  1.0f, 0.0f,
                1.0f,  1.0f,  1.0f, 1.0f
            };
            glBufferData(GL_ARRAY_BUFFER, 24 * sizeof(float), &quadVertices[0], GL_STATIC_DRAW);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float), (void *)0);
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float), (void *)(sizeof(float) * 3));
            setFrameBuffer(screen_width,screen_height);
        }
        /* 渲染缓冲 */
        void draw(GLSingleShader &screen_shader, GLenum mode = GL_TRIANGLES)
        {
            // now bind back to default framebuffer and draw a quad plane with the attached framebuffer color texture
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glDisable(GL_DEPTH_TEST); // disable depth test so screen-space quad isn't discarded due to depth test.
            glDisable(GL_STENCIL_TEST);
            glDisable(GL_BLEND);
            glDisable(GL_CULL_FACE);
            // clear all relevant buffers
            glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // set clear color to white (not really necessary actually, since we won't be able to see behind the quad anyways)
            glClear(GL_COLOR_BUFFER_BIT);
            
            glUseProgram(screen_shader.getShaderProgram());
            glBindVertexArray(quadVAO);
            glBindTexture(GL_TEXTURE_2D, TBO);	// use the color attachment texture as the texture of the quad plane
            glDrawArrays(mode, 0, 6);
            // glEnable(GL_DEPTH_TEST);
        }
        /* 绑定缓冲 */
        void bindBuffer()
        {
            glBindFramebuffer(GL_FRAMEBUFFER, FBO);
        }
    private:
        /*  帧缓冲 */
        GLuint FBO,RBO,TBO;
        GLuint quadVAO, quadVBO;
        /* 设置帧缓冲 */
        void setFrameBuffer(const int width, const int height)
        {
            // bind frame buffer
            glGenFramebuffers(1, &FBO);
            glBindFramebuffer(GL_FRAMEBUFFER, FBO);
            // create a color attachment texture
            glGenTextures(1, &TBO);
            glBindTexture(GL_TEXTURE_2D,  TBO);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, TBO, 0);
            // create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
            glGenRenderbuffers(1, &RBO);
            glBindRenderbuffer(GL_RENDERBUFFER, RBO);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height); // use a single renderbuffer object for both a depth AND stencil buffer.
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO); // now actually attach it
            // now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
            if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
                std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete! \n" ;
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        } 
};

#endif
