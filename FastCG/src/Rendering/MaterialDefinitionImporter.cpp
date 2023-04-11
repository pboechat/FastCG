#include <FastCG/Rendering/MaterialDefinitionRegistry.h>
#include <FastCG/Rendering/MaterialDefinitionLoader.h>
#include <FastCG/Rendering/MaterialDefinitionImporter.h>
#include <FastCG/AssetSystem.h>

namespace FastCG
{
    void MaterialDefinitionImporter::Import()
    {
        for (const auto &rMaterialFileName : AssetSystem::GetInstance()->List("materials", true))
        {
            MaterialDefinitionRegistry::GetInstance()->AddMaterialDefinition(MaterialDefinitionLoader::Load(rMaterialFileName));
        }
    }
}