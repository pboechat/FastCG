#include <ModelImporter.h>
#include <AssimpUtils.h>
#include <Transform.h>
#include <MeshRenderer.h>
#include <MeshFilter.h>
#include <ShaderRegistry.h>
#include <TextureImporter.h>
#include <Colors.h>
#include <Texture.h>
#include <File.h>
#include <MathT.h>
#include <FileReader.h>
#include <Exception.h>

#include <assimp/PostProcess.h>

#include <map>
#include <stdlib.h>
#include <stdio.h>

const std::string ModelImporter::DEFAULT_LOG_FILE = "assimp.log";

bool ModelImporter::s_mInitialized = false;
std::vector<Texture*> ModelImporter::s_mManagedTextures;
std::vector<Material*> ModelImporter::s_mManagedMaterials;
std::vector<Mesh*> ModelImporter::s_mManagedMeshes;
std::vector<const aiScene*> ModelImporter::s_mImportedScenes;
Assimp::Logger* ModelImporter::s_mpCurrentLogger = 0;

//////////////////////////////////////////////////////////////////////////
void ModelImporter::Initialize()
{
	if (s_mInitialized)
	{
		// FIXME: checking invariants
		THROW_EXCEPTION(Exception, "s_mInitialized");
	}

	s_mInitialized = true;
}

//////////////////////////////////////////////////////////////////////////
void ModelImporter::DeleteLogger()
{
	if (s_mpCurrentLogger != 0)
	{
		delete s_mpCurrentLogger;
		s_mpCurrentLogger = 0;
	}
}

//////////////////////////////////////////////////////////////////////////
void ModelImporter::NoLog()
{
	DeleteLogger();
}

//////////////////////////////////////////////////////////////////////////
void ModelImporter::LogToConsole()
{
	DeleteLogger();

	Assimp::Logger::LogSeverity severity = Assimp::Logger::VERBOSE;
	s_mpCurrentLogger = Assimp::DefaultLogger::create("", severity, aiDefaultLogStream_STDOUT);
}

//////////////////////////////////////////////////////////////////////////
void ModelImporter::LogToFile()
{
	DeleteLogger();

	Assimp::Logger::LogSeverity severity = Assimp::Logger::VERBOSE;
	s_mpCurrentLogger = Assimp::DefaultLogger::create(DEFAULT_LOG_FILE.c_str(), severity, aiDefaultLogStream_FILE);
}

//////////////////////////////////////////////////////////////////////////
void ModelImporter::Dispose()
{
	for (unsigned int i = 0; i < s_mManagedMaterials.size(); i++)
	{
		delete s_mManagedMaterials[i];
	}
	s_mManagedMaterials.clear();

	for (unsigned int i = 0; i < s_mManagedTextures.size(); i++)
	{
		delete s_mManagedTextures[i];
	}
	s_mManagedTextures.clear();

	for (unsigned int i = 0; i < s_mManagedMeshes.size(); i++)
	{
		delete s_mManagedMeshes[i];
	}
	s_mManagedMeshes.clear();

	for (unsigned int i = 0; i < s_mImportedScenes.size(); i++)
	{
		aiReleaseImport(s_mImportedScenes[i]);
	}
	s_mImportedScenes.clear();

	s_mInitialized = false;
}

//////////////////////////////////////////////////////////////////////////
unsigned int ModelImporter::GetAssimpImportSettings(unsigned int importSettings)
{
	if (importSettings & IS_QUALITY_POOR)
	{
		return aiProcessPreset_TargetRealtime_Fast;
	}
	else if (importSettings & IS_QUALITY_GOOD)
	{
		return aiProcessPreset_TargetRealtime_Quality;
	}
	else if (importSettings & IS_QUALITY_BEST)
	{
		return aiProcessPreset_TargetRealtime_MaxQuality;
	}
	else
	{
		THROW_EXCEPTION(Exception, "Invalid import settings");
	}
}

