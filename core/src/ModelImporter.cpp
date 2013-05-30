#include <ModelImporter.h>
#include <AssimpUtils.h>
#include <Transform.h>
#include <MeshRenderer.h>
#include <MeshFilter.h>
#include <ShaderRegistry.h>
#include <TextureImporter.h>
#include <Texture.h>
#include <File.h>
#include <MathT.h>
#include <Exception.h>

#include <assimp/postprocess.h>

bool ModelImporter::s_mInitialized = false;
std::vector<Texture*> ModelImporter::s_mManagedTextures;
std::vector<Material*> ModelImporter::s_mManagedMaterials;
std::vector<Mesh*> ModelImporter::s_mManagedMeshes;
std::vector<const aiScene*> ModelImporter::s_mImportedScenes;

void ModelImporter::Initialize()
{
	if (s_mInitialized)
	{
		// FIXME: checking invariants
		THROW_EXCEPTION(Exception, "s_mInitialized");
	}

	s_mInitialized = true;
}

void ModelImporter::Dispose()
{
	if (!s_mInitialized)
	{
		// FIXME: checking invariants
		THROW_EXCEPTION(Exception, "!s_mInitialized");
	}

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

GameObject* ModelImporter::Import(const std::string& rFileName)
{
	const aiScene* pScene = aiImportFile(rFileName.c_str(), aiProcessPreset_TargetRealtime_Quality);

	if (pScene == 0)
	{
		THROW_EXCEPTION(Exception, "Failed to import model: %s", rFileName.c_str());
	}

	s_mImportedScenes.push_back(pScene);

	std::string baseDirectory = File::GetFilePath(rFileName);

	std::map<unsigned int, Material*> materialCatalog;
	std::map<unsigned int, Mesh*> meshCatalog;

	BuildMaterialCatalog(pScene, baseDirectory, materialCatalog);
	BuildMeshCatalog(pScene, meshCatalog);

	GameObject* pGameObject = BuildGameObject(pScene, materialCatalog, meshCatalog);
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
#ifdef FIXED_FUNCTION_PIPELINE
		Material* pManagedMaterial = new Material();
#else
		Material* pManagedMaterial = new Material(ShaderRegistry::Find("Specular"));
#endif

		aiColor4D aiAmbientColor;
		if (aiGetMaterialColor(pAIMaterial, AI_MATKEY_COLOR_AMBIENT, &aiAmbientColor) == AI_SUCCESS)
		{
			glm::vec4 ambientColor = AssimpUtils::Convert(aiAmbientColor);
#ifdef FIXED_FUNCTION_PIPELINE
			pManagedMaterial->SetAmbientColor(ambientColor);
#else
			pManagedMaterial->SetVec4("ambientColor", ambientColor);
#endif
		}

		aiColor4D aiDiffuseColor;
		if (aiGetMaterialColor(pAIMaterial, AI_MATKEY_COLOR_DIFFUSE, &aiDiffuseColor) == AI_SUCCESS) {
			glm::vec4 diffuseColor = AssimpUtils::Convert(aiDiffuseColor);
#ifdef FIXED_FUNCTION_PIPELINE
			pManagedMaterial->SetDiffuseColor(diffuseColor);
#else
			pManagedMaterial->SetVec4("diffuseColor", diffuseColor);
#endif
		}

		aiColor4D aiSpecularColor;
		if (aiGetMaterialColor(pAIMaterial, AI_MATKEY_COLOR_SPECULAR, &aiSpecularColor) == AI_SUCCESS)
		{
			glm::vec4 specularColor = AssimpUtils::Convert(aiSpecularColor);
#ifdef FIXED_FUNCTION_PIPELINE
			pManagedMaterial->SetSpecularColor(specularColor);
#else
			pManagedMaterial->SetVec4("specularColor", specularColor);
#endif
		}

		aiColor4D aiEmissiveColor;
		if (aiGetMaterialColor(pAIMaterial, AI_MATKEY_COLOR_EMISSIVE, &aiEmissiveColor) == AI_SUCCESS)
		{
			glm::vec4 emissiveColor = AssimpUtils::Convert(aiEmissiveColor);
#ifdef FIXED_FUNCTION_PIPELINE
			pManagedMaterial->SetEmissiveColor(emissiveColor);
#else
			pManagedMaterial->SetVec4("emissiveColor", emissiveColor);
#endif
		}

		float shininess = 0;
		float strength = 0;
		if (aiGetMaterialFloatArray(pAIMaterial, AI_MATKEY_SHININESS, &shininess, &max) == AI_SUCCESS &&
			aiGetMaterialFloatArray(pAIMaterial, AI_MATKEY_SHININESS_STRENGTH, &strength, &max) == AI_SUCCESS)
		{
			shininess *= strength;
#ifdef FIXED_FUNCTION_PIPELINE
			pManagedMaterial->SetShininess(shininess);
#else
			pManagedMaterial->SetFloat("shininess", shininess);
#endif
		}

		Texture* pImportedTexture;
		aiString texturePath;
		if (pAIMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &texturePath) == aiReturn_SUCCESS)
		{
			pImportedTexture = TextureImporter::Import(rBaseDirectory + "/" + AssimpUtils::Convert(texturePath));
#ifdef FIXED_FUNCTION_PIPELINE
			pManagedMaterial->SetTexture(pImportedTexture);
#else
			pManagedMaterial->SetTexture("colorMap", pImportedTexture);
#endif
			s_mManagedTextures.push_back(pImportedTexture);
		}

		int twoSided;
		if (aiGetMaterialIntegerArray(pAIMaterial, AI_MATKEY_TWOSIDED, &twoSided, &max) == AI_SUCCESS)
		{
			pManagedMaterial->SetTwoSided(twoSided != 0);
		}

		rMaterialCatalog.insert(std::make_pair(i, pManagedMaterial));
		s_mManagedMaterials.push_back(pManagedMaterial);
	}
}

