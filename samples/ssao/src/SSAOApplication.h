#ifndef SSAOAPPLICATION_H_
#define SSAOAPPLICATION_H_

#include <FastCG/Mesh.h>
#include <FastCG/Material.h>
#include <FastCG/Application.h>

#include <vector>
#include <memory.h>

using namespace FastCG;

class SSAOApplication : public Application
{
public:
	SSAOApplication();

protected:
	void OnStart() override;

	std::vector<std::shared_ptr<Mesh>> mMeshes;
	std::shared_ptr<Material> mpDefaultMaterial;

};

#endif