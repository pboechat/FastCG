#ifndef MODELIMPORTER_H_
#define MODELIMPORTER_H_

#include <GameObject.h>
#include <AABB.h>
#include <Material.h>
#include <Mesh.h>

#include <assimp/cimport.h>
#include <assimp/scene.h>

#include <glm/glm.hpp>

#include <string>
#include <map>

class ModelImporter
{
public:
	static void Initialize();
	static GameObject* Import(const std::string& rFileName);
	static void Dispose();

private:
	static bool s_mInitialized;
	static std::vector<Texture*> s_mManagedTextures;
	static std::vector<Material*> s_mManagedMaterials;
	static std::vector<Mesh*> s_mManagedMeshes;
	static std::vector<const aiScene*> s_mImportedScenes;

	static void BuildMaterialCatalog(const aiScene* pScene, const std::string& rBaseDirectory, std::map<unsigned int, Material*>& rMaterialCatalog);
	static void BuildMeshCatalog(const aiScene* pScene, std::map<unsigned int, Mesh*>& rMeshCatalog);
	static AABB CalculateBoundingVolume(const aiScene* pScene);
	static void CalculateBoundingVolumeRecursively(const aiScene* pScene, aiNode* pNode, const glm::mat4& rParentTransform, AABB& boundingVolume);
	static GameObject* BuildGameObject(const aiScene* pScene, std::map<unsigned int, Material*>& rMaterialCatalog, std::map<unsigned int, Mesh*>& rMeshCatalog);
	static void BuildGameObjectRecursively(const aiScene* pScene, std::map<unsigned int, Material*>& rMaterialCatalog, std::map<unsigned int, Mesh*>& rMeshCatalog, aiNode* pNode, GameObject* pGameObject);

	ModelImporter()
	{
	}

	~ModelImporter()
	{
	}

};

#endif