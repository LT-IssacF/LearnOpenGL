#ifndef MESH_H
#define MESH_H
#include <glad/glad.h> // holds all OpenGL type declarations
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <shader.h>
#include <vector>
#include <string>
#define MAX_BONE_INFLUENCE 4
using namespace std;

struct Vertex { // 网格有一系列顶点，一个顶点包括位置、法向量和纹理坐标
    // position
    glm::vec3 Position;
    // normal
    glm::vec3 Normal;
    // texCoords
    glm::vec2 TexCoords;
    // tangent
    glm::vec3 Tangent;
    // bitangent
    glm::vec3 Bitangent;
	//bone indexes which will influence this vertex
	int m_BoneIDs[MAX_BONE_INFLUENCE];
	//weights from each bone
	float m_Weights[MAX_BONE_INFLUENCE];
};
struct Texture { // 整合纹理数据
    unsigned int id;
    string type; // 纹理类型
    string path;
};
class Mesh { // 网格类
public:
    // mesh Data
    vector<Vertex>       vertices;
    vector<unsigned int> indices;
    vector<Texture>      textures;
    unsigned int VAO;
    // constructor
    Mesh(const vector<Vertex> &vertices, const vector<unsigned int> &indices, const vector<Texture> &textures);
    // render the mesh
    void Draw(Shader &shader);
private:
    // render data 
    unsigned int VBO, EBO;
    // initializes all the buffer objects/arrays
    void setupMesh();
};
#endif