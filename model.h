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

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define POSITION_LOCATION    0
#define TEX_COORD_LOCATION   1
#define NORMAL_LOCATION      2
#define BONE_ID_LOCATION     3
#define BONE_WEIGHT_LOCATION 4

class Model
{
public:
	Model(const std::string& Filename, unsigned int ASSIMP_FLAGS);

	struct NeedNode
	{
		aiNode* node;
		bool needOrNot;
	};

	~Model();

	void Render(Shader shader);

	void GetBoneTransforms(float TimeInSeconds, std::vector<glm::mat4>& Transforms, int numOfAnimation);

	int maxNumOfAnimations;
	
	static void printMatrix(glm::mat4 matrix);
	static aiMatrix4x4 convertMat4(glm::mat4 glmMatrix)
	{

		aiMatrix4x4 finalMatrix;
		glmMatrix[0][0] = finalMatrix.a1;
		glmMatrix[1][0] = finalMatrix.a2;
		glmMatrix[2][0] = finalMatrix.a3;
		glmMatrix[3][0] = finalMatrix.a4;

		glmMatrix[0][1] = finalMatrix.b1;
		glmMatrix[1][1] = finalMatrix.b2;
		glmMatrix[2][1] = finalMatrix.b3;
		glmMatrix[3][1] = finalMatrix.b4;

		glmMatrix[0][2] = finalMatrix.c1;
		glmMatrix[1][2] = finalMatrix.c2;
		glmMatrix[2][2] = finalMatrix.c3;
		glmMatrix[3][2] = finalMatrix.c4;

		glmMatrix[0][3] = finalMatrix.d1;
		glmMatrix[1][3] = finalMatrix.d2;
		glmMatrix[2][3] = finalMatrix.d3;
		glmMatrix[3][3] = finalMatrix.d4;

		return finalMatrix;
	}

	
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

	static glm::mat4 convertMat4(aiMatrix3x3 aiMatrix)
	{
		
		glm::mat4 finalMatrix;
		finalMatrix[0][0] = aiMatrix.a1;
		finalMatrix[1][0] = aiMatrix.a2;
		finalMatrix[2][0] = aiMatrix.a3;
		finalMatrix[3][0] = 0.0f;

		finalMatrix[0][1] = aiMatrix.b1;
		finalMatrix[1][1] = aiMatrix.b2;
		finalMatrix[2][1] = aiMatrix.b3;
		finalMatrix[3][1] = 0.0f;

		finalMatrix[0][2] = aiMatrix.c1;
		finalMatrix[1][2] = aiMatrix.c2;
		finalMatrix[2][2] = aiMatrix.c3;
		finalMatrix[3][2] = 0.0f;

		finalMatrix[0][3] = 0.0f;
		finalMatrix[1][3] = 0.0f;
		finalMatrix[2][3] = 0.0f;
		finalMatrix[3][3] = 1.0f;
		
		
		/*
		glm::mat4 finalMatrix;

		finalMatrix[0][0] = aiMatrix.a1;
		finalMatrix[0][1] = aiMatrix.a2;
		finalMatrix[0][2] = aiMatrix.a3;
		finalMatrix[0][3] = 0.0f;

		finalMatrix[1][0] = aiMatrix.b1;
		finalMatrix[1][1] = aiMatrix.b2;
		finalMatrix[1][2] = aiMatrix.b3;
		finalMatrix[1][3] = 0.0f;

		finalMatrix[2][0] = aiMatrix.c1;
		finalMatrix[2][1] = aiMatrix.c2;
		finalMatrix[2][2] = aiMatrix.c3;
		finalMatrix[2][3] = 0.0f;

		finalMatrix[3][0] = 0.0f;
		finalMatrix[3][1] = 0.0f;
		finalMatrix[3][2] = 0.0f;
		finalMatrix[3][3] = 1.0f;
		*/


		return finalMatrix;
		
	}

	std::map<std::string, unsigned int> m_BoneNameToIndexMap;

	unsigned int NodesCounter = 0;

	struct BoneInfo
	{
		std::string name;
		glm::mat4 OffsetMatrix;
		glm::mat4 FinalTransformation;

		BoneInfo(const aiBone& pBone)
		{
			aiVector3D pScaling(1.0f);
			aiVector3D pRotation(1.0f);
			aiVector3D pPosition(1.0f);
			ai_real rotationAngle(1.0f);


			OffsetMatrix = convertMat4(pBone.mOffsetMatrix);
			FinalTransformation = glm::mat4(1.0f);
			name = pBone.mName.C_Str();
		}
	};

	std::vector<BoneInfo> m_BoneInfo;

	std::vector<NeedNode> nodeArray;

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

	

	Assimp::Importer Importer;

	const aiScene* pScene = NULL;

	std::string directory;

	std::vector<BasicMeshEntry> m_Meshes;
	std::vector<Texture> m_Textures;

	

	std::vector<glm::vec3> m_Positions;
	std::vector<glm::vec3> m_Normals;
	std::vector<glm::vec2> m_TexCoords;
	std::vector<unsigned int> m_Indices;
	std::vector<VertexBoneData> m_Bones; // Массив данных о КАЖДОЙ вершине, а конкретней о костях которые на неё влияют

	

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

	void ReadNodeHierarchy(float AnimationTimeTicks, const aiNode* pNode, const glm::mat4& ParentTransform, int numOfAnimation);

	const aiNodeAnim* FindNodeAnim(const aiAnimation* pAnimation, const std::string& NodeName);

	void CalcInterpolatedScaling(aiVector3D& Out, float AnimationTimeTicks, const aiNodeAnim* pNodeAnim);

	glm::mat4 m_GlobalInverseTransform;

	unsigned int FindScaling(float AnimationTimeTicks, const aiNodeAnim* pNodeAnim);
};



#endif