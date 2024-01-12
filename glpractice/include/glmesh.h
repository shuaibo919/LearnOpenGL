#ifndef HSB_GL_BASIC_MESH
#define HSB_GL_BASIC_MESH

#include "glad/glad.h"
#include "KHR/khrplatform.h"
#include "glfw3.h"
#include <iostream>
#include <vector>
#include <glm.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

const int MAX_BONE_INFLUENCE = 4;

struct GLBasicVertex
{
    glm::vec3 Position;                  // 顶点位置
    glm::vec3 Normal;                    // 顶点法线
    glm::vec2 TexCoords;                 // 纹理坐标
    glm::vec3 Tangent;                   // tangent
    glm::vec3 Bitangent;                 // bitangent
    int m_BoneIDs[MAX_BONE_INFLUENCE];   // bone indexes which will influence this vertex
    float m_Weights[MAX_BONE_INFLUENCE]; // weights from each bone
};
struct GLBasicTexture
{
    GLuint id;        // 纹理id
    std::string type; // 纹理类型
    std::string path; // 纹理路径
};

class GLSingleShader
{
private:
    std::string GLSL;
    GLuint m_shaderProgram;
    void attachGLSL(std::string glsl_file_path, GLenum type);

public:
    GLSingleShader(std::string glsl_file_path, bool load_geometry = false);
    GLuint getShaderProgram();
    void setUniform(const std::string &name, int value);
    void setUniform(const std::string &name, float value);
    void setUniform(const std::string &name, bool value);
    void setUniform(const std::string &name, glm::mat4 mat4);
    void setUniform(const std::string &name, glm::mat3 mat3);
    void setUniform(const std::string &name, glm::vec3 vec3);
    void use()
    {
        glUseProgram(m_shaderProgram);
    }
    ~GLSingleShader();
};

class GLBasicMesh
{
public:
    /*  网格数据  */
    std::vector<GLBasicVertex> m_vertices;
    std::vector<unsigned int> m_indices;
    std::vector<GLBasicTexture> m_textures;
    /*  构造函数  */
    GLBasicMesh(std::vector<GLBasicVertex> vertices, std::vector<GLuint> indices, std::vector<GLBasicTexture> textures);
    /*  渲染  */
    void draw(GLSingleShader &shader);
    void draw(GLSingleShader &shader, int amount);
    /*  属性 */
    unsigned int getVAO() { return VAO; };

private:
    /*  渲染数据  */
    unsigned int VAO, VBO, EBO;
    /*  初始化设置  */
    void setupMesh();
};

class GLBasicModel
{
private:
    /*  模型数据  */
    std::vector<GLBasicTexture> textures_loaded;
    std::string directory;
    /*  函数   */
    void loadGLBasicModel(const std::string &path);
    void processNode(aiNode *node, const aiScene *scene);
    void processNodeWhile(aiNode *node, const aiScene *scene);
    GLBasicMesh processMesh(aiMesh *mesh, const aiScene *scene);
    std::vector<GLBasicTexture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, const std::string &typeName);

public:
    /*  构造函数   */
    std::vector<GLBasicMesh> meshes;
    GLBasicModel(const std::string &path)
    {
        loadGLBasicModel(path);
    }
    void draw(GLSingleShader &shader)
    {
        for (unsigned int i = 0; i < meshes.size(); i++)
            meshes[i].draw(shader);
    }
    void draw(GLSingleShader &shader, int amount)
    {
        for (unsigned int i = 0; i < meshes.size(); i++)
            meshes[i].draw(shader, amount);
    }
};

#endif