#ifndef BUMPMAPPING_APPLICATION_H
#define BUMPMAPPING_APPLICATION_H

#include <FastCG/Application.h>
#include <FastCG/GameObject.h>
#include <FastCG/Mesh.h>
#include <FastCG/Material.h>
#include <FastCG/Texture.h>

#include <memory>

using namespace FastCG;

class BumpMappingApplication : public Application
{
public:
	BumpMappingApplication();

protected:
	void OnStart() override;

private:
	std::shared_ptr<Mesh> mpGroundMesh{nullptr};
	std::shared_ptr<Material> mpGroundMaterial{nullptr};
	std::shared_ptr<Texture> mpGroundColorMapTexture{nullptr};
	std::shared_ptr<Texture> mpGroundBumpMapTexture{nullptr};

	void CreateGround();
};

#endif