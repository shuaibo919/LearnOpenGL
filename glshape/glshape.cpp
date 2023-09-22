#include "glshape.h"
#include <iostream>

GLBasicTriangle::GLBasicTriangle(const GLBsicPoint &point1,const GLBsicPoint &point2,const GLBsicPoint &point3, const GLBsicColor &fill_color)
    :m_vertices{point1.x,point1.y,point1.z,point2.x,point2.y,point2.z,point3.x,point3.y,point3.z},m_fill_color(fill_color)
{
    // 绑定至VAO
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER,m_VBO);
    glBufferData(GL_ARRAY_BUFFER,sizeof(m_vertices),m_vertices,GL_STATIC_DRAW);

    // 链接顶点属性
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),(void*)0);
    glEnableVertexAttribArray(0);

    // 解绑VAO & VBO
    glBindBuffer(GL_ARRAY_BUFFER,0);
    glBindVertexArray(0);

    // 配置颜色
    prepareShaderProgram(m_fill_color);
}
void GLBasicTriangle::draw(GLuint &shaderProgram)
{
    glUseProgram(shaderProgram);
    glBindVertexArray(m_VAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glBindVertexArray(0);
}
void GLBasicTriangle::deleteObject()
{
    glDeleteVertexArrays(1, &m_VAO);
    glDeleteBuffers(1, &m_VBO);
}
void GLBasicTriangle::prepareShaderProgram(const GLBsicColor &fill_color)
{
    // 创建顶点着色器
    const char *vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "}\0";
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader,1,&vertexShaderSource,NULL);
    glCompileShader(vertexShader);
    int  success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // 创建片段着色器
    const char *fragmentSahderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "    FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
    "}\0";
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader,1,&fragmentSahderSource,NULL);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader,GL_COMPILE_STATUS,&success);
    if(!success)
    {
        glGetShaderInfoLog(fragmentShader,512,NULL,infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // 创建Shader程序
    m_shaderProgram = glCreateProgram();
    glAttachShader(m_shaderProgram,vertexShader);
    glAttachShader(m_shaderProgram,fragmentShader);
    glLinkProgram(m_shaderProgram);
    glUseProgram(m_shaderProgram);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}
GLBasicTriangle::~GLBasicTriangle()
{
    deleteObject();
}