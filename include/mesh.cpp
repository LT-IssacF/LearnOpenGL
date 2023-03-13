#include "mesh.h"
#include <glad/glad.h> // holds all OpenGL type declarations
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <shader.h>
#include <vector>
#include <string>
using namespace std;

Mesh::Mesh(const vector<Vertex> &vertices, const vector<unsigned int> &indices, const vector<Texture> &textures) {
    this->vertices = vertices;
    this->indices = indices;
    this->textures = textures;
    // now that we have all the required data, set the vertex buffers and its attribute pointers.
    setupMesh();
}
// render the mesh
void Mesh::Draw(Shader &shader) {
    // 一个网格上可能有多张不同的纹理
    unsigned int diffuseNr  = 1;
    unsigned int specularNr = 1;
    unsigned int normalNr   = 1;
    unsigned int heightNr   = 1;
    for(unsigned int i = 0; i < textures.size(); i++) { // 遍历所有的纹理
        glActiveTexture(GL_TEXTURE0 + i); // 先激活纹理单元
        string number;
        if(textures[i].type == "texture_diffuse") {
            number = std::to_string(diffuseNr++);
        } else if(textures[i].type == "texture_specular") {
            number = std::to_string(specularNr++);
        } else if(textures[i].type == "texture_normal") {
            number = std::to_string(normalNr++);
        } else if(textures[i].type == "texture_height") {
            number = std::to_string(heightNr++);
        } shader.setInt(("material." + textures[i].type + number), i);
        // shader.setInt((textures[i].type + number), i); // 绑定纹理单元
        glBindTexture(GL_TEXTURE_2D, textures[i].id); // 绑定纹理对象
    }
    // draw mesh
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    // always good practice to set everything back to defaults once configured.
    glActiveTexture(GL_TEXTURE0);
}
void Mesh::setupMesh() { // shader类常见操作
    // create buffers/arrays
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    // load data into vertex buffers
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // A great thing about structs is that their memory layout is sequential for all its items.
    // The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
    // again translates to 3/2 floats which translates to a byte array.
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);  

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    // set the vertex attribute pointers
    // vertex Positions
    glEnableVertexAttribArray(0);	
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    // vertex normals
    glEnableVertexAttribArray(1);	
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
    // vertex texture coords
    glEnableVertexAttribArray(2);	
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
    // vertex tangent
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
    // vertex bitangent
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));
    // ids
    glEnableVertexAttribArray(5);
    glVertexAttribIPointer(5, 4, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, m_BoneIDs));

    // weights
    glEnableVertexAttribArray(6);
    glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_Weights));
    glBindVertexArray(0);
}