//////////////////////////////////////////////////////////////////////////
GameObject* ModelImporter::Import(const std::string& rFileName, unsigned int importSettings)
{
	if (CheckOptimizedModelFile(rFileName))
	{
		return ImportOptimizedModelFile(rFileName);
	}
	else
	{
		return ImportFromRegularFile(rFileName, importSettings);
	}
}

//////////////////////////////////////////////////////////////////////////
void ModelImporter::BuildMaterialCatalog(const aiScene* pScene, const std::string& rBaseDirectory, std::map<unsigned int, Material*>& rMaterialCatalog)
{
	if (pScene->HasTextures())
	{
		THROW_EXCEPTION(Exception, "Meshes with embedded textures are not supported yet");
	}

	unsigned int max;
	for (unsigned int i = 0; i < pScene->mNumMaterials; i++)
	{
		aiMaterial* pAIMaterial = pScene->mMaterials[i];
		unsigned int materialAttributesMask = 0;

		aiColor4D aiAmbientColor;
		glm::vec4 ambientColor;
		if (aiGetMaterialColor(pAIMaterial, AI_MATKEY_COLOR_AMBIENT, &aiAmbientColor) == AI_SUCCESS)
		{
			ambientColor = AssimpUtils::Convert(aiAmbientColor);
			materialAttributesMask |= MA_HAS_AMBIENT_COLOR;
		}

		aiColor4D aiDiffuseColor;
		glm::vec4 diffuseColor;
		if (aiGetMaterialColor(pAIMaterial, AI_MATKEY_COLOR_DIFFUSE, &aiDiffuseColor) == AI_SUCCESS) 
		{
			diffuseColor = AssimpUtils::Convert(aiDiffuseColor);
			materialAttributesMask |= MA_HAS_DIFFUSE_COLOR;
		}

		aiColor4D aiSpecularColor;
		glm::vec4 specularColor;
		if (aiGetMaterialColor(pAIMaterial, AI_MATKEY_COLOR_SPECULAR, &aiSpecularColor) == AI_SUCCESS)
		{
			specularColor = AssimpUtils::Convert(aiSpecularColor);
			materialAttributesMask |= MA_HAS_SPECULAR_COLOR;
		}

		aiColor4D aiEmissiveColor;
		glm::vec4 emissiveColor;
		if (aiGetMaterialColor(pAIMaterial, AI_MATKEY_COLOR_EMISSIVE, &aiEmissiveColor) == AI_SUCCESS)
		{
			emissiveColor = AssimpUtils::Convert(aiEmissiveColor);
			materialAttributesMask |= MA_IS_EMISSIVE;
		}

		float shininess = 0;
		if (aiGetMaterialFloatArray(pAIMaterial, AI_MATKEY_SHININESS, &shininess, &max) == AI_SUCCESS)
		{
			shininess /= specularColor.length();
			materialAttributesMask |= MA_HAS_SHININESS;
		}

		aiString texturePath;
		Texture* pColorMapTexture = 0;
		if (pAIMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &texturePath) == aiReturn_SUCCESS)
		{
			pColorMapTexture = TextureImporter::Import(rBaseDirectory + "/" + AssimpUtils::Convert(texturePath));
			materialAttributesMask |= MA_HAS_COLOR_MAP;
			s_mManagedTextures.push_back(pColorMapTexture);
		}

		Texture* pBumpMapTexture = 0;
		if (pAIMaterial->GetTexture(aiTextureType_NORMALS, 0, &texturePath) == aiReturn_SUCCESS ||
			pAIMaterial->GetTexture(aiTextureType_HEIGHT, 0, &texturePath) == aiReturn_SUCCESS)
		{
			pBumpMapTexture = TextureImporter::Import(rBaseDirectory + "/" + AssimpUtils::Convert(texturePath));
			materialAttributesMask |= MA_HAS_BUMP_MAP;
			s_mManagedTextures.push_back(pBumpMapTexture);
		}

		int twoSided = 0;
		if (aiGetMaterialIntegerArray(pAIMaterial, AI_MATKEY_TWOSIDED, &twoSided, &max) == AI_SUCCESS)
		{
			materialAttributesMask |= MA_IS_TWO_SIDED;
		}

		Shader* pShader;

		if (materialAttributesMask & MA_HAS_BUMP_MAP)
		{
			if (materialAttributesMask & MA_HAS_SHININESS)
			{
				pShader = ShaderRegistry::Find("BumpedSpecular");
			}
			else
			{
				pShader = ShaderRegistry::Find("BumpedDiffuse");
			}
		}
		else if (materialAttributesMask & MA_HAS_COLOR_MAP)
		{
			if (materialAttributesMask & MA_HAS_SHININESS)
			{
				pShader = ShaderRegistry::Find("Specular");
			}
			else
			{
				pShader = ShaderRegistry::Find("Diffuse");
			}
		}
		else
		{
			pShader = ShaderRegistry::Find("SolidColor");
		}

		Material* pManagedMaterial = new Material(pShader);
		if (materialAttributesMask & MA_HAS_AMBIENT_COLOR)
		{
			pManagedMaterial->SetVec4("ambientColor", ambientColor);
		}
		else
		{
			pManagedMaterial->SetVec4("ambientColor", Colors::BLACK);
		}

		if (materialAttributesMask & MA_HAS_DIFFUSE_COLOR)
		{
			pManagedMaterial->SetVec4("diffuseColor", diffuseColor);
		}
		else
		{
			pManagedMaterial->SetVec4("diffuseColor", Colors::BLACK);
		}

		if (materialAttributesMask & MA_HAS_SPECULAR_COLOR)
		{
			pManagedMaterial->SetVec4("specularColor", specularColor);
		}
		else
		{
			pManagedMaterial->SetVec4("specularColor", Colors::BLACK);
		}

		if (materialAttributesMask & MA_IS_EMISSIVE)
		{
			pManagedMaterial->SetVec4("emissiveColor", emissiveColor);
		}

		if (materialAttributesMask & MA_HAS_SHININESS)
		{
			pManagedMaterial->SetFloat("shininess", shininess);
		}
		else
		{
			pManagedMaterial->SetFloat("shininess", 0.0f);
		}

		if (materialAttributesMask & MA_HAS_COLOR_MAP)
		{
			pManagedMaterial->SetTexture("colorMap", pColorMapTexture);
		}

		if (materialAttributesMask & MA_HAS_BUMP_MAP)
		{
			pManagedMaterial->SetTexture("bumpMap", pBumpMapTexture);
		}

		pManagedMaterial->SetTwoSided(twoSided != 0);

		rMaterialCatalog.insert(std::make_pair(i, pManagedMaterial));
		s_mManagedMaterials.push_back(pManagedMaterial);
	}
}

