#include "model.h"

Model::Model(const std::string& Filename, unsigned int ASSIMP_FLAGS)
{
	LoadModel(Filename, ASSIMP_FLAGS);
}

Model::~Model()
{

}

void Model::Render(Shader shader)
{
	glBindVertexArray(m_VAO);

	

	for (int i = 0; i < m_Meshes.size(); i++)
	{
		unsigned int MaterialIndex = m_Meshes[i].MaterialIndex;

		assert(MaterialIndex < m_Textures.size());

		if (m_Textures[0].id)
		{
			glActiveTexture(GL_TEXTURE0);
			shader.setFloat("diffuse_texture", 0);

			glBindVertexArray(m_VAO);
			glBindTexture(GL_TEXTURE_2D, m_Textures[0].id);
			glDrawElements(GL_TRIANGLES, m_Indices.size(), GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);
		}

	}
}

void Model::Clear()
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

void Model::LoadModel(const std::string& Filename, unsigned int ASSIMP_FLAGS)
{
	Clear();

	directory = Filename.substr(0, Filename.find_last_of('/'));	// Строка хранящая путь до модели

	glGenVertexArrays(1, &m_VAO);
	glBindVertexArray(m_VAO);

	int sizeBuffersInElements = sizeof(m_Buffers) / sizeof(m_Buffers[0]);
	glGenBuffers(sizeBuffersInElements, m_Buffers);

	pScene = Importer.ReadFile(Filename.c_str(), ASSIMP_FLAGS);

	if (pScene)
	{
		InitFromScene(pScene, Filename);
		m_GlobalInverseTransform = convertMat4(pScene->mRootNode->mTransformation);
		m_GlobalInverseTransform = glm::inverse(m_GlobalInverseTransform);
	}
	else
	{
		printf("Error loading '%s' : '%s'\n", Filename.c_str(), Importer.GetErrorString());
	}

	glBindVertexArray(0);
}

void Model::InitFromScene(const aiScene* pScene, const std::string& Filename)
{
	m_Meshes.resize(pScene->mNumMeshes);
	m_Textures.resize(pScene->mNumMaterials);

	if (pScene->HasAnimations())
	{
		std::cout << "Animations detected!\n";
	}
	else
	{
		std::cout << "Animations NOT detected!\n";
	}

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


	PopulateBuffers();
}

void Model::CountVerticesAndIndices(const aiScene* pScene, unsigned int& NumVertices, unsigned int& NumIndices)
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

void Model::ReserveSpace(unsigned int NumVertices, unsigned int NumIndices)
{
	m_Positions.reserve(NumVertices);
	m_Normals.reserve(NumVertices);
	m_TexCoords.reserve(NumVertices);
	m_Indices.reserve(NumIndices);
	m_Bones.resize(NumVertices);
}

void Model::InitAllMeshes(const aiScene* pScene)
{
	for (unsigned int i = 0; i < pScene->mNumMeshes; i++)
	{
		const aiMesh* paiMesh = pScene->mMeshes[i];
		InitSingleMesh(i, paiMesh);
	}
}

void Model::InitSingleMesh(unsigned int MeshIndex, const aiMesh* paiMesh)
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

	for (unsigned int i = 0; i < paiMesh->mNumFaces; i++)
	{
		const aiFace& Face = paiMesh->mFaces[i];

		m_Indices.push_back(Face.mIndices[0]);
		m_Indices.push_back(Face.mIndices[1]);
		m_Indices.push_back(Face.mIndices[2]);
	}
}

void Model::LoadMeshBones(unsigned int MeshIndex, const aiMesh* pMesh)
{
	for (unsigned int i = 0; i < pMesh->mNumBones; i++)
	{
		LoadSingleBone(MeshIndex, pMesh->mBones[i]);
	}
}

