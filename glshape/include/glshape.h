#ifndef HSB_GL_BASIC_SHAPE
#define HSB_GL_BASIC_SHAPE

#include "glad/glad.h"
#include "KHR/khrplatform.h"
#include "glfw3.h"
struct GLBsicPoint
{
    float x;
    float y;
    float z;
    GLBsicPoint(float e_x,float e_y,float e_z = 0.0f):x(e_x),y(e_y),z(e_z){}
};
struct GLBsicColor
{
    float r;
    float g;
    float b;
    float a;
    GLBsicColor(float e_r,float e_g,float e_b,float e_a=1.0f):r(e_r),g(e_g),b(e_b),a(e_a){}
};

class GLBasicTriangle{
    private:
        // vertices property
        float m_vertices[9];
        GLuint m_VAO;
        GLuint m_VBO;
        // color property
        GLuint m_shaderProgram;
        GLBsicColor m_fill_color;
        void prepareShaderProgram(const GLBsicColor &fill_color);

    public:
        GLBasicTriangle(const GLBsicPoint &point1,const GLBsicPoint &point2,const GLBsicPoint &point3, const GLBsicColor &fill_color);
        void draw(GLuint &shaderProgram);
        void deleteObject();
        ~GLBasicTriangle();
};



#endif