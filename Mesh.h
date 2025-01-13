#ifndef MESH_H
#define MESH_H

#include <glad/glad.h> // содержит все объявления OpenGL-типов

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Shader_class.h" // shader.h идентичен файлу shader_s.h

#include <string>
#include <vector>
using namespace std;

struct BoneWeight
{
    int boneID;
    float boneWeight;
};

struct Vertex {

    // Позиция
    glm::vec3 Position;

    // Нормаль
    glm::vec3 Normal;

    // Текстурные координаты
    glm::vec2 TexCoords;

    // Касательный вектор
    glm::vec3 Tangent;

    // Вектор бинормали (вектор, перпендикулярный касательному вектору и вектору нормали)
    glm::vec3 Bitangent;

    /*
    // Вес и ID кости 0
    BoneWeight weight0;

    // Вес и ID кости 1
    BoneWeight weight1;

    // Вес и ID кости 2
    BoneWeight weight2;

    // Вес и ID кости 3
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
    // Данные меша
    vector<Vertex> vertices;
    vector<unsigned int> indices;
    vector<Texture> textures;
    unsigned int VAO;

    // Конструктор
    Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures)
    {
        this->vertices = vertices;
        this->indices = indices;
        this->textures = textures;

        // Теперь, когда у нас есть все необходимые данные, устанавливаем вершинные буферы и указатели атрибутов
        setupMesh();
    }


    // Рендеринг меша
    void Draw(Shader& shader)
    {
        // Связываем соответствующие текстуры
        unsigned int diffuseNr = 1;
        unsigned int specularNr = 1;
        unsigned int normalNr = 1;
        unsigned int heightNr = 1;
        for (unsigned int i = 0; i < textures.size(); i++)
        {
            glActiveTexture(GL_TEXTURE0 + i); // перед связыванием активируем нужный текстурный юнит

            // Получаем номер текстуры (номер N в diffuse_textureN)
            string number;
            string name = textures[i].type;
            if (name == "texture_diffuse")
                number = std::to_string(diffuseNr++);
            else if (name == "texture_specular")
                number = std::to_string(specularNr++); // конвертируем unsigned int в строку
            else if (name == "texture_normal")
                number = std::to_string(normalNr++); // конвертируем unsigned int в строку
            else if (name == "texture_height")
                number = std::to_string(heightNr++); // конвертируем unsigned int в строку

            // Теперь устанавливаем сэмплер на нужный текстурный юнит
            glUniform1i(glGetUniformLocation(shader.ID, (name + number).c_str()), i);
            // и связываем текстуру
            glBindTexture(GL_TEXTURE_2D, textures[i].id);
        }

        // Отрисовываем меш
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        // Считается хорошей практикой возвращать значения переменных к их первоначальным значениям
        glActiveTexture(GL_TEXTURE0);
    }

private:
    // Данные для рендеринга 
    unsigned int VBO, EBO;
    std::vector<VertexBoneData> m_Bones;

    // Инициализируем все буферные объекты/массивы
    void setupMesh()
    {
        // Создаем буферные объекты/массивы
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        // Загружаем данные в вершинный буфер
        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        // Самое замечательное в структурах то, что расположение в памяти их внутренних переменных является последовательным.
        // Смысл данного трюка в том, что мы можем просто передать указатель на структуру, и она прекрасно преобразуется в массив данных с элементами типа glm::vec3 (или glm::vec2), 
        // который затем будет преобразован в массив данных float, ну а в конце – в байтовый массив
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

        // Устанавливаем указатели вершинных атрибутов

        // Координаты вершин
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

        // Нормали вершин
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));

        // Текстурные координаты вершин
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

        // Касательный вектор вершины
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));

        // Вектор бинормали вершины
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));

        glBindVertexArray(0);
    }


    

};
#endif