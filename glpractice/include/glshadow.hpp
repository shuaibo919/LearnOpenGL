#ifndef HSB_GL_BASIC_SHADOW
#define HSB_GL_BASIC_SHADOW

#include "glad/glad.h"
#include "KHR/khrplatform.h"
#include "glfw3.h"
#include "glmesh.h"
#include <vector>
#include <glm.hpp>
#include <functional>

template <GLuint T_SHADOW_WIDTH = 1024, GLuint T_SHADOW_HEIGHT = 1024>
class GLBasicShadow
{
private:
    GLuint depthMapFBO;
    GLuint depthMap;

public:
    const GLuint SHADOW_WIDTH = T_SHADOW_WIDTH, SHADOW_HEIGHT = T_SHADOW_HEIGHT;
    GLBasicShadow()
    {
        glGenFramebuffers(1, &depthMapFBO);
        // - Create depth texture
        glGenTextures(1, &depthMap);
        glBindTexture(GL_TEXTURE_2D, depthMap);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        GLfloat borderColor[] = {1.0, 1.0, 1.0, 1.0};
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
    void caputureRenderingToTexture(GLSingleShader &shader, std::function<void(GLSingleShader &shader)> render_func)
    {
        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);
        render_func(shader);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, SRC_WIDTH, SRC_HEIGHT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
    GLuint getDepthMap()
    {
        return depthMap;
    }
    GLuint getDepthMapFBO()
    {
        return depthMapFBO;
    }
};

#endif