//////////////////////////////////////////////////////////////////////////
void ModelImporter::BuildMeshCatalog(const aiScene* pScene, std::map<unsigned int, Mesh*>& rMeshCatalog, unsigned int importSettings)
{
	for (unsigned int i = 0; i < pScene->mNumMeshes; i++)
	{
		aiMesh* pAIMesh = pScene->mMeshes[i];
		
		std::vector<glm::vec3> vertices;
		std::vector<glm::vec3> normals;
		std::vector<glm::vec2> uvs;
		std::vector<unsigned int> indices;

		bool hasUV = pAIMesh->HasTextureCoords(0);
		bool invertV = ((importSettings & IS_INVERT_TEXTURE_COORDINATE_V) != 0);

		unsigned int materialIndex = pAIMesh->mMaterialIndex;
		aiMaterial* pAIMaterial = pScene->mMaterials[materialIndex];

		std::map<unsigned int, unsigned int> localIndexMap;
		int nextLocalIndex = 0;
		for (unsigned int j = 0; j < pAIMesh->mNumFaces; j++)
		{
			const aiFace* pFace = &pAIMesh->mFaces[j];

			if (pFace->mNumIndices < 3)
			{
				// FIXME: ignoring points and lines
				continue;
			}
			else if (pFace->mNumIndices > 3)
			{
				THROW_EXCEPTION(Exception, "rFace.mNumIndices > 3");
			}

			for (unsigned int k = 0; k < pFace->mNumIndices; k++)
			{
				int globalIndex = pFace->mIndices[k];
				std::map<unsigned int, unsigned int>::iterator it = localIndexMap.find(globalIndex);
				if (it == localIndexMap.end())
				{
					vertices.push_back(AssimpUtils::Convert(pAIMesh->mVertices[globalIndex]));
					normals.push_back(AssimpUtils::Convert(pAIMesh->mNormals[globalIndex]));

					if (hasUV)
					{
						// TODO: implement multi texturing support
						aiVector3D& rUV = pAIMesh->mTextureCoords[0][globalIndex];
						uvs.push_back(glm::vec2(rUV[0], (invertV) ? 1 - rUV[1] : rUV[1]));
					}

					indices.push_back(nextLocalIndex);
					localIndexMap.insert(std::make_pair(globalIndex, nextLocalIndex++));
				}
				else
				{
					indices.push_back(localIndexMap[globalIndex]);
				}
			}
		}

		Mesh* pManagedMesh = new Mesh(vertices, indices, normals, uvs);
		pManagedMesh->CalculateTangents();

		rMeshCatalog.insert(std::make_pair(i, pManagedMesh));
		s_mManagedMeshes.push_back(pManagedMesh);
	}
}

