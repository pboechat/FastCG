#include "HdrApplication.h"

#include <FastCG/Assets/AssetSystem.h>
#include <FastCG/World/GameObjectLoader.h>

using namespace FastCG;

HdrApplication::HdrApplication()
    : Application({"hdr", {}, {RenderingPath::DEFERRED, Colors::BLACK, Colors::BLACK, true}})
{
}

void HdrApplication::OnStart()
{
    GameObjectLoader::Load(AssetSystem::GetInstance()->Resolve("scenes/hdr.scene"));
}

FASTCG_MAIN(HdrApplication)