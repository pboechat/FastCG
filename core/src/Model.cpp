#include <Model.h>
#include <Transform.h>
#include <MeshRenderer.h>
#include <MeshFilter.h>
#include <ShaderRegistry.h>
#include <Texture.h>
#include <TextureUtils.h>
#include <AssimpUtils.h>
#include <File.h>
#include <Exception.h>

#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <glm/glm.hpp>

Model::Model(const std::string& rFileName) :
	mFileName(rFileName),
	mpScene(0),
	mpRootGameObject(0)
{
}

Model::~Model()
{
	DeallocateResources();
}

void Model::AllocateResources()
{
	mpScene = aiImportFile(mFileName.c_str(), aiProcessPreset_TargetRealtime_Quality);

	if (mpScene == 0)
	{
		THROW_EXCEPTION(Exception, "Failed to import model: %s", mFileName.c_str());
	}

	BuildMaterialCatalog();
	BuildMeshCatalog();
	BuildRootGameObject();
}

void Model::DeallocateResources()
{
	if (mpScene != 0)
	{
		aiReleaseImport(mpScene);
	}

	std::map<unsigned int, Material*>::iterator it1 = mMaterialCatalog.begin();
	while (it1 != mMaterialCatalog.end())
	{
		delete it1->second;
		it1++;
	}
	mMaterialCatalog.clear();

	for (unsigned int i = 0; i < mImportedTextures.size(); i++)
	{
		delete mImportedTextures[i];
	}
	mImportedTextures.clear();

	std::map<unsigned int, Mesh*>::iterator it2 = mMeshCatalog.begin();
	while (it2 != mMeshCatalog.end())
	{
		delete it2->second;
		it2++;
	}
	mMeshCatalog.clear();
}

void Model::BuildMaterialCatalog()
{
	std::string filePath = File::GetPath(mFileName);

	for (unsigned int i = 0; i < mpScene->mNumMaterials; i++)
	{
		aiMaterial* pImportedMaterial = mpScene->mMaterials[i];
#ifdef USE_PROGRAMMABLE_PIPELINE
		Material* pMaterial = new Material(ShaderRegistry::Find("Diffuse"));
#else
		Material* pMaterial = new Material();
#endif

		Texture* pTexture;
		aiString path;
		if (pImportedMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &path) == aiReturn_SUCCESS)
		{
			pTexture = TextureUtils::LoadPNGAsTexture(filePath + AssimpUtils::Convert(path));
#ifdef USE_PROGRAMMABLE_PIPELINE
			pMaterial->SetTexture("colorMap", pTexture);
#else
			pMaterial->SetTexture(pTexture);
#endif
			mImportedTextures.push_back(pTexture);
		}
		
		// TODO:
		mMaterialCatalog.insert(std::make_pair(i, pMaterial));
	}
}

void Model::BuildMeshCatalog()
{
	for (unsigned int i = 0; i < mpScene->mNumMeshes; i++)
	{
		aiMesh* pImportedMesh = mpScene->mMeshes[i];
		
		std::vector<glm::vec3> vertices;
		std::vector<glm::vec3> normals;
		std::vector<glm::vec2> uvs;
		std::vector<unsigned int> indices;
#ifdef USE_PROGRAMMABLE_PIPELINE
		std::vector<glm::vec4> tangents;
#endif

		for (unsigned int j = 0; j < pImportedMesh->mNumFaces; j++)
		{
			aiFace& rFace = pImportedMesh->mFaces[j];

			if (rFace.mNumIndices < 3)
			{
				// FIXME: ignoring points and lines
				continue;
			}
			else if (rFace.mNumIndices > 3)
			{
				THROW_EXCEPTION(Exception, "rFace.mNumIndices > 3");
			}

			int i0 = rFace.mIndices[0];
			int i1 = rFace.mIndices[1];
			int i2 = rFace.mIndices[2];

			indices.push_back(i0);
			indices.push_back(i1);
			indices.push_back(i2);

			vertices.push_back(AssimpUtils::Convert(pImportedMesh->mVertices[i0]));
			vertices.push_back(AssimpUtils::Convert(pImportedMesh->mVertices[i1]));
			vertices.push_back(AssimpUtils::Convert(pImportedMesh->mVertices[i2]));

			normals.push_back(AssimpUtils::Convert(pImportedMesh->mNormals[i0]));
			normals.push_back(AssimpUtils::Convert(pImportedMesh->mNormals[i1]));
			normals.push_back(AssimpUtils::Convert(pImportedMesh->mNormals[i2]));

			// TODO: implement multi channel material support
			aiVector3D& rVector = pImportedMesh->mTextureCoords[0][i1];
			aiVector2D uv(rVector[0], rVector[1]);
			uvs.push_back(AssimpUtils::Convert(uv));
			rVector = pImportedMesh->mTextureCoords[0][i1];
			uv = aiVector2D(rVector[0], rVector[1]);
			uvs.push_back(AssimpUtils::Convert(uv));
			rVector = pImportedMesh->mTextureCoords[0][i2];
			uv = aiVector2D(rVector[0], rVector[1]);
			uvs.push_back(AssimpUtils::Convert(uv));			
		}

#ifdef USE_PROGRAMMABLE_PIPELINE
		Mesh* pMesh = new Mesh(vertices, indices, normals, tangents, uvs);
#else
		Mesh* pMesh = new Mesh(vertices, indices, normals, uvs);
#endif

		mMeshCatalog.insert(std::make_pair(i, pMesh));
	}
}

void Model::BuildRootGameObject()
{
	mpRootGameObject = GameObject::Instantiate();
	BuildGameObject(mpRootGameObject, mpScene->mRootNode);
}

void Model::BuildGameObject(GameObject* pGameObject, aiNode* pNode)
{
	Transform* pTransform = pGameObject->GetTransform();
	pTransform->SetWorldTransform(AssimpUtils::Convert(pNode->mTransformation));

	MeshRenderer* pMeshRenderer = MeshRenderer::Instantiate(pGameObject);

	MeshFilter* pMeshFilter = MeshFilter::Instantiate(pGameObject);
	// TODO: multiple materials support
	bool hasAddedMaterial = false;
	for (unsigned int i = 0; i < pNode->mNumMeshes; i++)
	{
		unsigned int meshIndex = pNode->mMeshes[i];
		aiMesh* pImportedMesh = mpScene->mMeshes[meshIndex];
		unsigned int materialIndex = pImportedMesh->mMaterialIndex;

		Mesh* pMesh = mMeshCatalog[meshIndex];
		Material* pMaterial = mMaterialCatalog[materialIndex];

		pMeshRenderer->AddMesh(pMesh);
		if (!hasAddedMaterial)
		{
			pMeshFilter->SetMaterial(pMaterial);
			hasAddedMaterial = true;
		}
	}

	for (unsigned int i = 0; i < pNode->mNumChildren; i++)
	{
		GameObject* pChildNode = GameObject::Instantiate();
		pChildNode->GetTransform()->SetParent(pTransform);

		BuildGameObject(pChildNode, pNode->mChildren[i]);
	}
}