//////////////////////////////////////////////////////////////////////////
AABB ModelImporter::CalculateBoundingVolume(const aiScene* pScene)
{
	glm::mat4 transform;

	AABB boundingVolume;
	boundingVolume.min.x = boundingVolume.min.y = boundingVolume.min.z =  1e10f;
	boundingVolume.max.x = boundingVolume.max.y = boundingVolume.max.z = -1e10f;

	CalculateBoundingVolumeRecursively(pScene, pScene->mRootNode, transform, boundingVolume);

	boundingVolume.center.x = (boundingVolume.min.x + boundingVolume.max.x) / 2.0f;
	boundingVolume.center.y = (boundingVolume.min.y + boundingVolume.max.y) / 2.0f;
	boundingVolume.center.z = (boundingVolume.min.z + boundingVolume.max.z) / 2.0f;

	return boundingVolume;
}

//////////////////////////////////////////////////////////////////////////
AABB ModelImporter::CalculateBoundingVolume(const std::vector<glm::vec3>& rVertices)
{
	AABB boundingVolume;
	boundingVolume.min.x = boundingVolume.min.y = boundingVolume.min.z =  1e10f;
	boundingVolume.max.x = boundingVolume.max.y = boundingVolume.max.z = -1e10f;

	for (unsigned int i = 0; i < rVertices.size(); i++)
	{
		const glm::vec3& rVertex = rVertices[i];
		boundingVolume.min.x = MathF::Min(boundingVolume.min.x, rVertex.x);
		boundingVolume.min.y = MathF::Min(boundingVolume.min.y, rVertex.y);
		boundingVolume.min.z = MathF::Min(boundingVolume.min.z, rVertex.z);
		boundingVolume.max.x = MathF::Max(boundingVolume.max.x, rVertex.x);
		boundingVolume.max.y = MathF::Max(boundingVolume.max.y, rVertex.y);
		boundingVolume.max.z = MathF::Max(boundingVolume.max.z, rVertex.z);
	}

	boundingVolume.center.x = (boundingVolume.min.x + boundingVolume.max.x) / 2.0f;
	boundingVolume.center.y = (boundingVolume.min.y + boundingVolume.max.y) / 2.0f;
	boundingVolume.center.z = (boundingVolume.min.z + boundingVolume.max.z) / 2.0f;

	return boundingVolume;
}

