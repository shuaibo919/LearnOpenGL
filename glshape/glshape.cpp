#include "glshape.h"
#include <sstream>
#include <fstream>
// GLBasicFillObject OPENGL基本纹理对象
GLBasicShaderObject::GLBasicShaderObject(std::string glsl_file_path)
{
    // 1. 从文件路径中获取顶点/片段着色器
    std::string vertexCode;
    std::string fragmentCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;
    // 保证ifstream对象可以抛出异常：
    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try
    {
        // 打开文件
        auto vsPath = glsl_file_path + ".vs";
        auto fsPath = glsl_file_path + ".fs";
        vShaderFile.open(vsPath);
        fShaderFile.open(fsPath);
        std::stringstream vShaderStream, fShaderStream;
        // 读取文件的缓冲内容到数据流中
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();
        // 关闭文件处理器
        vShaderFile.close();
        fShaderFile.close();
        // 转换数据流到string
        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();
    }
    catch (std::ifstream::failure e)
    {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ  PATH:" << glsl_file_path << std::endl;
    }
    const char *vShaderCode = vertexCode.c_str();
    const char *fShaderCode = fragmentCode.c_str();
    // 创建顶点着色器
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vShaderCode, NULL);
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
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fShaderCode, NULL);
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
GLuint GLBasicShaderObject::getShaderProgram()
{
    return m_shaderProgram;
}
void GLBasicShaderObject::setUniform(const std::string &name, int value)
{
    glUseProgram(m_shaderProgram);
    glUniform1i(glGetUniformLocation(m_shaderProgram, name.c_str()), value);
}
void GLBasicShaderObject::setUniform(const std::string &name, float value)
{
    glUseProgram(m_shaderProgram);
    glUniform1f(glGetUniformLocation(m_shaderProgram, name.c_str()), value);
}
void GLBasicShaderObject::setUniform(const std::string &name, bool value)
{
    glUseProgram(m_shaderProgram);
    glUniform1i(glGetUniformLocation(m_shaderProgram, name.c_str()), (int)value);
}
void GLBasicShaderObject::setUniform(const std::string &name, GLBasicColor value)
{
    glUseProgram(m_shaderProgram);
    glUniform4f(glGetUniformLocation(m_shaderProgram, name.c_str()), value.r, value.g, value.b, value.a);
}
GLBasicShaderObject::~GLBasicShaderObject()
{
    glDeleteProgram(m_shaderProgram);
}

// GLBasicTriangle OPENGL基本三角形对象

GLBasicTriangle::GLBasicTriangle(const GLBsicPoint &point1, const GLBsicPoint &point2, const GLBsicPoint &point3,
                                 const GLBasicColor &fill_color, std::string glsl_file_path)
    : m_vertices{point1.x, point1.y, point1.z, point2.x, point2.y, point2.z, point3.x, point3.y, point3.z}, GLBasicShaderObject(glsl_file_path)
{
    // 设置填充颜色
    setUniform("mFragmentColor", fill_color);
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

// GLVertexTriangle OPENGL顶点三角形对象
GLVertexTriangle::GLVertexTriangle(const GLBsicPoint &point1, const GLBsicPoint &point2, const GLBsicPoint &point3,
                                   const GLBasicColor &color1,const GLBasicColor &color2, const GLBasicColor &color3, 
                                   std::string glsl_file_path)
    :m_vertices{point1.x,point1.y,point1.z,color1.r,color1.g,color1.b,
                point2.x,point2.y,point2.z,color2.r,color2.g,color2.b,
                point3.x,point3.y,point3.z,color3.r,color3.g,color3.b}, GLBasicShaderObject(glsl_file_path)
{
    // 绑定至VAO
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(m_vertices), m_vertices, GL_STATIC_DRAW);

    // 链接顶点属性
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3* sizeof(float)));
    glEnableVertexAttribArray(1);

    // 解绑VAO & VBO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
                    
}
void GLVertexTriangle::draw()
{
    glUseProgram(getShaderProgram());
    glBindVertexArray(m_VAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glBindVertexArray(0);
}
void GLVertexTriangle::deleteObject()
{
    glDeleteVertexArrays(1, &m_VAO);
    glDeleteBuffers(1, &m_VBO);
}
GLVertexTriangle::~GLVertexTriangle()
{
    deleteObject();
}


// GLBasicRectangle OPENGL基本矩形对象

GLBasicRectangle::GLBasicRectangle(const GLBsicPoint &point1, const GLBsicPoint &point2, const GLBsicPoint &point3, const GLBsicPoint &point4,
                                   const GLBasicColor &fill_color, std::string glsl_file_path)
    : m_vertices{point1.x, point1.y, point1.z, point2.x, point2.y, point2.z, point3.x, point3.y, point3.z, point4.x, point4.y, point4.z}, GLBasicShaderObject(glsl_file_path)
{
    // 设置填充颜色
    setUniform("mFragmentColor", fill_color);
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

GLTextureRectangle::GLTextureRectangle(const GLBsicPoint &point1, const GLBsicPoint &point2, const GLBsicPoint &point3, const GLBsicPoint &point4, 
                                       const GLBasicColor &color1, const GLBasicColor &color2, const GLBasicColor &color3, const GLBasicColor &color4, 
                                       std::string glsl_file_path)
:m_vertices{point1.x, point1.y, point1.z, color1.r, color1.g, color1.b, 0.0f, 1.0f, // 左上
            point2.x, point2.y, point2.z, color2.r, color2.g, color2.b, 1.0f, 1.0f, // 右上
            point3.x, point3.y, point3.z, color3.r, color3.g, color3.b, 0.0f, 0.0f, // 左下
            point4.x, point4.y, point4.z, color4.r, color4.g, color4.b, 1.0f, 0.0f}, GLBasicShaderObject(glsl_file_path)
{
    GLuint indices[] = {0, 1, 2, 1, 2, 3};
    // 绑定至VAO,VBO,EBO
    glGenVertexArrays(1,&m_VAO);
    glGenBuffers(1, &m_VBO);
    glGenBuffers(1, &m_EBO);
    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(m_vertices), m_vertices, GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    // 链接顶点属性
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3* sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    // 解绑VAO & VBO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}
void GLTextureRectangle::draw()
{
    glUseProgram(getShaderProgram());
    glBindVertexArray(m_VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}
void GLTextureRectangle::draw(std::vector<GLuint> textureIds)
{
    glUseProgram(getShaderProgram());
    for(GLuint i=0;i<textureIds.size();++i){
        glActiveTexture(GL_TEXTURE0 + i );
        glBindTexture(GL_TEXTURE_2D,textureIds[i]);
    }
    glBindVertexArray(m_VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}
void GLTextureRectangle::deleteObject()
{
    glDeleteVertexArrays(1, &m_VAO);
    glDeleteBuffers(1, &m_VBO);
    glDeleteBuffers(1,&m_EBO);
}
GLTextureRectangle::~GLTextureRectangle()
{
    deleteObject();
}
