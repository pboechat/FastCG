#ifndef DEFERRED_RENDERING_APPLICATION_H
#define DEFERRED_RENDERING_APPLICATION_H

#include <FastCG/Texture.h>
#include <FastCG/Mesh.h>
#include <FastCG/Material.h>
#include <FastCG/Application.h>

using namespace FastCG;

class DeferredRenderingApplication : public Application
{
public:
	DeferredRenderingApplication();

protected:
	void OnStart() override;

private:
	std::shared_ptr<Texture> mpCheckersColorMapTexture{nullptr};
	std::shared_ptr<Material> mpFloorMaterial{nullptr};
	std::shared_ptr<Mesh> mpFloorMesh{nullptr};
	std::shared_ptr<Material> mpSphereMaterial{nullptr};
	std::shared_ptr<Mesh> mpSphereMesh{nullptr};
};

#endif