//////////////////////////////////////////////////////////////////////////
void ModelImporter::CalculateBoundingVolumeRecursively(const aiScene* pScene, aiNode* pNode, const glm::mat4& rParentTransform, AABB& boundingVolume)
{
	glm::mat4 transform = rParentTransform * AssimpUtils::Convert(pNode->mTransformation);

	for (unsigned int i = 0; i < pNode->mNumMeshes; i++)
	{
		const aiMesh* pMesh = pScene->mMeshes[pNode->mMeshes[i]];

		for (unsigned int j = 0; j < pMesh->mNumVertices; ++j)
		{
			glm::vec4 vertex = transform * glm::vec4(AssimpUtils::Convert(pMesh->mVertices[j]), 1.0f);

			boundingVolume.min.x = MathF::Min(boundingVolume.min.x, vertex.x);
			boundingVolume.min.y = MathF::Min(boundingVolume.min.y, vertex.y);
			boundingVolume.min.z = MathF::Min(boundingVolume.min.z, vertex.z);
			boundingVolume.max.x = MathF::Max(boundingVolume.max.x, vertex.x);
			boundingVolume.max.y = MathF::Max(boundingVolume.max.y, vertex.y);
			boundingVolume.max.z = MathF::Max(boundingVolume.max.z, vertex.z);
		}
	}

	for (unsigned int i = 0; i < pNode->mNumChildren; i++)
	{
		CalculateBoundingVolumeRecursively(pScene, pNode->mChildren[i], transform, boundingVolume);
	}
}

//////////////////////////////////////////////////////////////////////////
GameObject* ModelImporter::BuildGameObject(const std::string& rModelName, const aiScene* pScene, std::map<unsigned int, Material*>& rMaterialCatalog, std::map<unsigned int, Mesh*>& rMeshCatalog)
{
	GameObject* pGameObject = GameObject::Instantiate(rModelName);
	BuildGameObjectRecursively(pScene, rMaterialCatalog, rMeshCatalog, pScene->mRootNode, pGameObject);
	return pGameObject;
}

//////////////////////////////////////////////////////////////////////////
void ModelImporter::BuildGameObjectRecursively(const aiScene* pScene, std::map<unsigned int, Material*>& rMaterialCatalog, std::map<unsigned int, Mesh*>& rMeshCatalog, aiNode* pNode, GameObject* pGameObject)
{
	Transform* pTransform = pGameObject->GetTransform();
	glm::mat4& rModel = AssimpUtils::Convert(pNode->mTransformation.Transpose());
	pTransform->SetPosition(glm::vec3(rModel[3]));
	pTransform->SetRotation(glm::toQuat(rModel));

	if (pNode->mNumMeshes > 0)
	{
		MeshRenderer* pMeshRenderer = MeshRenderer::Instantiate(pGameObject);
		MeshFilter* pMeshFilter = MeshFilter::Instantiate(pGameObject);

		// TODO: multiple materials support
		bool hasAddedMaterial = false;
		for (unsigned int i = 0; i < pNode->mNumMeshes; i++)
		{
			unsigned int meshIndex = pNode->mMeshes[i];
			aiMesh* pAIMesh = pScene->mMeshes[meshIndex];
			unsigned int materialIndex = pAIMesh->mMaterialIndex;

			Mesh* pMesh = rMeshCatalog[meshIndex];
			Material* pMaterial = rMaterialCatalog[materialIndex];

			pMeshRenderer->AddMesh(pMesh);

			if (!hasAddedMaterial)
			{
				pMeshFilter->SetMaterial(pMaterial);
				hasAddedMaterial = true;
			}
		}
	}

	for (unsigned int i = 0; i < pNode->mNumChildren; i++)
	{
		GameObject* pChild = GameObject::Instantiate();
		pChild->GetTransform()->SetParent(pTransform);
		BuildGameObjectRecursively(pScene, rMaterialCatalog, rMeshCatalog, pNode->mChildren[i], pChild);
	}
}

