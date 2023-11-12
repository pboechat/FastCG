#include "DeferredRenderingApplication.h"
#include "Controls.h"
#include "LightsAnimator.h"

#include <FastCG/World/GameObjectLoader.h>
#include <FastCG/World/ComponentRegistry.h>
#include <FastCG/Assets/AssetSystem.h>

#include <vector>
#include <cstdint>

using namespace FastCG;

DeferredRenderingApplication::DeferredRenderingApplication() : Application({"deferred_rendering", 1024, 768, 60, 3, false, {RenderingPath::DEFERRED}, {{"deferred_rendering"}}})
{
	ComponentRegistry::RegisterComponent<Controls>();
	ComponentRegistry::RegisterComponent<LightsAnimator>();
}

void DeferredRenderingApplication::OnStart()
{
	GameObjectLoader::Load(AssetSystem::GetInstance()->Resolve("scenes/deferred_rendering.scene"));
}

FASTCG_MAIN(DeferredRenderingApplication)