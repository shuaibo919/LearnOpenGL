#ifndef HSB_GL_BASIC_VERTICES
#define HSB_GL_BASIC_VERTICES

#include "glad/glad.h"
#include "KHR/khrplatform.h"
#include "glfw3.h"
#include "glmesh.h"
#include <iostream>
#include <vector>
#include <glm.hpp>

// (PTN) Postion Normal Texcoords
struct GLBasicPTNVertex
{
    glm::vec3 Position;     // 顶点位置
    glm::vec3 Normal;       // 顶点法线
    glm::vec2 TexCoords;    // 纹理坐标
};
// (PC) Postion Color 
struct GLBasicPCVertex
{
    glm::vec3 Position;     // 顶点位置
    glm::vec3 Color;       // 颜色
};
// (PCT) Postion Color Texcoords
struct GLBasicPCTVertex
{
    glm::vec3 Position;     // 顶点位置
    glm::vec3 Color;        // 颜色
    glm::vec2 TexCoords;    // 纹理坐标
};
// (PT) Postion Texcoords
struct GLBasicPTVertex
{
    glm::vec3 Position;     // 顶点位置
    glm::vec2 TexCoords;    // 纹理坐标
};

template<typename T>
class GLBasicVerticesObj{
    public:
        /*  网格数据  */
        std::vector<T> m_vertices;
        std::vector<GLuint> m_indices;
        std::vector<GLBasicTexture> m_textures;
        /*  构造函数  */
        GLBasicVerticesObj(std::vector<T> vertices, std::vector<GLuint> indices, std::vector<GLBasicTexture> textures)
            :m_vertices(vertices),m_indices(indices),m_textures(textures)
        {
            setupVerticesObj();
        }
        GLBasicVerticesObj(std::vector<T> vertices, std::vector<GLuint> indices)
            :m_vertices(vertices),m_indices(indices)
        {
            setupVerticesObj();
        }
        GLBasicVerticesObj(std::vector<T> vertices)
            :m_vertices(vertices)
        {
            setupVerticesObj();
        }
        /*  渲染  */
        void draw(GLSingleShader &shader)
        {
            unsigned int diffuseNr = 1;
            unsigned int specularNr = 1;
            unsigned int normalNr = 1;
            unsigned int heightNr = 1;
            for (int i = 0; i < m_textures.size(); i++)
            {
                glActiveTexture(GL_TEXTURE0 + i); // 在绑定之前激活相应的纹理单元
                // 获取纹理序号（diffuse_textureN 中的 N）
                std::string number;
                std::string name = m_textures[i].type;
                if (name == "texture_diffuse")
                    number = std::to_string(diffuseNr++);
                else if (name == "texture_specular")
                    number = std::to_string(specularNr++);
                else if (name == "texture_normal")
                    number = std::to_string(normalNr++);
                else if (name == "texture_height")
                    number = std::to_string(heightNr++);

                shader.setUniform("material." + name + number, i);
                glBindTexture(GL_TEXTURE_2D, m_textures[i].id);
            }
            glActiveTexture(GL_TEXTURE0);

            // 绘制网格
            glBindVertexArray(VAO);
            if(!m_indices.empty()) glDrawElements(GL_TRIANGLES, static_cast<GLuint>(m_indices.size()), GL_UNSIGNED_INT, 0);
            // todo:
            else glDrawArrays(GL_TRIANGLES, 0, );
            glBindVertexArray(0);
        }
    private:
        /*  渲染数据  */
        unsigned int VAO, VBO, EBO;
        /*  初始化设置  */
        void setupVerticesObj()
        {
            // 设置顶点缓存和元素索引缓存
            glGenVertexArrays(1, &VAO);
            glGenBuffers(1, &VBO);
            glBindVertexArray(VAO);
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(GLBasicVertex), &m_vertices[0], GL_STATIC_DRAW);
            if(!m_indices.empty()){
                glGenBuffers(1, &EBO);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(unsigned int), &m_indices[0], GL_STATIC_DRAW);
            }
            // 顶点位置
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(decltype(T)), (void *)0);
            // (PTN) Postion Normal Texcoords
            if(decltype(T)==decltype(GLBasicPTNVertex))
            {
                // 顶点法线
                glEnableVertexAttribArray(1);
                glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(decltype(T)), (void *)offsetof(decltype(T), Normal));
                // 顶点纹理坐标
                glEnableVertexAttribArray(2);
                glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(decltype(T)), (void *)offsetof(decltype(T), TexCoords));
            }
            // (PC) Postion Color
            else if(decltype(T)==decltype(GLBasicPCVertex))
            {
                // 颜色
                glEnableVertexAttribArray(1);
                glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(decltype(T)), (void *)offsetof(decltype(T), Color));
            }
            // (PCT) Postion Color Texcoords
            else if(decltype(T)==decltype(GLBasicPCTVertex))
            {
                // 颜色
                glEnableVertexAttribArray(1);
                glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(decltype(T)), (void *)offsetof(decltype(T), Color));
                // 顶点纹理坐标
                glEnableVertexAttribArray(2);
                glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(decltype(T)), (void *)offsetof(decltype(T), TexCoords));
            }
            // (PT) Postion Texcoords
            else if(decltype(T)==decltype(GLBasicPTVertex))
            {
                // 顶点纹理坐标
                glEnableVertexAttribArray(1);
                glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(decltype(T)), (void *)offsetof(decltype(T), TexCoords));
            }
            else{
                std::cout << "ERROR! TYPE OF VERTEX NOT SUPPORT! \n"; 
            }
            glBindVertexArray(0);
        }
};



#endif