//////////////////////////////////////////////////////////////////////////
GameObject* ModelImporter::ImportFromRegularFile(const std::string &rFileName, unsigned int importSettings)
{
	const aiScene* pScene = aiImportFile(rFileName.c_str(), GetAssimpImportSettings(importSettings));

	if (pScene == 0)
	{
		THROW_EXCEPTION(Exception, "Failed to import model: %s", rFileName.c_str());
	}

	s_mImportedScenes.push_back(pScene);

	std::string baseDirectory = File::GetFilePath(rFileName);

	std::map<unsigned int, Material*> materialCatalog;
	std::map<unsigned int, Mesh*> meshCatalog;

	BuildMaterialCatalog(pScene, baseDirectory, materialCatalog);
	BuildMeshCatalog(pScene, meshCatalog, importSettings);

	std::string modelName = File::GetFileNameWithoutExtension(rFileName);

	GameObject* pGameObject = BuildGameObject(modelName, pScene, materialCatalog, meshCatalog);
	pGameObject->SetBoundingVolume(CalculateBoundingVolume(pScene));

	if (importSettings & IS_GENERATE_OPTIMIZED_MODEL_FILE)
	{
		GenerateOptimizedModelFile(rFileName, pGameObject);
	}

	return pGameObject;
}

//////////////////////////////////////////////////////////////////////////
const bool ModelImporter::CheckOptimizedModelFile(const std::string& rFileName)
{
	return File::Exists(File::GetFileNameWithoutExtension(rFileName) + ".model");
}

//////////////////////////////////////////////////////////////////////////
GameObject* ModelImporter::ImportOptimizedModelFile(const std::string& rFileName)
{
	std::string optimizedModelFileName = File::GetFileNameWithoutExtension(rFileName) + ".model";

	FILE* pFile = fopen(optimizedModelFileName.c_str(), "rb");

	//int fileSize = ftell(pFile);

	OptimizedModelHeader header;
	
	unsigned int itemsRead = fread((void*)&header, sizeof(OptimizedModelHeader), 1, pFile);

	if (itemsRead != 1)
	{
		THROW_EXCEPTION(Exception, "Error reading optimized model header from file: %s", optimizedModelFileName.c_str());
	}

	unsigned int itemsToRead = header.numVertices * 3;
	float* pVerticesBuffer = new float[itemsToRead];
	itemsRead = fread((void*)pVerticesBuffer, sizeof(float), itemsToRead, pFile);

	if (itemsRead != itemsToRead)
	{
		THROW_EXCEPTION(Exception, "Error reading vertices from optimized model file: %s", optimizedModelFileName.c_str());
	}

	std::vector<glm::vec3> vertices;
	vertices.resize(header.numVertices);
	for (unsigned int i = 0, j = 0; i < header.numVertices; i++, j += 3)
	{
		vertices[i] = glm::vec3(pVerticesBuffer[j], pVerticesBuffer[j + 1], pVerticesBuffer[j + 2]);
	}

	delete[] pVerticesBuffer;

	itemsToRead = header.numNormals * 3;
	float* pNormalsBuffer = new float[itemsToRead];
	itemsRead = fread((void*)pNormalsBuffer, sizeof(float), itemsToRead, pFile);

	if (itemsRead != itemsToRead)
	{
		THROW_EXCEPTION(Exception, "Error reading normals from optimized model file: %s", optimizedModelFileName.c_str());
	}

	std::vector<glm::vec3> normals;
	normals.resize(header.numNormals);
	for (unsigned int i = 0, j = 0; i < header.numNormals; i++, j += 3)
	{
		normals[i] = glm::vec3(pNormalsBuffer[j], pNormalsBuffer[j + 1], pNormalsBuffer[j + 2]);
	}

	delete[] pNormalsBuffer;

	itemsToRead = header.numUvs * 2;
	float* pUVsBuffer = new float[itemsToRead];
	itemsRead = fread((void*)pUVsBuffer, sizeof(float), itemsToRead, pFile);

	if (itemsRead != itemsRead)
	{
		THROW_EXCEPTION(Exception, "Error reading uvs from optimized model file: %s", optimizedModelFileName.c_str());
	}

	std::vector<glm::vec2> uvs;
	uvs.resize(header.numUvs);
	for (unsigned int i = 0, j = 0; i < header.numUvs; i++, j += 2)
	{
		uvs[i] = glm::vec2(pUVsBuffer[j], pUVsBuffer[j + 1]);
	}

	delete[] pUVsBuffer;

	unsigned int* pIndicesBuffer = new unsigned int[header.numIndices];
	itemsRead = fread((void*)pIndicesBuffer, sizeof(unsigned int), header.numIndices, pFile);

	if (itemsRead != header.numIndices)
	{
		THROW_EXCEPTION(Exception, "Error reading indices from optimized model file: %s", optimizedModelFileName.c_str());
	}

	std::vector<unsigned int> indices;
	indices.resize(header.numIndices);
	for (unsigned int i = 0; i < header.numIndices; i++)
	{
		indices[i] = pIndicesBuffer[i];
	}

	delete[] pIndicesBuffer;

	fclose(pFile);

	Mesh* pMesh = new Mesh(vertices, indices, normals, uvs);
	s_mManagedMeshes.push_back(pMesh);

	Material* pMaterial = new Material(ShaderRegistry::Find("SolidColor"));
	pMaterial->SetVec4("diffuseColor", Colors::WHITE);
	pMaterial->SetVec4("specularColor", Colors::BLACK);
	pMaterial->SetFloat("shininess", 0);
	s_mManagedMaterials.push_back(pMaterial);

	std::string modelName = File::GetFileNameWithoutExtension(rFileName);

	GameObject* pGameObject = GameObject::Instantiate(modelName);

	MeshRenderer* pMeshRenderer = MeshRenderer::Instantiate(pGameObject);
	MeshFilter* pMeshFilter = MeshFilter::Instantiate(pGameObject);

	pMeshFilter->SetMaterial(pMaterial);
	pMeshRenderer->AddMesh(pMesh);

	pGameObject->SetBoundingVolume(CalculateBoundingVolume(vertices));

	return pGameObject;
}

