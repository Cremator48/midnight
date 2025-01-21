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

class Model
{
public:
	Model(const std::string& Filename, unsigned int ASSIMP_FLAGS)
	{
		LoadModel(Filename, ASSIMP_FLAGS);
	}

	~Model()
	{

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
		BONE_VEREX_BUFFER = 4,
		NUM_BUFFERS = 5
	};

	glm::mat4 m_Model = glm::mat4(1.0f);
	unsigned int m_VAO = 0;
	unsigned int m_Buffers[NUM_BUFFERS] = { 0 };

	std::string directory;

	std::vector<BasicMeshEntry> m_Meshes;
	std::vector<Texture> m_Textures;

	std::vector<glm::vec3> m_Positions;
	std::vector<glm::vec3> m_Normals;
	std::vector<glm::vec2> m_TexCoords;
	std::vector<unsigned int> m_Indices;
	std::vector<VertexBoneData> m_Bones; // Массив данных о КАЖДОЙ вершине, а конкретней о костях которые на неё влияют

	std::map<std::string, unsigned int> m_BoneNameToIndexMap;

	void Clear()
	{
		if (m_Buffers[0] != 0)
		{
			int sizeBuffersInElements = sizeof(m_Buffers) / sizeof(m_Buffers[0]);
			glDeleteBuffers(sizeBuffersInElements, m_Buffers);
		}

		if (m_VAO != 0)
		{
			glDeleteVertexArrays(1, &m_VAO);
			m_VAO = 0;
		}
	}

	void LoadModel(const std::string& Filename, unsigned int ASSIMP_FLAGS)
	{
		Clear();

		directory = Filename.substr(0, Filename.find_last_of('/'));

		glGenVertexArrays(1, &m_VAO);
		glBindVertexArray(m_VAO);

		int sizeBuffersInElements = sizeof(m_Buffers) / sizeof(m_Buffers[0]);
		glGenBuffers(sizeBuffersInElements, m_Buffers);

		Assimp::Importer Importer;

		const aiScene* pScene = Importer.ReadFile(Filename.c_str(), ASSIMP_FLAGS);

		if (pScene)
		{
			InitFromScene(pScene, Filename);
		}
		else
		{
			printf("Error loading '%s' : '%s'\n", Filename.c_str(), Importer.GetErrorString());
		}

		glBindVertexArray(0);
	}

	void InitFromScene(const aiScene* pScene, const std::string& Filename)
	{
		m_Meshes.resize(pScene->mNumMeshes);
		m_Textures.resize(pScene->mNumMaterials);

		unsigned int NumVertices = 0;
		unsigned int NumIndices = 0;

		CountVerticesAndIndices(pScene, NumVertices, NumIndices);	
		// Эта функция считает количество вершин и граней во всех мешах и сохраняет в numVertices и numIndices
		// Так же этой функцией заполняются данные в m_Meshes (количество вершин)

		ReserveSpace(NumVertices, NumIndices);

		
		InitAllMeshes(pScene);

		if (pScene->HasMaterials())
		{
			if (!InitAllMaterials(pScene, Filename)) { std::cout << "Error init all materials!\n"; }
		}
		

		// PopulateBuffers();

	}

	void CountVerticesAndIndices(const aiScene* pScene, unsigned int& NumVertices, unsigned int& NumIndices)
	{
		for (unsigned int i = 0; i < m_Meshes.size(); i++)
		{
			m_Meshes[i].MaterialIndex = pScene->mMeshes[i]->mMaterialIndex;
			m_Meshes[i].NumIndices = pScene->mMeshes[i]->mNumFaces * 3;
			m_Meshes[i].BaseVertex = NumVertices;
			m_Meshes[i].BaseIndex = NumIndices;

			NumVertices += pScene->mMeshes[i]->mNumVertices;
			NumIndices += m_Meshes[i].NumIndices;
		}
	}

	void ReserveSpace(unsigned int NumVertices, unsigned int NumIndices)
	{
		m_Positions.reserve(NumVertices);
		m_Normals.reserve(NumVertices);
		m_TexCoords.reserve(NumVertices);
		m_Indices.reserve(NumIndices);
		m_Bones.resize(NumVertices);
	}

	void InitAllMeshes(const aiScene* pScene)
	{
		for (unsigned int i = 0; i < pScene->mNumMeshes; i++)
		{
			const aiMesh* paiMesh = pScene->mMeshes[i];
			InitSingleMesh(i, paiMesh);
		}
	}