void ModelImporter::BuildMeshCatalog(const aiScene* pScene, std::map<unsigned int, Mesh*>& rMeshCatalog)
{
	for (unsigned int i = 0; i < pScene->mNumMeshes; i++)
	{
		aiMesh* pAIMesh = pScene->mMeshes[i];
		
		std::vector<glm::vec3> vertices;
		std::vector<glm::vec3> normals;
		std::vector<glm::vec2> uvs;
		std::vector<unsigned int> indices;
/*#ifndef FIXED_FUNCTION_PIPELINE
		std::vector<glm::vec4> tangents;
#endif*/

		bool hasUV = pAIMesh->HasTextureCoords(0);

		// TODO: improve vertex index usage!
		int c = 0;
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

			int i0 = pFace->mIndices[0];
			int i1 = pFace->mIndices[1];
			int i2 = pFace->mIndices[2];

			indices.push_back(c++);
			indices.push_back(c++);
			indices.push_back(c++);

			vertices.push_back(AssimpUtils::Convert(pAIMesh->mVertices[i0]));
			vertices.push_back(AssimpUtils::Convert(pAIMesh->mVertices[i1]));
			vertices.push_back(AssimpUtils::Convert(pAIMesh->mVertices[i2]));

			normals.push_back(AssimpUtils::Convert(pAIMesh->mNormals[i0]));
			normals.push_back(AssimpUtils::Convert(pAIMesh->mNormals[i1]));
			normals.push_back(AssimpUtils::Convert(pAIMesh->mNormals[i2]));

			if (hasUV)
			{
				// TODO: implement multi texturing support

				aiVector3D& rUV1 = pAIMesh->mTextureCoords[0][i0];
				uvs.push_back(glm::vec2(rUV1[0], 1 - rUV1[1]));

				aiVector3D& rUV2 = pAIMesh->mTextureCoords[0][i1];
				uvs.push_back(glm::vec2(rUV2[0], 1 - rUV2[1]));

				aiVector3D& rUV3 = pAIMesh->mTextureCoords[0][i2];
				uvs.push_back(glm::vec2(rUV3[0], 1 - rUV3[1]));
			}
		}

//#ifndef FIXED_FUNCTION_PIPELINE
		//Mesh* pMesh = new Mesh(vertices, indices, normals, tangents, uvs);
//#else
		Mesh* pManagedMesh = new Mesh(vertices, indices, normals, uvs);
//#endif

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

// DEBUG:
/*GameObject* ModelImporter::BuildGameObject(const aiScene* pScene, std::map<unsigned int, Material*>& rMaterialCatalog, std::map<unsigned int, Mesh*>& rMeshCatalog)
{
	GameObject* pGameObject = new GameObject();
	BuildGameObjectRecursively(pScene, rMaterialCatalog, rMeshCatalog, pScene->mRootNode, pGameObject);
	return pGameObject;
}*/

// DEBUG:
/*void ModelImporter::BuildGameObjectRecursively(const aiScene* pScene, std::map<unsigned int, Material*>& rMaterialCatalog, std::map<unsigned int, Mesh*>& rMeshCatalog, aiNode* pNode, GameObject* pRenderNode)
{
	pRenderNode->transform = AssimpUtils::Convert(pNode->mTransformation.Transpose());

	for (unsigned int i = 0; i < pNode->mNumMeshes; i++)
	{
		unsigned int meshIndex = pNode->mMeshes[i];
		const aiMesh* pImportedMesh = pScene->mMeshes[meshIndex];
		unsigned int materialIndex = pImportedMesh->mMaterialIndex;

		Mesh* pMesh = rMeshCatalog[meshIndex];
		Material* pMaterial = rMaterialCatalog[materialIndex];

		pRenderNode->meshes.push_back(pMesh);
		pRenderNode->materials.insert(std::make_pair(pMesh, pMaterial));
	}

	for (unsigned int i = 0; i < pNode->mNumChildren; i++)
	{
		aiNode* pChildNode = pNode->mChildren[i];
		GameObject* pChild = new GameObject();
		BuildGameObjectRecursively(pScene, rMaterialCatalog, rMeshCatalog, pChildNode, pChild);
		pRenderNode->children.push_back(pChild);
	}
}*/

GameObject* ModelImporter::BuildGameObject(const aiScene* pScene, std::map<unsigned int, Material*>& rMaterialCatalog, std::map<unsigned int, Mesh*>& rMeshCatalog)
{
	GameObject* pGameObject = GameObject::Instantiate();
	BuildGameObjectRecursively(pScene, rMaterialCatalog, rMeshCatalog, pScene->mRootNode, pGameObject);
	return pGameObject;
}

void ModelImporter::BuildGameObjectRecursively(const aiScene* pScene, std::map<unsigned int, Material*>& rMaterialCatalog, std::map<unsigned int, Mesh*>& rMeshCatalog, aiNode* pNode, GameObject* pGameObject)
{
	Transform* pTransform = pGameObject->GetTransform();
	pTransform->SetWorldTransform(AssimpUtils::Convert(pNode->mTransformation.Transpose()));

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

	for (unsigned int i = 0; i < pNode->mNumChildren; i++)
	{
		GameObject* pChild = GameObject::Instantiate();
		pChild->GetTransform()->SetParent(pTransform);
		BuildGameObjectRecursively(pScene, rMaterialCatalog, rMeshCatalog, pNode->mChildren[i], pChild);
	}
}