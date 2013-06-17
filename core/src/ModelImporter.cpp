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
#include <Exception.h>

#include <assimp/PostProcess.h>

#include <map>

const std::string ModelImporter::DEFAULT_LOG_FILE = "assimp.log";

bool ModelImporter::s_mInitialized = false;
std::vector<Texture*> ModelImporter::s_mManagedTextures;
std::vector<Material*> ModelImporter::s_mManagedMaterials;
std::vector<Mesh*> ModelImporter::s_mManagedMeshes;
std::vector<const aiScene*> ModelImporter::s_mImportedScenes;
Assimp::Logger* ModelImporter::s_mpCurrentLogger = 0;

void ModelImporter::Initialize()
{
	if (s_mInitialized)
	{
		// FIXME: checking invariants
		THROW_EXCEPTION(Exception, "s_mInitialized");
	}

	s_mInitialized = true;
}

void ModelImporter::LogToConsole()
{
	if (s_mpCurrentLogger != 0)
	{
		delete s_mpCurrentLogger;
		s_mpCurrentLogger = 0;
	}

	Assimp::Logger::LogSeverity severity = Assimp::Logger::VERBOSE;
	s_mpCurrentLogger = Assimp::DefaultLogger::create("", severity, aiDefaultLogStream_STDOUT);
}

void ModelImporter::LogToFile()
{
	if (s_mpCurrentLogger != 0)
	{
		delete s_mpCurrentLogger;
		s_mpCurrentLogger = 0;
	}

	Assimp::Logger::LogSeverity severity = Assimp::Logger::VERBOSE;
	s_mpCurrentLogger = Assimp::DefaultLogger::create(DEFAULT_LOG_FILE.c_str(), severity, aiDefaultLogStream_FILE);
}

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

GameObject* ModelImporter::Import(const std::string& rFileName, unsigned int importSettings)
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

	return pGameObject;
}

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

#ifdef FIXED_FUNCTION_PIPELINE
		Material* pManagedMaterial = new Material();
#else
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
#endif

		if (materialAttributesMask & MA_HAS_AMBIENT_COLOR)
		{
#ifdef FIXED_FUNCTION_PIPELINE
			pManagedMaterial->SetAmbientColor(ambientColor);
#else
			pManagedMaterial->SetVec4("ambientColor", ambientColor);
#endif
		}
		else
		{
			pManagedMaterial->SetVec4("ambientColor", Colors::BLACK);
		}

		if (materialAttributesMask & MA_HAS_DIFFUSE_COLOR)
		{
#ifdef FIXED_FUNCTION_PIPELINE
			pManagedMaterial->SetDiffuseColor(diffuseColor);
#else
			pManagedMaterial->SetVec4("diffuseColor", diffuseColor);
#endif
		}
		else
		{
			pManagedMaterial->SetVec4("diffuseColor", Colors::BLACK);
		}

		if (materialAttributesMask & MA_HAS_SPECULAR_COLOR)
		{
#ifdef FIXED_FUNCTION_PIPELINE
			pManagedMaterial->SetSpecularColor(specularColor);
#else
			pManagedMaterial->SetVec4("specularColor", specularColor);
#endif
		}
		else
		{
			pManagedMaterial->SetVec4("specularColor", Colors::BLACK);
		}

		if (materialAttributesMask & MA_IS_EMISSIVE)
		{
#ifdef FIXED_FUNCTION_PIPELINE
			pManagedMaterial->SetEmissiveColor(emissiveColor);
#else
			pManagedMaterial->SetVec4("emissiveColor", emissiveColor);
#endif
		}

		if (materialAttributesMask & MA_HAS_SHININESS)
		{
#ifdef FIXED_FUNCTION_PIPELINE
			pManagedMaterial->SetShininess(shininess);
#else
			pManagedMaterial->SetFloat("shininess", shininess);
#endif
		}
		else
		{
			pManagedMaterial->SetFloat("shininess", 0.0f);
		}

		if (materialAttributesMask & MA_HAS_COLOR_MAP)
		{
#ifdef FIXED_FUNCTION_PIPELINE
			pManagedMaterial->SetTexture(pColorMapTexture);
#else
			pManagedMaterial->SetTexture("colorMap", pColorMapTexture);
#endif
		}

		if (materialAttributesMask & MA_HAS_BUMP_MAP)
		{
#ifdef FIXED_FUNCTION_PIPELINE
			pManagedMaterial->SetTexture(pBumpMapTexture);
#else
			pManagedMaterial->SetTexture("bumpMap", pBumpMapTexture);
#endif
		}

		pManagedMaterial->SetTwoSided(twoSided != 0);

		rMaterialCatalog.insert(std::make_pair(i, pManagedMaterial));
		s_mManagedMaterials.push_back(pManagedMaterial);
	}
}

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
#ifndef FIXED_FUNCTION_PIPELINE
		pManagedMesh->CalculateTangents();
#endif

		rMeshCatalog.insert(std::make_pair(i, pManagedMesh));
		s_mManagedMeshes.push_back(pManagedMesh);
	}
}

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

GameObject* ModelImporter::BuildGameObject(const std::string& rModelName, const aiScene* pScene, std::map<unsigned int, Material*>& rMaterialCatalog, std::map<unsigned int, Mesh*>& rMeshCatalog)
{
	GameObject* pGameObject = GameObject::Instantiate(rModelName);
	BuildGameObjectRecursively(pScene, rMaterialCatalog, rMeshCatalog, pScene->mRootNode, pGameObject);
	return pGameObject;
}

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