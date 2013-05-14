#ifndef DEFERREDSHADINGAPPLICATION_H_
#define DEFERREDSHADINGAPPLICATION_H_

#include <Application.h>
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
	Mesh* mpMesh;
	Material* mpMaterial;

};

#endif
