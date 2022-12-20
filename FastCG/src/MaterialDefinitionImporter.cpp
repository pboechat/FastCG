#include <FastCG/MaterialDefinitionLoader.h>
#include <FastCG/MaterialDefinitionImporter.h>
#include <FastCG/AssetSystem.h>

namespace FastCG
{
    void MaterialDefinitionImporter::Import()
    {
        for (const auto &rMaterialFileName : AssetSystem::GetInstance()->List("materials", true))
        {
            MaterialDefinitionLoader::Load(rMaterialFileName);
        }
    }
}