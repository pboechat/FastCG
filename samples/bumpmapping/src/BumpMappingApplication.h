#ifndef BUMPMAPPINGAPPLICATION_H_
#define BUMPMAPPINGAPPLICATION_H_

#include <Application.h>
#include <Texture.h>

#include <string>
#include <vector>

class BumpMappingApplication: public Application
{
public:
	BumpMappingApplication();
	virtual ~BumpMappingApplication();

protected:
	virtual void OnStart();

private:
	static const unsigned int FLOOR_SIZE;
	static const unsigned int NUMBER_OF_SPHERE_SLICES;
	static const float SPHERE_RADIUS;
	static const float WALK_SPEED;
	static const float TURN_SPEED;

	Texture* LoadPNGAsTexture(const std::string& rFileName);

	std::vector<Texture*> mpFloorColorMapTextures;
	std::vector<Texture*> mpFloorBumpMapTextures;
	std::vector<Texture*> mpSphereColorMapTextures;
	std::vector<Texture*> mpSphereBumpMapTextures;
	Material* mpFloorMaterial;
	Mesh* mpFloorMesh;
	Material* mpSphereMaterial;
	Mesh* mpSphereMesh;

};

#endif
