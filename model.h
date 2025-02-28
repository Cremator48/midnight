#ifndef MODEL_H
#define MODEL_H

#include <glad/glad.h> 

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "stb_image.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "mesh.h"
#include "Shader_class.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>

#define POSITION_LOCATION    0
#define TEX_COORD_LOCATION   1
#define NORMAL_LOCATION      2
#define BONE_ID_LOCATION     3
#define BONE_WEIGHT_LOCATION 4

class Model
{
public:
	Model(const std::string& Filename, unsigned int ASSIMP_FLAGS);

	~Model();

	void Render(Shader shader);

	void GetBoneTransforms(std::vector<glm::mat4>& Transforms);

	

	static glm::mat4 convertMat4(aiMatrix4x4 aiMatrix)
	{
		glm::mat4 finalMatrix;

		finalMatrix[0][0] = aiMatrix.a1;
		finalMatrix[1][0] = aiMatrix.a2;
		finalMatrix[2][0] = aiMatrix.a3;
		finalMatrix[3][0] = aiMatrix.a4;

		finalMatrix[0][1] = aiMatrix.b1;
		finalMatrix[1][1] = aiMatrix.b2;
		finalMatrix[2][1] = aiMatrix.b3;
		finalMatrix[3][1] = aiMatrix.b4;

		finalMatrix[0][2] = aiMatrix.c1;
		finalMatrix[1][2] = aiMatrix.c2;
		finalMatrix[2][2] = aiMatrix.c3;
		finalMatrix[3][2] = aiMatrix.c4;

		finalMatrix[0][3] = aiMatrix.d1;
		finalMatrix[1][3] = aiMatrix.d2;
		finalMatrix[2][3] = aiMatrix.d3;
		finalMatrix[3][3] = aiMatrix.d4;

		return finalMatrix;
	}



private:

#define MAX_NUM_BONES_PER_VERTEX 4


	struct VertexBoneData
	{
		unsigned int BoneIDs[MAX_NUM_BONES_PER_VERTEX] = { 0 };
		float Weights[MAX_NUM_BONES_PER_VERTEX] = { 0.0f };

		VertexBoneData()
		{

		}

		void AddBoneData(unsigned int BoneID, float Weight)
		{
			unsigned int arraySize = sizeof(BoneIDs) / sizeof(BoneIDs[0]);

			for (unsigned int i = 0; i < arraySize; i++)
			{
				if (Weights[i] == 0.0f)
				{
					BoneIDs[i] = BoneID;
					Weights[i] = Weight;

					return;
				}
			}
			assert(0);
		}
	};


	struct BasicMeshEntry
	{
		BasicMeshEntry()
		{
			NumIndices = 0;
			BaseVertex = 0;
			BaseIndex = 0;
			MaterialIndex = 0;
		}

		unsigned int NumIndices;
		unsigned int BaseVertex;
		unsigned int BaseIndex;
		unsigned int MaterialIndex;
	};

	struct Texture {

		Texture()
		{
			id = 0;
			type = std::string("diffuse");
			path = std::string("./");
		}

		unsigned int id;
		std::string type;
		std::string path; // мы сохраняем путь к текстуре, чтобы сравнивать с другими текстурами
	};

	enum BufferType
	{
		INDEX_BUFFER = 0,
		POSITION_VERTEX_BUFFER = 1,
		TEXCOORD_VEREX_BUFFER = 2,
		NORMAL_VEREX_BUFFER = 3,
		BONE_VERTEX_BUFFER = 4,
		NUM_BUFFERS = 5
	};

	struct BoneInfo
	{
		glm::mat4 OffsetMatrix;
		glm::mat4 FinalTransformation;

		BoneInfo(const glm::mat4& Offset)
		{
			OffsetMatrix = Offset;
			FinalTransformation = glm::mat4(0.0f);
		}
		
		BoneInfo(const aiMatrix4x4& Offset)
		{
			OffsetMatrix = convertMat4(Offset);
			FinalTransformation = glm::mat4(0.0f);
		}		
	};

	Assimp::Importer Importer;

	const aiScene* pScene = NULL;

	std::string directory;

	std::vector<BasicMeshEntry> m_Meshes;
	std::vector<Texture> m_Textures;
	std::vector<BoneInfo> m_BoneInfo;

	std::vector<glm::vec3> m_Positions;
	std::vector<glm::vec3> m_Normals;
	std::vector<glm::vec2> m_TexCoords;
	std::vector<unsigned int> m_Indices;
	std::vector<VertexBoneData> m_Bones; // Массив данных о КАЖДОЙ вершине, а конкретней о костях которые на неё влияют

	std::map<std::string, unsigned int> m_BoneNameToIndexMap;

	glm::mat4 m_Model = glm::mat4(1.0f);

	

	unsigned int m_VAO = 0;
	unsigned int m_Buffers[NUM_BUFFERS] = { 0 };

	void Clear();

	void LoadModel(const std::string& Filename, unsigned int ASSIMP_FLAGS);

	void InitFromScene(const aiScene* pScene, const std::string& Filename);

	void CountVerticesAndIndices(const aiScene* pScene, unsigned int& NumVertices, unsigned int& NumIndices);

	void ReserveSpace(unsigned int NumVertices, unsigned int NumIndices);

	void InitAllMeshes(const aiScene* pScene);

	void InitSingleMesh(unsigned int MeshIndex, const aiMesh* paiMesh);

	void LoadMeshBones(unsigned int MeshIndex, const aiMesh* pMesh);

	void LoadSingleBone(unsigned int MeshIndex, const aiBone* pBone);


	int getBoneId(const aiBone* pBone);	// Возвращает уникальный ID кости, если его нет в базе, создёт и возвращает


	bool InitAllMaterials(const aiScene* pScene, std::string Filename);

	std::vector<Texture> LoadMaterialTextures(aiMaterial* material, aiTextureType type, std::string typeName);
	

	void PopulateBuffers();

	std::string fixPathSlash(std::string string);	// Заменяет все слеши с / на \

	unsigned int TextureFromFile(const char* path, const std::string& directory);

	void ReadNodeHierarchy(const aiNode* pNode, const glm::mat4& ParentTransform);
};



#endif