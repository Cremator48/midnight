#include "model.h"

static int space_count = 0;


void print_space()
{
	for (int i = 0; i < space_count; i++) {
		printf(" ");
	}
}

Model::Model(const std::string& Filename, unsigned int ASSIMP_FLAGS)
{
	LoadModel(Filename, ASSIMP_FLAGS);
}

Model::~Model()
{
	Clear();
}

void Model::printMatrix(glm::mat4 matrix)
{
	std::cout << (float)matrix[0][0] << " " << (float)matrix[1][0] << " " << (float)matrix[2][0] << " " << (float)matrix[3][0] << "\n";
	std::cout << (float)matrix[0][1] << " " << (float)matrix[1][1] << " " << (float)matrix[2][1] << " " << (float)matrix[3][1] << "\n";
	std::cout << (float)matrix[0][2] << " " << (float)matrix[1][2] << " " << (float)matrix[2][2] << " " << (float)matrix[3][2] << "\n";
	std::cout << (float)matrix[0][3] << " " << (float)matrix[1][3] << " " << (float)matrix[2][3] << " " << (float)matrix[3][3] << "\n";
	std::cout << "\n";
}

static void print_assimp_matrix(const aiMatrix4x4& m)
{
	std::cout << (float)m.a1 << " " << (float)m.a2 << " " << (float)m.a3 << " " << (float)m.a4 << "\n";
	std::cout << (float)m.b1 << " " << (float)m.b2 << " " << (float)m.b3 << " " << (float)m.b4 << "\n";
	std::cout << (float)m.c1 << " " << (float)m.c2 << " " << (float)m.c3 << " " << (float)m.c4 << "\n";
	std::cout << (float)m.d1 << " " << (float)m.d2 << " " << (float)m.d3 << " " << (float)m.d4 << "\n";
	std::cout << "\n";
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

	
	pScene = Importer.ReadFile(Filename, ASSIMP_FLAGS);
	
	//Importer.SetPropertyBool(AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, true);

	if (pScene)
	{
		maxNumOfAnimations = pScene->mNumAnimations;
		m_GlobalInverseTransform = convertMat4(pScene->mRootNode->mTransformation);
		m_GlobalInverseTransform = glm::inverse(m_GlobalInverseTransform);
		InitFromScene(pScene, Filename);


		glm::mat4 glmMatrix(1.0f);
		aiMatrix4x4 aiMatrix;
		aiMatrix.a1 = glmMatrix[0][0];
		aiMatrix.a2 = glmMatrix[1][0];
		aiMatrix.a3 = glmMatrix[2][0];
		aiMatrix.a4 = glmMatrix[3][0];

		aiMatrix.b1 = glmMatrix[0][1];
		aiMatrix.b2 = glmMatrix[1][1];
		aiMatrix.b3 = glmMatrix[2][1];
		aiMatrix.b4 = glmMatrix[3][1];

		aiMatrix.c1 = glmMatrix[0][2];
		aiMatrix.c2 = glmMatrix[1][2];
		aiMatrix.c3 = glmMatrix[2][2];
		aiMatrix.c4 = glmMatrix[3][2];

		aiMatrix.d1 = glmMatrix[0][3];
		aiMatrix.d2 = glmMatrix[1][3];
		aiMatrix.d3 = glmMatrix[2][3];
		aiMatrix.d4 = glmMatrix[3][3];

		printf("%f   %f   %f   %f\n", aiMatrix.a1, aiMatrix.a2, aiMatrix.a3, aiMatrix.a4);
		printf("%f   %f   %f   %f\n", aiMatrix.b1, aiMatrix.b2, aiMatrix.b3, aiMatrix.b4);
		printf("%f   %f   %f   %f\n", aiMatrix.c1, aiMatrix.c2, aiMatrix.c3, aiMatrix.c4);
		printf("%f   %f   %f   %f\n", aiMatrix.d1, aiMatrix.d2, aiMatrix.d3, aiMatrix.d4);
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
//	std::cout << "mNumMaterials: " << pScene->mNumMaterials << "\n";

	if (pScene->HasAnimations())
	{
		std::cout << "Animations detected!\nNum of animations: " << pScene->mNumAnimations << "\n";
	//	for(int i = 0; i < pScene->mNumAnimations; i++)
	//	std::cout << i << "\nName of animations : " << pScene->mAnimations[i]->mName.C_Str() << "\n";

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
		if (!InitAllMaterials(pScene, Filename))
		{
			std::cout << "Error init all materials!\n";
		}
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

void Model::LoadSingleBone(unsigned int MeshIndex, const aiBone* pBone) ///////////////!!!!!!!!!!!!!!
{
	int BoneID = getBoneId(pBone);

	if (BoneID == m_BoneInfo.size())
	{
		BoneInfo bi(*pBone);

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

	//for (int i = 0; i < pScene->mNumMaterials; i++)
	//{
	//	std::cout << "[" << i << "] pScene->mMaterials[i] name is" << pScene->mMaterials[i]->GetName().C_Str() << "\n";
	//}

	for (int i = 0; i < pScene->mNumMaterials-1; i++)
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
	

	/*std::cout << "1. filename = " << filename << std::endl;

	filename = directory + "/" + filename;

	std::cout << "directory + / + filename = " << filename << std::endl;

	filename = fixPathSlash(filename);

	std::cout << "directory with changed filename = " << filename << std::endl;*/

	

	filename = directory + '/' + filename;

	filename = fixPathSlash(filename);
//	std::cout << "directory with changed filename = " << filename << std::endl;

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

void Model::GetBoneTransforms(float TimeInSeconds, std::vector<glm::mat4>& Transforms, float factor)
{
	glm::mat4 Identity(1.0f);
	
	// Текущее время в тиках
	TicksPerSecondVector.resize(pScene->mNumAnimations);
	TimeInTicksVector.resize(pScene->mNumAnimations);
	firstFrameVector.resize(pScene->mNumAnimations);
	AnimationTimeTicksVector.resize(pScene->mNumAnimations);

	for (int i = 0; i < pScene->mNumAnimations; i++)
	{
		TicksPerSecondVector[i] = (float)(pScene->mAnimations[i]->mTicksPerSecond != 0 ? pScene->mAnimations[i]->mTicksPerSecond : 25.0f);
	//	TicksPerSecondVector[i] = 120.0f;
		TimeInTicksVector[i] = TimeInSeconds * TicksPerSecondVector[i];
		firstFrameVector[i] = pScene->mAnimations[i]->mChannels[0]->mPositionKeys[0].mTime;
		AnimationTimeTicksVector[i] = firstFrameVector[i] + fmod(TimeInTicksVector[i], (float)pScene->mAnimations[i]->mDuration - firstFrameVector[i]);
	}
	
	//	printf("TimeInTicks: %f\n", TimeInTicks);

	//	printf("Duraiton: %f\n", (float)pScene->mAnimations[numOfAnimation]->mDuration);

	// Остаток от деления текущего времени в тиках на длительность анимации в тиках
	// Эта переменная содержит текущее время для анимации в тиках. После завершения анимации - анимация начинается сначала
	//	float firstAnimationTimeTicks = fmod(firstTimeInTicks, (float)pScene->mAnimations[1]->mDuration);
	
	
	//printf("AnimationTimeTicks: %f\n", secondAnimationTimeTicks);

//	ReadNodeHierarchy(secondAnimationTimeTicks, pScene->mRootNode, Identity, 2);
	BlendAnimationReadNodeHierarchy(AnimationTimeTicksVector[1], AnimationTimeTicksVector[2], pScene->mRootNode, Identity, 1, 2, factor);
	Transforms.resize(m_BoneInfo.size());

	for (unsigned int i = 0; i < m_BoneInfo.size(); i++) {
		Transforms[i] = m_BoneInfo[i].FinalTransformation;
	}

}



void Model::CalcInterpolatedScaling(aiVector3D& Out, float AnimationTimeTicks, const aiNodeAnim* pNodeAnim, const aiAnimation* pAnimation)
{
	if (pNodeAnim->mNumScalingKeys == 1)
	{
		Out = pNodeAnim->mScalingKeys[0].mValue;
		return;
	}

	if (AnimationTimeTicks < pNodeAnim->mScalingKeys[0].mTime)
	{
		float DeltaTime = (float)pNodeAnim->mScalingKeys[0].mTime;
		float Factor = AnimationTimeTicks / DeltaTime;

		const aiVector3D& Start = pNodeAnim->mScalingKeys[pNodeAnim->mNumScalingKeys - 1].mValue;
		const aiVector3D& End = pNodeAnim->mScalingKeys[0].mValue;
		aiVector3D Delta = End - Start;
		Out = Start + Factor * Delta;
	}
	else if (AnimationTimeTicks > pNodeAnim->mScalingKeys[pNodeAnim->mNumScalingKeys - 1].mTime)
	{
		std::cout << "It's happen!\n";
	}
	else
	{
		unsigned int ScalingIndex = FindScaling(AnimationTimeTicks, pNodeAnim);
		unsigned int NextScalingIndex = ScalingIndex + 1;
		assert(NextScalingIndex < pNodeAnim->mNumScalingKeys);
		float t1 = (float)pNodeAnim->mScalingKeys[ScalingIndex].mTime;
		float t2 = (float)pNodeAnim->mScalingKeys[NextScalingIndex].mTime;
		float DeltaTime = t2 - t1;
		float Factor = (AnimationTimeTicks - (float)t1) / DeltaTime;
		assert(Factor >= 0.0f && Factor <= 1.0f);
		const aiVector3D& Start = pNodeAnim->mScalingKeys[ScalingIndex].mValue;
		const aiVector3D& End = pNodeAnim->mScalingKeys[NextScalingIndex].mValue;
		aiVector3D Delta = End - Start;
		Out = Start + Factor * Delta;
	}
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

unsigned int findRotation(float AnimationTimeTicks, const aiNodeAnim* pNodeAnim)
{
	assert(pNodeAnim->mNumRotationKeys > 0);

	for (unsigned int i = 0; i < pNodeAnim->mNumRotationKeys - 1; i++)
	{
		float t = (float)pNodeAnim->mRotationKeys[i + 1].mTime;
		if (AnimationTimeTicks < t)
		{
			return i;
		}
	}

	return 0;
}

void CalcInterpolatedRotation(aiQuaternion& Out, float AnimationTimeTicks, const aiNodeAnim* pNodeAnim)
{
	if (pNodeAnim->mNumRotationKeys == 1)
	{
		Out = pNodeAnim->mRotationKeys[0].mValue;
		return;
	}

	if (AnimationTimeTicks < pNodeAnim->mRotationKeys[0].mTime)
	{
		float DeltaTime = pNodeAnim->mRotationKeys[0].mTime;
		float Factor = AnimationTimeTicks / DeltaTime;
		// std::cout << Factor << "\n";
		assert(Factor >= 0.0f && Factor <= 1.0f);
		const aiQuaternion& StartRotationQ = pNodeAnim->mRotationKeys[pNodeAnim->mNumRotationKeys-1].mValue;
		const aiQuaternion& EndRotationQ = pNodeAnim->mRotationKeys[0].mValue;
		aiQuaternion::Interpolate(Out, StartRotationQ, EndRotationQ, Factor);
		Out.Normalize();
	}
	else
	{
		unsigned int RotationIndex = findRotation(AnimationTimeTicks, pNodeAnim);
		unsigned int NextRotationIndex = RotationIndex + 1;
		assert(NextRotationIndex < pNodeAnim->mNumRotationKeys);
		float t1 = (float)pNodeAnim->mRotationKeys[RotationIndex].mTime;
		float t2 = (float)pNodeAnim->mRotationKeys[NextRotationIndex].mTime;
		float DeltaTime = t2 - t1;
		float Factor = (AnimationTimeTicks - t1) / DeltaTime;
		assert(Factor >= 0.0f && Factor <= 1.0f);
		const aiQuaternion& StartRotationQ = pNodeAnim->mRotationKeys[RotationIndex].mValue;
		const aiQuaternion& EndRotationQ = pNodeAnim->mRotationKeys[NextRotationIndex].mValue;
		aiQuaternion::Interpolate(Out, StartRotationQ, EndRotationQ, Factor);
		Out.Normalize();
	}
	
}

unsigned int FindPosition(float AnimationTimeTicks, const aiNodeAnim* pNodeAnim)
{
	for (unsigned int i = 0; i < pNodeAnim->mNumPositionKeys - 1; i++) {
		float t = (float)pNodeAnim->mPositionKeys[i + 1].mTime;
		if (AnimationTimeTicks < t) {
			return i;
		}
	}

	return 0;
}

void CalcInterpolatedPosition(aiVector3D& Out, float AnimationTimeTicks, const aiNodeAnim* pNodeAnim)
{
	if (pNodeAnim->mNumPositionKeys == 1) {
		Out = pNodeAnim->mPositionKeys[0].mValue;
		return;
	}

	if (AnimationTimeTicks < pNodeAnim->mPositionKeys[0].mTime)
	{
		float DeltaTime = (float)pNodeAnim->mPositionKeys[0].mTime;
		float Factor = AnimationTimeTicks / DeltaTime;

		const aiVector3D& Start = pNodeAnim->mPositionKeys[pNodeAnim->mNumPositionKeys - 1].mValue;
		const aiVector3D& End = pNodeAnim->mPositionKeys[0].mValue;
		aiVector3D Delta = End - Start;
		Out = Start + Factor * Delta;
	}
	else
	{
		unsigned int PositionIndex = FindPosition(AnimationTimeTicks, pNodeAnim);
		unsigned int NextPositionIndex = PositionIndex + 1;
		assert(NextPositionIndex < pNodeAnim->mNumPositionKeys);
		float t1 = (float)pNodeAnim->mPositionKeys[PositionIndex].mTime;
		float t2 = (float)pNodeAnim->mPositionKeys[NextPositionIndex].mTime;
		float DeltaTime = t2 - t1;
		float Factor = (AnimationTimeTicks - t1) / DeltaTime;
		assert(Factor >= 0.0f && Factor <= 1.0f);
		const aiVector3D& Start = pNodeAnim->mPositionKeys[PositionIndex].mValue;
		const aiVector3D& End = pNodeAnim->mPositionKeys[NextPositionIndex].mValue;
		aiVector3D Delta = End - Start;
		Out = Start + Factor * Delta;
	}
	
}




void Model::ReadNodeHierarchy(float AnimationTimeTicks, const aiNode* pNode, const glm::mat4& ParentTransform, int numOfAnimation)
{
	std::string NodeName(pNode->mName.data);

	const aiAnimation* pAnimation = pScene->mAnimations[numOfAnimation];
	glm::mat4 NodeTransformation = convertMat4(pNode->mTransformation);
	
	// Поиск анимции конкретного узла среди всех внутри анимации
	const aiNodeAnim* pNodeAnim = FindNodeAnim(pAnimation, NodeName);

	if (pNodeAnim)
	{
		aiVector3D Scaling;
		CalcInterpolatedScaling(Scaling, AnimationTimeTicks, pNodeAnim, pAnimation);
		glm::vec3 glmScaling(Scaling.x, Scaling.y, Scaling.z);
		glm::mat4 ScalingM(1.0f);
		ScalingM = glm::scale(ScalingM, glmScaling);

		
		

		aiQuaternion RotationQ;
		CalcInterpolatedRotation(RotationQ, AnimationTimeTicks, pNodeAnim);
		glm::mat4 RotationM = glm::mat4(convertMat4(RotationQ.GetMatrix()));


		aiVector3D Translation;
		CalcInterpolatedPosition(Translation, AnimationTimeTicks, pNodeAnim);
		glm::mat4 TranslationM(1.0f);
		TranslationM = glm::translate(TranslationM, glm::vec3(Translation.x, Translation.y, Translation.z));

		NodeTransformation = TranslationM * RotationM * ScalingM;
	}

	//		printf("%s - \n", NodeName.c_str());

	glm::mat4 GlobalTransformation = ParentTransform * NodeTransformation;

	if (m_BoneNameToIndexMap.find(NodeName) != m_BoneNameToIndexMap.end()) {
		unsigned int BoneIndex = m_BoneNameToIndexMap[NodeName];
		m_BoneInfo[BoneIndex].FinalTransformation = GlobalTransformation * m_BoneInfo[BoneIndex].OffsetMatrix;
	}

	for (unsigned int i = 0; i < pNode->mNumChildren; i++) {
		ReadNodeHierarchy(AnimationTimeTicks, pNode->mChildren[i], GlobalTransformation, numOfAnimation);
	}



}

void Model::BlendAnimationReadNodeHierarchy(float FirstAnimationTimeTicks, float SecondAnimationTimeTicks, const aiNode* pNode, const glm::mat4& ParentTransform, int numOfFirstAnimation, int numOfSecondAnimation, float FactorOfBlendAnim)
{
	std::string NodeName(pNode->mName.data);

	const aiAnimation* pFirstAnimation = pScene->mAnimations[numOfFirstAnimation];

	const aiAnimation* pSecondAnimation = pScene->mAnimations[numOfSecondAnimation];

	glm::mat4 NodeTransformation = convertMat4(pNode->mTransformation);

	// Первая анимция этого конкретного узла
	const aiNodeAnim* pNodeFirstAnim = FindNodeAnim(pFirstAnimation, NodeName);

	// Вторая анимация этого конкретного узла
	const aiNodeAnim* pNodeSecondAnim = FindNodeAnim(pSecondAnimation, NodeName);

	// Возможны 3 варианта:
	// [1] Обе анимации содержат ключи анимции для этого узла;
	// [2] Первая анимация содержит анимацию для этого узла, а вторая анимация не содержит;
	// [3] Первая анимация не содержит анимацию для этого узла, а вторая содержит.

	if (pNodeFirstAnim && pNodeSecondAnim) /*[0]*/
	{
		aiVector3D firstScaling, secondScaling;
		CalcInterpolatedScaling(firstScaling, FirstAnimationTimeTicks, pNodeFirstAnim, pFirstAnimation);
		CalcInterpolatedScaling(secondScaling, SecondAnimationTimeTicks, pNodeSecondAnim, pSecondAnimation);

		glm::vec3 glmFirstScaling(firstScaling.x, firstScaling.y, firstScaling.z);
		glm::vec3 glmSecondScaling(secondScaling.x, secondScaling.y, secondScaling.z);

		// float Factor = степень перехода от pos1 к pos2. Лежит между 0 и 1;
		// vec3 Delta = pos2 - pos1; 
		// Интерполированный vec3 = pos1 + Factor * Delta; 

		
		glm::vec3 DeltaScaling = glmSecondScaling - glmFirstScaling;
		glm::vec3 ResultScaling = glmFirstScaling + FactorOfBlendAnim * DeltaScaling;

		glm::mat4 ScalingM(1.0f);
		ScalingM = glm::scale(ScalingM, ResultScaling);

		aiQuaternion firstRotationQ, secondRotationQ, resultRotationQ;
		CalcInterpolatedRotation(firstRotationQ, FirstAnimationTimeTicks, pNodeFirstAnim);
		CalcInterpolatedRotation(secondRotationQ, SecondAnimationTimeTicks, pNodeSecondAnim);


		aiQuaternion::Interpolate(resultRotationQ, firstRotationQ, secondRotationQ, FactorOfBlendAnim);

		glm::mat4 RotationM = glm::mat4(convertMat4(resultRotationQ.GetMatrix()));



		aiVector3D firstTranslation, secondTranslation;
		glm::vec3 firstTranslationGLM, secondTranslationGLM, resultTranslation;
		CalcInterpolatedPosition(firstTranslation, FirstAnimationTimeTicks, pNodeFirstAnim);
		CalcInterpolatedPosition(secondTranslation, SecondAnimationTimeTicks, pNodeSecondAnim);

		// vec3 Delta = pos2 - pos1; 
		// Интерполированный vec3 = pos1 + Factor * Delta; 

		firstTranslationGLM = glm::vec3(firstTranslation.x, firstTranslation.y, firstTranslation.z);
		secondTranslationGLM = glm::vec3(secondTranslation.x, secondTranslation.y, secondTranslation.z);
		glm::vec3 DeltaTranslation = secondTranslationGLM - firstTranslationGLM;
		resultTranslation = firstTranslationGLM + FactorOfBlendAnim * DeltaTranslation;

		glm::mat4 TranslationM(1.0f);
		TranslationM = glm::translate(TranslationM, resultTranslation);

		NodeTransformation = TranslationM * RotationM * ScalingM;
	}
	else if(!pNodeFirstAnim && pNodeSecondAnim) //[1]
	{
		aiMatrix4x4 pTransform = pNode->mTransformation;
		aiVector3D pTranformScaling, pTransformPosition;
		aiQuaternion pTranformQuat;
		pTransform.Decompose(pTranformScaling, pTranformQuat, pTransformPosition);


		// Расчёт интерполяции (pNodeSecondAnim) анимированного узла
		aiVector3D Scaling;
		CalcInterpolatedScaling(Scaling, SecondAnimationTimeTicks, pNodeSecondAnim, pSecondAnimation);
		glm::vec3 glmScaling(Scaling.x, Scaling.y, Scaling.z);
		
		// float Factor = степень перехода от pos1 к pos2. Лежит между 0 и 1;
		// vec3 Delta = pos2 - pos1; 
		// Интерполированный vec3 = pos1 + Factor * Delta; 

		// Интерполяция между анимированным и не анимированным положением

		glm::vec3 DeltaScaling = glmScaling - glm::vec3(pTranformScaling.x, pTranformScaling.y, pTranformScaling.z);
		glm::vec3 resultScaling = glmScaling + FactorOfBlendAnim * DeltaScaling;

		glm::mat4 ScalingM(1.0f);
		ScalingM = glm::scale(ScalingM, resultScaling);



		aiQuaternion RotationQ;
		CalcInterpolatedRotation(RotationQ, SecondAnimationTimeTicks, pNodeSecondAnim);
		
		aiQuaternion resultRotation;
		aiQuaternion::Interpolate(resultRotation, RotationQ, pTranformQuat, FactorOfBlendAnim);
		

		glm::mat4 RotationM = glm::mat4(convertMat4(resultRotation.GetMatrix()));



		aiVector3D Translation;
		CalcInterpolatedPosition(Translation, SecondAnimationTimeTicks, pNodeSecondAnim);
		glm::vec3 firstTranslationVec = glm::vec3(Translation.x, Translation.y, Translation.z);

		glm::mat4 TranslationM(1.0f);
		// vec3 Delta = pos2 - pos1; 
		// Интерполированный vec3 = pos1 + Factor * Delta; 
		glm::vec3 DeltaTranslation = glm::vec3(pTransformPosition.x, pTransformPosition.y, pTransformPosition.z) - firstTranslationVec;
		glm::vec3 resultTranslation = firstTranslationVec + FactorOfBlendAnim * DeltaTranslation;

		TranslationM = glm::translate(TranslationM, resultTranslation);

		NodeTransformation = TranslationM * RotationM * ScalingM;
	}
	else if (pNodeFirstAnim && !pNodeSecondAnim) // [2]
	{
		aiMatrix4x4 pTransform = pNode->mTransformation;
		aiVector3D pTranformScaling, pTransformPosition;
		aiQuaternion pTranformQuat;
		pTransform.Decompose(pTranformScaling, pTranformQuat, pTransformPosition);


		// Расчёт интерполяции (pNodeSecondAnim) анимированного узла
		aiVector3D Scaling;
		CalcInterpolatedScaling(Scaling, FirstAnimationTimeTicks, pNodeFirstAnim, pFirstAnimation);
		glm::vec3 glmScaling(Scaling.x, Scaling.y, Scaling.z);

		// float Factor = степень перехода от pos1 к pos2. Лежит между 0 и 1;
		// vec3 Delta = pos2 - pos1; 
		// Интерполированный vec3 = pos1 + Factor * Delta; 

		// Интерполяция между анимированным и не анимированным положением

		glm::vec3 DeltaScaling = glmScaling - glm::vec3(pTranformScaling.x, pTranformScaling.y, pTranformScaling.z);
		glm::vec3 resultScaling = glmScaling + FactorOfBlendAnim * DeltaScaling;

		glm::mat4 ScalingM(1.0f);
		ScalingM = glm::scale(ScalingM, resultScaling);



		aiQuaternion RotationQ;
		CalcInterpolatedRotation(RotationQ, FirstAnimationTimeTicks, pNodeFirstAnim);

		aiQuaternion resultRotation;
		aiQuaternion::Interpolate(resultRotation, RotationQ, pTranformQuat, FactorOfBlendAnim);


		glm::mat4 RotationM = glm::mat4(convertMat4(resultRotation.GetMatrix()));



		aiVector3D Translation;
		CalcInterpolatedPosition(Translation, FirstAnimationTimeTicks, pNodeFirstAnim);
		glm::vec3 firstTranslationVec = glm::vec3(Translation.x, Translation.y, Translation.z);

		glm::mat4 TranslationM(1.0f);
		// vec3 Delta = pos2 - pos1; 
		// Интерполированный vec3 = pos1 + Factor * Delta; 
		glm::vec3 DeltaTranslation = glm::vec3(pTransformPosition.x, pTransformPosition.y, pTransformPosition.z) - firstTranslationVec;
		glm::vec3 resultTranslation = firstTranslationVec + FactorOfBlendAnim * DeltaTranslation;

		TranslationM = glm::translate(TranslationM, resultTranslation);

		NodeTransformation = TranslationM * RotationM * ScalingM;
	}


	glm::mat4 GlobalTransformation = ParentTransform * NodeTransformation;

	if (m_BoneNameToIndexMap.find(NodeName) != m_BoneNameToIndexMap.end()) {
		unsigned int BoneIndex = m_BoneNameToIndexMap[NodeName];
		m_BoneInfo[BoneIndex].FinalTransformation = GlobalTransformation * m_BoneInfo[BoneIndex].OffsetMatrix;
	}

	for (unsigned int i = 0; i < pNode->mNumChildren; i++) {
		BlendAnimationReadNodeHierarchy(FirstAnimationTimeTicks, SecondAnimationTimeTicks, pNode->mChildren[i], GlobalTransformation, numOfFirstAnimation, numOfSecondAnimation, FactorOfBlendAnim);
	}

}



const aiNodeAnim* Model::FindNodeAnim(const aiAnimation* pAnimation, const std::string& NodeName)
{
	for (unsigned int i = 0; i < pAnimation->mNumChannels; i++) {
		const aiNodeAnim* pNodeAnim = pAnimation->mChannels[i];

		if (std::string(pNodeAnim->mNodeName.data) == NodeName) {
			return pNodeAnim;
		}
	}

	return NULL;
}