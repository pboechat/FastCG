#ifndef BUMPMAPPING2APPLICATION_H_
#define BUMPMAPPING2APPLICATION_H_

#include <Application.h>
#include <GameObject.h>
#include <Mesh.h>
#include <Material.h>
#include <Texture.h>

#include <vector>

class BumpMapping2Application : public Application
{
public:
	BumpMapping2Application();
	~BumpMapping2Application();

protected:
	virtual void OnStart();

private:
	Mesh* mpGroundMesh;
	Material* mpGroundMaterial;
	Texture* mpGroundColorMapTexture;
	Texture* mpGroundBumpMapTexture;

	void CreateSceneLights(std::vector<GameObject*>& rSceneLights) const;
	void LoadModel(std::vector<Material*>& rModelMaterials) const;
	void CreateGround();
	void GetMaterialsRecursively(const GameObject* pGameObject, std::vector<Material*>& rMaterials) const;

};

#endif