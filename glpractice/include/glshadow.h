#ifndef HSB_GL_BASIC_SHADOW
#define HSB_GL_BASIC_SHADOW

#include "glad/glad.h"
#include "KHR/khrplatform.h"
#include "glfw3.h"
#include "glmesh.h"
#include <iostream>
#include <vector>
#include <glm.hpp>
#include <functional>

class BasicShadowMap{

    private:
        GLuint depthMapFBO;
        GLuint depthMap;
        GLuint quadVAO;
        GLuint quadVBO;
        int SHADOW_WIDTH,SHADOW_HEIGHT;
        void bindVAOVBO();
    public:
        BasicShadowMap(int x, int y);
        GLuint getDepthMap();
        void renderLightView(GLSingleShader &shader, std::function<void(GLSingleShader &shader)> render_func);
        void draw();
};

#endif HSB_GL_BASIC_SHADOW