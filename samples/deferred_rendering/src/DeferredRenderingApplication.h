#ifndef DEFERRED_RENDERING_APPLICATION_H
#define DEFERRED_RENDERING_APPLICATION_H

#include <FastCG/RenderingSystem.h>
#include <FastCG/Application.h>

class DeferredRenderingApplication : public FastCG::Application
{
public:
	DeferredRenderingApplication();

protected:
	void OnStart() override;

private:
	FastCG::Texture *mpCheckersColorMap{nullptr};
	FastCG::Material *mpFloorMaterial{nullptr};
	FastCG::Mesh *mpFloorMesh{nullptr};
	FastCG::Material *mpSphereMaterial{nullptr};
	FastCG::Mesh *mpSphereMesh{nullptr};
};

#endif
