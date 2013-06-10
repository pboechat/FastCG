#ifndef DEFERREDRENDERINGAPPLICATION_H_
#define DEFERREDRENDERINGAPPLICATION_H_

#include <Application.h>
#include <Texture.h>
#include <Material.h>
#include <Mesh.h>

class DeferredRenderingApplication: public Application
{
public:
	DeferredRenderingApplication();

protected:
	virtual void OnStart();
	virtual void OnEnd();

private:
	static const unsigned int FLOOR_SIZE;
	static const unsigned int NUMBER_OF_SPHERE_SLICES;
	static const unsigned int LIGHT_GRID_WIDTH;
	static const unsigned int LIGHT_GRID_DEPTH;
	static const unsigned int LIGHT_GRID_SIZE;
	static const float SPHERE_RADIUS;
	static const float WALK_SPEED;
	static const float TURN_SPEED;

	Texture* mpCheckersColorMapTexture;
	Material* mpFloorMaterial;
	Mesh* mpFloorMesh;
	Material* mpSphereMaterial;
	Mesh* mpSphereMesh;
	
};

#endif
