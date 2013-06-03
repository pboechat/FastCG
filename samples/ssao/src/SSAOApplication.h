#ifndef SSAOAPPLICATION_H_
#define SSAOAPPLICATION_H_

#include <Application.h>
#include <Mesh.h>
#include <Material.h>

class SSAOApplication : public Application
{
public:
	SSAOApplication();
	~SSAOApplication();

protected:
	virtual void OnStart();

private:
	Mesh* mpGroundMesh;
	Material* mpGroundMaterial;

};

#endif