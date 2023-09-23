#ifndef HSB_GL_BASIC_SHAPE
#define HSB_GL_BASIC_SHAPE

#include "glad/glad.h"
#include "KHR/khrplatform.h"
#include "glfw3.h"
#include <iostream>
struct GLBsicPoint
{
    float x;
    float y;
    float z;
    GLBsicPoint(float e_x, float e_y, float e_z = 0.0f) : x(e_x), y(e_y), z(e_z) {}
};
struct GLBasicColor
{
    float r;
    float g;
    float b;
    float a;
    GLBasicColor(float e_r, float e_g, float e_b, float e_a = 1.0f) : r(e_r), g(e_g), b(e_b), a(e_a) {}
};

class GLBasicShaderObject
{
private:
    const char *GLSL;
    GLuint m_shaderProgram;

public:
    GLBasicShaderObject(std::string glsl_file_path);
    GLuint getShaderProgram();
    void setUniform(const std::string &name, int value);
    void setUniform(const std::string &name, float value);
    void setUniform(const std::string &name, bool value);
    void setUniform(const std::string &name, GLBasicColor value);
    ~GLBasicShaderObject();
};

class GLBasicTriangle : public GLBasicShaderObject
{
private:
    // vertices property
    float m_vertices[9];
    GLuint m_VAO;
    GLuint m_VBO;

public:
    GLBasicTriangle(const GLBsicPoint &point1, const GLBsicPoint &point2, const GLBsicPoint &point3,
                    const GLBasicColor &fill_color, std::string glsl_file_path = "resource/shader/fillobj");
    void draw();
    void deleteObject();
    ~GLBasicTriangle();
};

class GLBasicRectangle : public GLBasicShaderObject
{
private:
    // vertices property
    float m_vertices[12];
    GLuint m_VAO;
    GLuint m_VBO;
    GLuint m_EBO;

public:
    GLBasicRectangle(const GLBsicPoint &point1, const GLBsicPoint &point2, const GLBsicPoint &point3, const GLBsicPoint &point4,
                     const GLBasicColor &fill_color, std::string glsl_file_path = "resource/shader/fillobj");
    void draw();
    void deleteObject();
    ~GLBasicRectangle();
};

#endif