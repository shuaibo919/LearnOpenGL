#include "glshape.h"
#include <iostream>
#include <sstream>

GLBasicFillObject::GLBasicFillObject(const GLBsicColor &fill_color) : m_fill_color(fill_color)
{
    // 配置颜色
    prepareShaderProgram(m_fill_color);
}

void GLBasicFillObject::prepareShaderProgram(const GLBsicColor &fill_color)
{
    // 创建顶点着色器
    const char *vertexShaderSource = "#version 330 core\n"
                                     "layout (location = 0) in vec3 aPos;\n"
                                     "void main()\n"
                                     "{\n"
                                     "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
                                     "}\0";
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
                  << infoLog << std::endl;
    }

    // 创建片段着色器
    std::stringstream fmt;
    fmt << "#version 330 core\nout vec4 FragColor;\nvoid main(){\n   FragColor = vec4("
        << m_fill_color.r << "," << m_fill_color.g << "," << m_fill_color.b << "," << m_fill_color.a
        << ");\n}";
    std::string fmtString = fmt.str();
    const char *fragmentSahderSource = fmtString.c_str();
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentSahderSource, NULL);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
                  << infoLog << std::endl;
    }
    // 创建Shader程序
    m_shaderProgram = glCreateProgram();
    glAttachShader(m_shaderProgram, vertexShader);
    glAttachShader(m_shaderProgram, fragmentShader);
    glLinkProgram(m_shaderProgram);
    glUseProgram(m_shaderProgram);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

GLuint GLBasicFillObject::getShaderProgram()
{
    return m_shaderProgram;
}

GLBasicFillObject::~GLBasicFillObject()
{
    glDeleteProgram(m_shaderProgram);
}

GLBasicTriangle::GLBasicTriangle(const GLBsicPoint &point1, const GLBsicPoint &point2, const GLBsicPoint &point3, const GLBsicColor &fill_color)
    : m_vertices{point1.x, point1.y, point1.z, point2.x, point2.y, point2.z, point3.x, point3.y, point3.z}, GLBasicFillObject(fill_color)
{
    // 绑定至VAO
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(m_vertices), m_vertices, GL_STATIC_DRAW);

    // 链接顶点属性
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    // 解绑VAO & VBO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}
void GLBasicTriangle::draw()
{
    glUseProgram(getShaderProgram());
    glBindVertexArray(m_VAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glBindVertexArray(0);
}
void GLBasicTriangle::deleteObject()
{
    glDeleteVertexArrays(1, &m_VAO);
    glDeleteBuffers(1, &m_VBO);
}
GLBasicTriangle::~GLBasicTriangle()
{
    deleteObject();
}

GLBasicRectangle::GLBasicRectangle(const GLBsicPoint &point1, const GLBsicPoint &point2, const GLBsicPoint &point3, const GLBsicPoint &point4, const GLBsicColor &fill_color)
    : m_vertices{point1.x, point1.y, point1.z, point2.x, point2.y, point2.z, point3.x, point3.y, point3.z, point4.x, point4.y, point4.z}, GLBasicFillObject(fill_color)
{
    GLuint indices[] = {0, 1, 2, 1, 2, 3};
    // 绑定至VAO
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glGenBuffers(1, &m_EBO);
    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(m_vertices), m_vertices, GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    // 链接顶点属性
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    // 解绑VAO & VBO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}
void GLBasicRectangle::draw()
{
    glUseProgram(getShaderProgram());
    glBindVertexArray(m_VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}
void GLBasicRectangle::deleteObject()
{
    glDeleteVertexArrays(1, &m_VAO);
    glDeleteBuffers(1, &m_VBO);
    glDeleteBuffers(1, &m_EBO);
}
GLBasicRectangle::~GLBasicRectangle()
{
    deleteObject();
}
