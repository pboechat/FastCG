#ifndef DEFERREDSHADINGAPPLICATION_H_
#define DEFERREDSHADINGAPPLICATION_H_

#include <Application.h>
#include <Texture.h>
#include <Material.h>
#include <Mesh.h>

class DeferredShadingApplication: public Application
{
public:
	DeferredShadingApplication();

protected:
	virtual void OnStart();
	virtual void OnEnd();

private:
	static const unsigned int NUMBER_OF_LIGHTS;
	static const float LIGHT_DISTANCE;
	static const float WALK_SPEED;
	static const float TURN_SPEED;

	Texture* mpTexture;
	Material* mpMaterial;
	Mesh* mpMesh;
	
};

#endif
