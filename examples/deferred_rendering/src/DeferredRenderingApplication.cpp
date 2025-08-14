#include "DeferredRenderingApplication.h"
#include "Controls.h"
#include "LightsAnimator.h"

#include <FastCG/Assets/AssetSystem.h>
#include <FastCG/World/ComponentRegistry.h>
#include <FastCG/World/GameObjectLoader.h>

#include <cstdint>
#include <vector>

using namespace FastCG;

DeferredRenderingApplication::DeferredRenderingApplication()
    : Application({"deferred_rendering", {}, {RenderingPath::DEFERRED, Colors::BLACK, Colors::BLACK, true}})
{
    ComponentRegistry::RegisterComponent<Controls>();
    ComponentRegistry::RegisterComponent<LightsAnimator>();
}

void DeferredRenderingApplication::OnStart()
{
    GameObjectLoader::Load(AssetSystem::GetInstance()->Resolve("scenes/deferred_rendering.scene"));
}

FASTCG_MAIN(DeferredRenderingApplication)