//////////////////////////////////////////////////////////////////////////
void ModelImporter::GenerateOptimizedModelFile(const std::string& rFileName, const GameObject* pGameObject)
{
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> uvs;
	std::vector<unsigned int> indices;

	CombineMeshes(pGameObject, vertices, normals, uvs, indices);

	OptimizedModelHeader header;
	header.numVertices = vertices.size();
	header.numNormals = normals.size();
	header.numUvs = uvs.size();
	header.numIndices = indices.size();

	float* pVerticesBuffer = new float[header.numVertices * 3];
	float* pNormalsBuffer = new float[header.numNormals * 3];
	float* pUVsBuffer = new float[header.numUvs * 2];
	unsigned int* pIndicesBuffer = new unsigned int[header.numIndices];

	for (unsigned int i = 0, j = 0; i < vertices.size(); i++, j += 3)
	{
		glm::vec3& rVertex = vertices[i];
		pVerticesBuffer[j] = rVertex.x;
		pVerticesBuffer[j + 1] = rVertex.y;
		pVerticesBuffer[j + 2] = rVertex.z;
	}

	for (unsigned int i = 0, j = 0; i < normals.size(); i++, j += 3)
	{
		glm::vec3& rNormal = normals[i];
		pNormalsBuffer[j] = rNormal.x;
		pNormalsBuffer[j + 1] = rNormal.y;
		pNormalsBuffer[j + 2] = rNormal.z;
	}

	for (unsigned int i = 0, j = 0; i < uvs.size(); i++, j += 2)
	{
		glm::vec2& rUV = uvs[i];
		pUVsBuffer[j] = rUV.x;
		pUVsBuffer[j + 1] = rUV.y;
	}

	for (unsigned int i = 0; i < indices.size(); i++)
	{
		pIndicesBuffer[i] = indices[i];
	}

	std::string optimizedModelFileName = File::GetFileNameWithoutExtension(rFileName) + ".model";

	FILE* pFile = fopen(optimizedModelFileName.c_str(), "wb");

	unsigned int itemsWritten = fwrite((const void*)&header, sizeof(OptimizedModelHeader), 1, pFile);

	if (itemsWritten != 1)
	{
		THROW_EXCEPTION(Exception, "Error writing header to optimized model: %s", optimizedModelFileName.c_str());
	}

	unsigned int itemsToWrite = vertices.size() * 3;
	itemsWritten = fwrite((const void*)pVerticesBuffer, sizeof(float), vertices.size() * 3, pFile);

	if (itemsWritten != itemsToWrite)
	{
		THROW_EXCEPTION(Exception,  "Error writing vertices to optimized model: %s", optimizedModelFileName.c_str());
	}

	delete[] pVerticesBuffer;

	itemsToWrite = normals.size() * 3;
	itemsWritten = fwrite((const void*)pNormalsBuffer, sizeof(float), normals.size() * 3, pFile);

	if (itemsWritten != itemsToWrite)
	{
		THROW_EXCEPTION(Exception, "Error writing normals to optimized model: %s", optimizedModelFileName.c_str());
	}

	delete[] pNormalsBuffer;

	itemsToWrite = uvs.size() * 2;
	itemsWritten = fwrite((const void*)pUVsBuffer, sizeof(float), uvs.size() * 2, pFile);

	if (itemsWritten != itemsToWrite)
	{
		THROW_EXCEPTION(Exception, "Error writing uvs to optimized model: %s", optimizedModelFileName.c_str());
	}

	delete[] pUVsBuffer;

	itemsToWrite = indices.size();
	itemsWritten = fwrite((const void*)pIndicesBuffer, sizeof(unsigned int), indices.size(), pFile);

	if (itemsWritten != itemsToWrite)
	{
		THROW_EXCEPTION(Exception, "Error writing indices to optimized model: %s", optimizedModelFileName.c_str());
	}

	delete[] pIndicesBuffer;

	fclose(pFile);
}

