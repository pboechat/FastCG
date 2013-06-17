#ifndef BUMPMAPPINGAPPLICATION_H_
#define BUMPMAPPINGAPPLICATION_H_

#include <Application.h>
#include <Texture.h>
#include <Material.h>
#include <Mesh.h>

#include <string>
#include <vector>

class BumpMappingApplication: public Application
{
public:
	BumpMappingApplication();

protected:
	virtual void OnStart();
	virtual void OnEnd();

private:
	static const float WALK_SPEED;
	static const float TURN_SPEED;

	std::vector<Texture*> mFloorColorMapTextures;
	std::vector<Texture*> mFloorBumpMapTextures;
	std::vector<Texture*> mSphereColorMapTextures;
	std::vector<Texture*> mSphereBumpMapTextures;
	Material* mpFloorMaterial;
	Mesh* mpFloorMesh;
	Material* mpSphereMaterial;
	Mesh* mpSphereMesh;

};

#endif
