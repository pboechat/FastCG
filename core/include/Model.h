#ifndef MODEL_H_
#define MODEL_H_

#include <GameObject.h>
#include <Material.h>
#include <Mesh.h>

#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <string>
#include <map>

class Model
{
public:
	Model(const std::string& rFileName);
	~Model();

	inline GameObject* GetRootGameObject()
	{
		if (mpScene == 0)
		{
			AllocateResources();
		}

		return mpRootGameObject;
	}

private:
	std::string mFileName;
	const aiScene* mpScene;
	std::map<unsigned int, Material*> mMaterialCatalog;
	std::map<unsigned int, Mesh*> mMeshCatalog;
	std::vector<Texture*> mImportedTextures;
	GameObject* mpRootGameObject;

	void AllocateResources();
	void DeallocateResources();
	void BuildMaterialCatalog();
	void BuildMeshCatalog();
	void BuildRootGameObject();
	void BuildGameObject(GameObject* pGameObject, aiNode* pNode);

};

#endif