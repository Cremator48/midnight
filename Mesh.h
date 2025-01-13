#ifndef MESH_H
#define MESH_H

#include <glad/glad.h> // �������� ��� ���������� OpenGL-�����

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Shader_class.h" // shader.h ��������� ����� shader_s.h

#include <string>
#include <vector>
using namespace std;

struct BoneWeight
{
    int boneID;
    float boneWeight;
};

struct Vertex {

    // �������
    glm::vec3 Position;

    // �������
    glm::vec3 Normal;

    // ���������� ����������
    glm::vec2 TexCoords;

    // ����������� ������
    glm::vec3 Tangent;

    // ������ ��������� (������, ���������������� ������������ ������� � ������� �������)
    glm::vec3 Bitangent;

    /*
    // ��� � ID ����� 0
    BoneWeight weight0;

    // ��� � ID ����� 1
    BoneWeight weight1;

    // ��� � ID ����� 2
    BoneWeight weight2;

    // ��� � ID ����� 3
    BoneWeight weight3;
    */
    
};

struct Texture {
    unsigned int id;
    string type;
    string path;
};

struct VertexBoneData
{
    #define MAX_NUM_BONES_PER_VERTEX 4

    unsigned int BoneIDs[MAX_NUM_BONES_PER_VERTEX] = { 0 };
    float Weights[MAX_NUM_BONES_PER_VERTEX] = { 0.0f };

    VertexBoneData()
    {
    }

    void AddBoneData(unsigned int BoneID, float Weight)
    {
        for (unsigned int i = 0; i < sizeof(BoneIDs)/sizeof(BoneIDs); i++) {
            if (Weights[i] == 0.0) {
                BoneIDs[i] = BoneID;
                Weights[i] = Weight;
                return;
            }
        }

        // should never get here - more bones than we have space for
        assert(0);
    }
};


class Mesh {
public:
    // ������ ����
    vector<Vertex> vertices;
    vector<unsigned int> indices;
    vector<Texture> textures;
    unsigned int VAO;

    // �����������
    Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures)
    {
        this->vertices = vertices;
        this->indices = indices;
        this->textures = textures;

        // ������, ����� � ��� ���� ��� ����������� ������, ������������� ��������� ������ � ��������� ���������
        setupMesh();
    }


    // ��������� ����
    void Draw(Shader& shader)
    {
        // ��������� ��������������� ��������
        unsigned int diffuseNr = 1;
        unsigned int specularNr = 1;
        unsigned int normalNr = 1;
        unsigned int heightNr = 1;
        for (unsigned int i = 0; i < textures.size(); i++)
        {
            glActiveTexture(GL_TEXTURE0 + i); // ����� ����������� ���������� ������ ���������� ����

            // �������� ����� �������� (����� N � diffuse_textureN)
            string number;
            string name = textures[i].type;
            if (name == "texture_diffuse")
                number = std::to_string(diffuseNr++);
            else if (name == "texture_specular")
                number = std::to_string(specularNr++); // ������������ unsigned int � ������
            else if (name == "texture_normal")
                number = std::to_string(normalNr++); // ������������ unsigned int � ������
            else if (name == "texture_height")
                number = std::to_string(heightNr++); // ������������ unsigned int � ������

            // ������ ������������� ������� �� ������ ���������� ����
            glUniform1i(glGetUniformLocation(shader.ID, (name + number).c_str()), i);
            // � ��������� ��������
            glBindTexture(GL_TEXTURE_2D, textures[i].id);
        }

        // ������������ ���
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        // ��������� ������� ��������� ���������� �������� ���������� � �� �������������� ���������
        glActiveTexture(GL_TEXTURE0);
    }

private:
    // ������ ��� ���������� 
    unsigned int VBO, EBO;
    std::vector<VertexBoneData> m_Bones;

    // �������������� ��� �������� �������/�������
    void setupMesh()
    {
        // ������� �������� �������/�������
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        // ��������� ������ � ��������� �����
        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        // ����� ������������� � ���������� ��, ��� ������������ � ������ �� ���������� ���������� �������� ����������������.
        // ����� ������� ����� � ���, ��� �� ����� ������ �������� ��������� �� ���������, � ��� ��������� ������������� � ������ ������ � ���������� ���� glm::vec3 (��� glm::vec2), 
        // ������� ����� ����� ������������ � ������ ������ float, �� � � ����� � � �������� ������
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

        // ������������� ��������� ��������� ���������

        // ���������� ������
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

        // ������� ������
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));

        // ���������� ���������� ������
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

        // ����������� ������ �������
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));

        // ������ ��������� �������
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));

        glBindVertexArray(0);
    }


    

};
#endif