void Model::LoadSingleBone(unsigned int MeshIndex, const aiBone* pBone)
{
	int BoneID = getBoneId(pBone);

	if (BoneID == m_BoneInfo.size())
	{
		BoneInfo bi(pBone->mOffsetMatrix);
		m_BoneInfo.push_back(bi);
	}

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

int Model::getBoneId(const aiBone* pBone)	// Возвращает уникальный ID кости, если его нет в базе, создёт и возвращает
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

bool Model::InitAllMaterials(const aiScene* pScene, std::string Filename)
{


	for (int i = 0; i < pScene->mNumMaterials; i++)
	{

		aiMaterial* material = pScene->mMaterials[i];

		m_Textures = LoadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");

	}
	return true;
}

std::vector<Model::Texture> Model::LoadMaterialTextures(aiMaterial* material, aiTextureType type, std::string typeName)
{
	std::vector<Texture> textures;

	for (unsigned int i = 0; i < material->GetTextureCount(type); i++)
	{
		aiString str;

		material->GetTexture(type, i, &str);
		Texture texture;

		texture.id = TextureFromFile(str.C_Str(), this->directory);

		texture.path = str.C_Str();
		texture.type = typeName;
		textures.push_back(texture);
	}

	return textures;
}

void Model::PopulateBuffers()
{
	glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[POSITION_VERTEX_BUFFER]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(m_Positions[0]) * m_Positions.size(), &m_Positions[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(POSITION_LOCATION);
	glVertexAttribPointer(POSITION_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[TEXCOORD_VEREX_BUFFER]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(m_TexCoords[0]) * m_TexCoords.size(), &m_TexCoords[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(TEX_COORD_LOCATION);
	glVertexAttribPointer(TEX_COORD_LOCATION, 2, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[NORMAL_VEREX_BUFFER]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(m_Normals[0]) * m_Normals.size(), &m_Normals[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(NORMAL_LOCATION);
	glVertexAttribPointer(NORMAL_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[BONE_VERTEX_BUFFER]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(m_Bones[0]) * m_Bones.size(), &m_Bones[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(BONE_ID_LOCATION);
	glVertexAttribIPointer(BONE_ID_LOCATION, MAX_NUM_BONES_PER_VERTEX, GL_INT, sizeof(VertexBoneData), (const GLvoid*)0);

	glEnableVertexAttribArray(BONE_WEIGHT_LOCATION);
	glVertexAttribPointer(BONE_WEIGHT_LOCATION, MAX_NUM_BONES_PER_VERTEX, GL_FLOAT, GL_FALSE, sizeof(VertexBoneData), (const GLvoid*)offsetof(VertexBoneData, Weights));

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Buffers[INDEX_BUFFER]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(m_Indices[0]) * m_Indices.size(), &m_Indices[0], GL_STATIC_DRAW);
}

std::string Model::fixPathSlash(std::string string)
{
	while (string.find('\\') != string.npos)
	{
		int index = string.find('\\');
		string.erase(index, 1);
		string.insert(index, "/");
	}

	return string;
}

unsigned int Model::TextureFromFile(const char* innerPath, const std::string& directory)
{
	std::string filename = std::string(innerPath);

	// Рабочий код
	/*
	
	std::cout << "1. filename = " << filename << std::endl;

	filename = directory + "/" + filename;

	std::cout << "directory + / + filename = " << filename << std::endl;

	filename = fixPathSlash(filename);

	std::cout << "directory with changed filename = " << filename << std::endl;
	
	*/

	filename = directory + '/' + filename;

	filename = fixPathSlash(filename);
	std::cout << "directory with changed filename = " << filename << std::endl;

	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;

	unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);

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
		std::cout << "Texture failed to load at path: " << filename << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}

void Model::GetBoneTransforms(float TimeInSeconds,  std::vector<glm::mat4>& Transforms)
{
	Transforms.resize(m_BoneInfo.size());

	glm::mat4  Identity(1.0f);

	float TicksPerSecond = (float)(pScene->mAnimations[0]->mTicksPerSecond != 0 ? pScene->mAnimations[0]->mTicksPerSecond : 25.0f);
	float TimeInTicks = TimeInSeconds * TicksPerSecond;
	float AnimationTimePerTicks = fmod(TimeInTicks, (float)pScene->mAnimations[0]->mDuration);

	ReadNodeHierarchy(AnimationTimePerTicks, pScene->mRootNode, Identity);

	for (unsigned int i = 0; i < m_BoneInfo.size(); i++) {
		Transforms[i] = m_BoneInfo[i].FinalTransformation;
	}
}

void Model::CalcInterpolatedScaling(aiVector3D& Out, float AnimationTimeTicks, const aiNodeAnim* pNodeAnim)
{
	if (pNodeAnim->mNumScalingKeys == 1)
	{
		Out = pNodeAnim->mScalingKeys[0].mValue;
		return;
	}

	unsigned int ScalingIndex = FindScaling(AnimationTimeTicks, pNodeAnim);
	unsigned int NextScalingIndex = ScalingIndex + 1;
	assert(NextScalingIndex < pNodeAnim->mNumScalingKeys);
	float t1 = (float)pNodeAnim->mScalingKeys[ScalingIndex].mTime;
	float t2 = (float)pNodeAnim->mScalingKeys[NextScalingIndex].mTime;
	float DeltaTime = t2 - t1;
	float Factor = (AnimationTimeTicks - (float)t1) / DeltaTime;
	assert(Factor > = 0.0f && Factor <= 1.0f);
	const aiVector3D& Start = pNodeAnim->mScalingKeys[ScalingIndex].mValue;
	const aiVector3D& End = pNodeAnim->mScalingKeys[NextScalingIndex].mValue;
	aiVector3D Delta = End - Start;
	Out = Start + Factor * Delta;

}

unsigned int Model::FindScaling(float AnimationTimeTicks, const aiNodeAnim* pNodeAnim)
{
	assert(pNodeAnim->mNumScalingKeys > 0);

	for (unsigned int i = 0; i < pNodeAnim->mNumScalingKeys - 1; i++)
	{
		float t = (float)pNodeAnim->mScalingKeys[i + 1].mTime;
		if (AnimationTimeTicks < t)
		{
			return i;
		}
	}

	return 0;
}

void Model::ReadNodeHierarchy(float AnimationTimeTicks, const aiNode* pNode, const glm::mat4& ParentTransform)
{
	std::string NodeName(pNode->mName.data);

	const aiAnimation* pAnimation = pScene->mAnimations[0];

	const aiNodeAnim* pNodeAnim = FindNodeAnim(pAnimation, NodeName);

	if (pNodeAnim)
	{
		aiVector3D Scaling;
		CalcInterpolatedScaling(Scaling, AnimationTimeTicks, pNodeAnim);

		glm::vec3 glmScaling = glm::vec3(Scaling.x, Scaling.y, Scaling.z);
		glm::mat4 ScalingM(1.0f);
		ScalingM = glm::scale(ScalingM, glmScaling);

		std::cout << pNode->mName.C_Str() << "\n";
		for (int i = 0; i < 3; i++)
		{
			std::cout << ScalingM[i][0] << " " << ScalingM[i][1] << " " << ScalingM[0][2] << " " << ScalingM[0][3] << "\n";
		}
		std::cout << "\n";
		
	}

	glm::mat4 NodeTransformation(convertMat4(pNode->mTransformation));

//	printf("%s - \n", NodeName.c_str());

	glm::mat4 GlobalTransformation = ParentTransform * NodeTransformation;

	if (m_BoneNameToIndexMap.find(NodeName) != m_BoneNameToIndexMap.end()) {
		unsigned int BoneIndex = m_BoneNameToIndexMap[NodeName];
		m_BoneInfo[BoneIndex].FinalTransformation = m_GlobalInverseTransform * GlobalTransformation * m_BoneInfo[BoneIndex].OffsetMatrix;
	}

	for (unsigned int i = 0; i < pNode->mNumChildren; i++) {
		ReadNodeHierarchy(AnimationTimeTicks, pNode->mChildren[i], GlobalTransformation);
	}
}	

const aiNodeAnim* Model::FindNodeAnim(const aiAnimation* pAnimation, const std::string NodeName)
{
	for (unsigned int i = 0; i < pAnimation->mNumChannels; i++)
	{
		const aiNodeAnim* pNodeAnim = pAnimation->mChannels[i];

		if ((std::string)pNodeAnim->mNodeName.data == NodeName)
		{
			return pNodeAnim;
		}
	}

	return NULL;
}