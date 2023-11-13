#include <FastCG/Rendering/MaterialDefinitionRegistry.h>
#include <FastCG/Rendering/MaterialDefinitionLoader.h>
#include <FastCG/Rendering/MaterialDefinitionImporter.h>
#include <FastCG/Core/Log.h>
#include <FastCG/Assets/AssetSystem.h>

namespace FastCG
{
    void MaterialDefinitionImporter::Import()
    {
        FASTCG_LOG_VERBOSE(MaterialDefinitionImporter, "Importing materials:");
        for (const auto &rMaterialFileName : AssetSystem::GetInstance()->List("materials", true))
        {
            auto pMaterialDefinition = MaterialDefinitionLoader::Load(rMaterialFileName);
            FASTCG_LOG_VERBOSE(MaterialDefinitionImporter, "- %s", pMaterialDefinition->GetName().c_str());
            MaterialDefinitionRegistry::GetInstance()->AddMaterialDefinition(std::move(pMaterialDefinition));
        }
    }
}