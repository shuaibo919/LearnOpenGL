#ifndef HSB_GL_BASIC_MESH
#define HSB_GL_BASIC_MESH

#include "glad/glad.h"
#include "KHR/khrplatform.h"
#include "glfw3.h"
#include <iostream>
#include <vector>
#include <glm.hpp>

struct GLBasicVertex
{
    glm::vec3 Position;     // 顶点位置
    glm::vec3 Normal;       // 顶点法线
    glm::vec2 TexCoords;    // 纹理坐标
};
struct GLBasicTexture
{
    GLuint id;              // 纹理id
    std::string type;       // 纹理类型
};

class GLSingleShader
{
private:
    const char *GLSL;
    GLuint m_shaderProgram;
    void attachGLSL(std::string glsl_file_path,GLenum type);

public:
    GLSingleShader(std::string glsl_file_path);
    GLuint getShaderProgram();
    void setUniform(const std::string &name, int value);
    void setUniform(const std::string &name, float value);
    void setUniform(const std::string &name, bool value);
    void setUniform(const std::string &name, glm::mat4 mat4);
    void setUniform(const std::string &name, glm::vec3 vec3);
    ~GLSingleShader();
};

class GLBasicMesh{
    public:
        /*  网格数据  */
        std::vector<GLBasicVertex> m_vertices;
        std::vector<unsigned int> m_indices;
        std::vector<GLBasicTexture> m_textures;
        /*  构造函数  */
        GLBasicMesh(std::vector<GLBasicVertex> vertices, std::vector<GLuint> indices, std::vector<GLBasicTexture> textures);
        /*  渲染  */
        void draw(GLuint shaderProgram);
    private:
        /*  渲染数据  */
        unsigned int VAO, VBO, EBO;
        /*  初始化设置  */
        void setupMesh();
};




#endif