//////////////////////////////////////////////////////////////////////////
void ModelImporter::CombineMeshes(const GameObject* pGameObject, std::vector<glm::vec3>& rVertices, std::vector<glm::vec3>& rNormals, std::vector<glm::vec2>& rUVs, std::vector<unsigned int>& rIndices)
{
	MeshRenderer* pMeshRenderer = dynamic_cast<MeshRenderer*>(pGameObject->GetComponent(MeshRenderer::TYPE));

	if (pMeshRenderer != 0)
	{
		glm::mat4& rModel = pGameObject->GetTransform()->GetModel();

		std::vector<Mesh*>& rMeshes = pMeshRenderer->GetMeshes();
		for (unsigned int i = 0; i < rMeshes.size(); i++)
		{
			Mesh* pMesh = rMeshes[i];

			unsigned int indicesOffset = rVertices.size();
			const std::vector<glm::vec3>& rMeshVertices = pMesh->GetVertices();
			for (unsigned int j = 0; j < rMeshVertices.size(); j++)
			{
				rVertices.push_back(glm::vec3(rModel * glm::vec4(rMeshVertices[j], 1.0f)));
			}

			const std::vector<glm::vec3>& rMeshNormals = pMesh->GetNormals();
			rNormals.insert(rNormals.end(), rMeshNormals.begin(), rMeshNormals.end());

			const std::vector<glm::vec2>& rMeshUVs = pMesh->GetUVs();
			rUVs.insert(rUVs.end(), rMeshUVs.begin(), rMeshUVs.end());

			const std::vector<unsigned int>& rMeshIndices = pMesh->GetIndices();
			for (unsigned int j = 0; j < rMeshIndices.size(); j++)
			{
				rIndices.push_back(indicesOffset + rMeshIndices[j]);
			}
		}
	}

	const std::vector<Transform*>& rChildren = pGameObject->GetTransform()->GetChildren();
	for (unsigned int i = 0; i < rChildren.size(); i++)
	{
		CombineMeshes(rChildren[i]->GetGameObject(), rVertices, rNormals, rUVs, rIndices);
	}
}
