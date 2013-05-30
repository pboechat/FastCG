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
#include <vector>

class ModelImporter
{
public:
	static void Initialize();
	static GameObject* Import(const std::string& rFileName);
	static void Dispose();

private:
	enum MaterialAttribute
	{
		MA_HAS_AMBIENT_COLOR = 0,
		MA_HAS_DIFFUSE_COLOR = 2,
		MA_HAS_SPECULAR_COLOR = 4,
		MA_IS_EMISSIVE = 8,
		MA_HAS_SHININESS = 16,
		MA_HAS_COLOR_MAP = 32,
		MA_IS_TWO_SIDED = 64
	};

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