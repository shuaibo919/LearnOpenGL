#include "glmesh.h"
#include <fstream>
#include <sstream>
#include <gtc/type_ptr.hpp>

// GLSingleShader OPENGL基本着色器对象
GLSingleShader::GLSingleShader(std::string glsl_file_path)
{
    // 创建Shader程序
    m_shaderProgram = glCreateProgram();
    // 加载GLSL
    attachGLSL(glsl_file_path + ".vs",GL_VERTEX_SHADER);
    attachGLSL(glsl_file_path + ".fs",GL_FRAGMENT_SHADER);
    glUseProgram(m_shaderProgram);
}
void GLSingleShader::attachGLSL(std::string glsl_file_path,GLenum type)
{
        // 1. 从文件路径中获取顶点/片段着色器
    std::string glslCode;
    std::ifstream shaderFile;
    // 保证ifstream对象可以抛出异常：
    shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try
    {
        // 打开文件
        auto glslPath = glsl_file_path ;
        shaderFile.open(glslPath);
        std::stringstream shaderStream;
        // 读取文件的缓冲内容到数据流中
        shaderStream << shaderFile.rdbuf();
        // 关闭文件处理器
        shaderFile.close();
        // 转换数据流到string
        glslCode = shaderStream.str();
    }
    catch (std::ifstream::failure e)
    {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ  PATH:" << glsl_file_path << std::endl;
    }
    const char *shaderCode = glslCode.c_str();
    // 创建顶点着色器
    GLuint tmpShader = glCreateShader(type);
    glShaderSource(tmpShader, 1, &shaderCode, NULL);
    glCompileShader(tmpShader);
    int success;
    char infoLog[512];
    glGetShaderiv(tmpShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(tmpShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
                  << infoLog << std::endl;
    }
    glAttachShader(m_shaderProgram, tmpShader);
    glLinkProgram(m_shaderProgram);
    glDeleteShader(tmpShader);
}
GLuint GLSingleShader::getShaderProgram()
{
    return m_shaderProgram;
}
void GLSingleShader::setUniform(const std::string &name, int value)
{
    glUseProgram(m_shaderProgram);
    glUniform1i(glGetUniformLocation(m_shaderProgram, name.c_str()), value);
}
void GLSingleShader::setUniform(const std::string &name, float value)
{
    glUseProgram(m_shaderProgram);
    glUniform1f(glGetUniformLocation(m_shaderProgram, name.c_str()), value);
}
void GLSingleShader::setUniform(const std::string &name, bool value)
{
    glUseProgram(m_shaderProgram);
    glUniform1i(glGetUniformLocation(m_shaderProgram, name.c_str()), (int)value);
}
void GLSingleShader::setUniform(const std::string &name, glm::mat4 mat4)
{
    glUseProgram(m_shaderProgram);
    glUniformMatrix4fv(glGetUniformLocation(m_shaderProgram, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat4));
}
void GLSingleShader::setUniform(const std::string &name, glm::vec3 vec3)
{
    glUseProgram(m_shaderProgram);
    glUniform3fv(glGetUniformLocation(m_shaderProgram, name.c_str()), 1, glm::value_ptr(vec3));
}
GLSingleShader::~GLSingleShader()
{
    glDeleteProgram(m_shaderProgram);
}

// GLBaiscMesh OPENGL基本网格对象
GLBasicMesh::GLBasicMesh(std::vector<GLBasicVertex> vertices, std::vector<GLuint> indices, std::vector<GLBasicTexture> textures)
    :m_vertices(vertices),m_indices(indices),m_textures(textures)
{
    setupMesh();
}
void GLBasicMesh::setupMesh()
{
    // 设置顶点缓存和元素索引缓存
    glGenVertexArrays(1,&VAO);
    glGenBuffers(1,&VBO);
    glGenBuffers(1,&EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(GLBasicVertex), &m_vertices[0], GL_STATIC_DRAW);  
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(unsigned int), &m_indices[0], GL_STATIC_DRAW);

    // 顶点位置
    glEnableVertexAttribArray(0);   
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLBasicVertex), (void*)0);
    // 顶点法线
    glEnableVertexAttribArray(1);   
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GLBasicVertex), (void*)offsetof(GLBasicVertex, Normal));
    // 顶点纹理坐标
    glEnableVertexAttribArray(2);   
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(GLBasicVertex), (void*)offsetof(GLBasicVertex, TexCoords));

    glBindVertexArray(0);

}
void GLBasicMesh::draw(GLuint shaderProgram)
{

}