	void InitSingleMesh(unsigned int MeshIndex, const aiMesh* paiMesh)
	{
		const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);

		for (unsigned int i = 0; i < paiMesh->mNumVertices; i++)
		{
			const aiVector3D& pPos = paiMesh->mVertices[i];
			m_Positions.push_back(glm::vec3(pPos.x, pPos.y, pPos.z));

			if (paiMesh->mNormals)
			{
				const aiVector3D& pNormal = paiMesh->mNormals[i];
				m_Normals.push_back(glm::vec3(pNormal.x, pNormal.y, pNormal.z));
			}
			else
			{
				const glm::vec3 Normal(0.0f, 1.0f, 0.0f);
				m_Normals.push_back(glm::vec3(Normal.x, Normal.y, Normal.z));
			}

			const aiVector3D& pTexCoord = paiMesh->HasTextureCoords(0) ? paiMesh->mTextureCoords[0][i] : Zero3D;
			m_TexCoords.push_back(glm::vec2(pTexCoord.x, pTexCoord.y));
		}

		LoadMeshBones(MeshIndex, paiMesh);


	}


	void LoadMeshBones(unsigned int MeshIndex, const aiMesh* pMesh)
	{
		for (unsigned int i = 0; i < pMesh->mNumBones; i++)
		{
			LoadSingleBone(MeshIndex, pMesh->mBones[i]);
		}
	}

	void LoadSingleBone(unsigned int MeshIndex, const aiBone* pBone)
	{
		int BoneID = getBoneId(pBone);

		for (unsigned int i = 0; i < pBone->mNumWeights; i++)
		{
			const aiVertexWeight& vw = pBone->mWeights[i];

			unsigned int GlobalVertexID = m_Meshes[MeshIndex].BaseVertex + pBone->mWeights[i].mVertexId; 
			// Вот в этой злоебучей строчке происходит следующее
			// Глобальный индекс вершины = Количество вершин в предыдущих мешах + ID текущей вершины (текущей кости)
			// В итоге создаётся единая база всех вершин, среди всех мешей

			m_Bones[GlobalVertexID].AddBoneData(BoneID, vw.mWeight);
		}

	}


	int getBoneId(const aiBone* pBone)	// Возвращает уникальный ID кости, если его нет в базе, создёт и возвращает
	{
		int BoneIndex = 0;
		std::string BoneName(pBone->mName.C_Str());

		if (m_BoneNameToIndexMap.find(BoneName) == m_BoneNameToIndexMap.end())
		{
			BoneIndex = (int)m_BoneNameToIndexMap.size();
			m_BoneNameToIndexMap[BoneName] = BoneIndex;
		}
		else
		{
			BoneIndex = m_BoneNameToIndexMap[BoneName];
		}

		return BoneIndex;
	}


	bool InitAllMaterials(const aiScene *pScene, std::string Filename)
	{
		for (int i = 0; i < pScene->mNumMaterials; i++)
		{
			aiString path;
			m_Textures[i].id = pScene->mMaterials[i]->GetTexture(aiTextureType_DIFFUSE, 0, &path, NULL, NULL, NULL, NULL, NULL);
		}
	}

	std::vector<Texture> LoadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName)
	{
		std::vector<Texture> textures;

		for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
		{
			aiString str;
			mat->GetTexture(type, i, &str);
			Texture texture;
			texture.id = TextureFromFile(str.C_Str(), this->directory);
			texture.path = str.C_Str();
			texture.type = typeName;
			textures.push_back(texture);
		}

		return textures;
	}


	void PopulateBuffers()
	{

	}

	unsigned int TextureFromFile(const char* path, const std::string& directory)
	{
		std::string filename = std::string(path);
		filename = directory + "/" + filename;

		unsigned int textureID;
		glGenTextures(1, &textureID);

		int width, height, nrComponents;

		unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);

		if (data)
		{
			GLenum format = 0;
			if (nrComponents == 0)
			{
				format = GL_RED;
			}
			else if (nrComponents == 3)
			{
				format = GL_RGB;
			}
			else if (nrComponents == 4)
			{
				format = GL_RGBA;
			}

			glBindTexture(GL_TEXTURE_2D, textureID);
			glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			stbi_image_free(data);
		}
		else
		{
			std::cout << "Texture failed to load at path: " << path << std::endl;
			stbi_image_free(data);
		}

		return textureID;
	}

};



#endif