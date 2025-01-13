#include <iostream>
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include <assert.h>
#include <map>
#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define ASSIMP_LOAD_FLAGS (aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace | aiProcess_GenNormals | aiProcess_JoinIdenticalVertices)
#define ARRAY_SIZE_IN_ELEMENTS(a) (sizeof(a)/sizeof(a[0]))




class SkinnedMesh
{
public:
    SkinnedMesh() {};

    void LoadMesh(const std::string& filename);

private:
    #define MAX_NUM_BONES_PER_VERTEX 4

    void ReserveSpace(unsigned int NumVertices, unsigned int NumIndices);

    void InitAllMeshes(const aiScene* pScene);

    struct BasicMeshEntry {
        BasicMeshEntry()
        {
            NumIndices = 0;
            BaseVertex = 0;
            BaseIndex = 0;
        }

        unsigned int NumIndices;
        unsigned int BaseVertex;
        unsigned int BaseIndex;

    };

    void InitSingleMesh(unsigned int MeshIndex, const aiMesh* paiMesh);
    void InitAllMeshes(const aiScene* pScene);
    void LoadSingleBone(unsigned int MeshIndex, const aiBone* pBone);
    void LoadMeshBones(unsigned int MeshIndex, const aiMesh* pMesh);
    void PopulateBuffers();

    struct VertexBoneData // Структура содержащая массивы индексов костей и весов влияющих на конкретную вершину
    {
        unsigned int BoneIDs[MAX_NUM_BONES_PER_VERTEX] = { 0 }; // Массив ID костей  влияющих на вершину (максимальное количество определенно как 4)
        float Weights[MAX_NUM_BONES_PER_VERTEX] = { 0.0f };    // Массив весов костей влияющих на вершину (максимальное количество определенно как 4)

        VertexBoneData()    // дефолтный конструктор
        {

        }

        void AddBoneData(unsigned int BoneID, float Weight) // Метод добавляеят данные об кости из аргументов в члены структуры
        {
            for (int i = 0; i < ARRAY_SIZE_IN_ELEMENTS(BoneIDs); i++)
            {
                if (Weights[i] == 0.0f)
                {
                    BoneIDs[i] = BoneID;
                    Weights[i] = Weight;
                    printf("bone %d weight %f index %i\n", BoneID, Weight, i);
                    return;
                }
            }

            assert(0); // Если что то пошло не по плану
        }
    };

    enum BUFFER_TYPE    // VB - Vertex Buffer
    {
        INDEX_BUFFER = 0,
        POS_VB       = 1,
        TEXCOORD_VB  = 2,
        NORMAL_VB    = 3,
        BONE_VB      = 4,
        NUM_BUFFERS  = 5
    };

    // WorldTrans m_worldTransform;
    GLuint m_VAO = 0;
    GLuint m_Buffers[NUM_BUFFERS] = { 0 };

    std::vector<glm::vec3> m_Positions;
    std::vector<glm::vec3> m_Normals;
    std::vector<glm::vec2> m_TexCoords;
    std::vector<BasicMeshEntry> m_Meshes;
    std::vector<unsigned int> m_Indices;

    std::vector<VertexBoneData> m_Bones; // Массив структур весов и индексов костей для каждой вершины
    std::vector<unsigned int> mesh_base_vertex;
    std::map<std::string, unsigned int> bone_name_to_index_map; // Ключ-значение: имя_кости-ID_кости

    void Load_scene(const aiScene* pScene);
    void Load_meshes(const aiScene* pScene);
    void Load_mesh_bones(int mesh_index, const aiMesh* pMesh);
    void Load_single_bone(int bone_index, const aiBone* pBone);
    int Get_bone_id(const aiBone* pBone);

    void Load_scene(const aiScene* pScene)
    {
        Load_meshes(pScene);
    }

    void Load_meshes(const aiScene* pScene)
    {
        printf("*********************************************\n");
        printf("Parsing %d meshes\n\n", pScene->mNumMeshes);
        int total_vertices = 0;
        int total_indices = 0;
        int total_bones = 0;

        mesh_base_vertex.resize(pScene->mNumMeshes);

        for (int i = 0; i < pScene->mNumMeshes; i++)
        {
            const aiMesh* pMesh = pScene->mMeshes[i];
            int num_vertices = pMesh->mNumVertices;
            int num_indices = pMesh->mNumFaces * 3;
            int num_bones = pMesh->mNumBones;
            mesh_base_vertex[i] = total_vertices;

            printf(" Mesh %d '%s : vertices %d indices %d bones %d\n\n", i, pMesh->mName.C_Str(), num_vertices, num_indices, num_bones);

            total_vertices += num_vertices;
            total_indices += num_indices;
            total_bones += num_bones;

            m_Bones.resize(total_vertices);

            if (pMesh->HasBones())
            {
                Load_mesh_bones(i, pMesh);
            }


            printf("\n");
        }
    }

    void Load_mesh_bones(int mesh_index, const aiMesh* pMesh)
    {
        for (int i = 0; i < pMesh->mNumBones; i++)
        {
            Load_single_bone(mesh_index, pMesh->mBones[i]);
        }
    }

    void Load_single_bone(int mesh_index, const aiBone* pBone) // для разбора кадой кости в функцию передаётся индекс разбираемого меша и саму кость.
    {
        printf("Bone '%s': num vertices affected  by this bone: %d\n", pBone->mName.C_Str(), pBone->mNumWeights);

        int bone_id = Get_bone_id(pBone);
        printf("bone id %d\n", bone_id);

        for (int i = 0; i < pBone->mNumWeights; i++)
        {
            if (i == 0)
            {
                printf("\n");
            }

            const aiVertexWeight& vw = pBone->mWeights[i];

            unsigned int global_vertex_id = mesh_base_vertex[mesh_index] + vw.mVertexId;
            printf("Vertex id %d ", global_vertex_id);

            assert(global_vertex_id < vertex_to_bones.size());
            m_Bones[global_vertex_id].AddBoneData(bone_id, vw.mWeight);
        }

        printf("\n");
    }

    int Get_bone_id(const aiBone* pBone) // возращает id кости
    {
        int bone_id = 0;
        std::string bone_name(pBone->mName.C_Str());

        if (bone_name_to_index_map.find(bone_name) == bone_name_to_index_map.end()) // Если ключ не нашёлся в словаре, то find указывает на конец словаря
        {
            // и в этом случае 
            bone_id = bone_name_to_index_map.size(); // .size возварщает количество элементов в словаре и это значение присваивается ID новой кости
            bone_name_to_index_map[bone_name] = bone_id; // Создаём новое связку ключ-значение: имя_новой_кости-ID_новой_кости
        }
        else // если же в словаре есть такое значение - вернуть имя
        {
            bone_id = bone_name_to_index_map[bone_name];
        }

        return bone_id;
    }

    void LoadMesh(const std::string& filename)
    {
        Assimp::Importer Importer;

        const aiScene* pScene = Importer.ReadFile(filename, ASSIMP_LOAD_FLAGS);

        if (!pScene)
        {
            printf("Error parsing '%s' : '%s'\n", filename, Importer.GetErrorString());
        }

        Load_scene(pScene);

        std::cout << "That's fine!\n";
    }

};


