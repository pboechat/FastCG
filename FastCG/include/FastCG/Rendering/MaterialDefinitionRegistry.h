#ifndef FASTCG_MATERIAL_DEFINITION_REGISTRY_H
#define FASTCG_MATERIAL_DEFINITION_REGISTRY_H

#include <FastCG/Rendering/MaterialDefinition.h>

#include <unordered_map>
#include <string>
#include <memory>
#include <cassert>

namespace FastCG
{
    class MaterialDefinitionRegistry
    {
    public:
        inline static MaterialDefinitionRegistry *GetInstance()
        {
            if (smpInstance == nullptr)
            {
                smpInstance = new MaterialDefinitionRegistry();
            }
            return smpInstance;
        }

        inline void AddMaterialDefinition(const std::shared_ptr<MaterialDefinition> &pMaterialDefinition)
        {
            assert(pMaterialDefinition != nullptr);
            mMaterialDefinitions[pMaterialDefinition->GetName()] = pMaterialDefinition;
        }

        inline std::shared_ptr<MaterialDefinition> GetMaterialDefinition(const std::string &rName)
        {
            auto it = mMaterialDefinitions.find(rName);
            if (it == mMaterialDefinitions.end())
            {
                return nullptr;
            }
            return it->second;
        }

    private:
        static MaterialDefinitionRegistry *smpInstance;

        std::unordered_map<std::string, std::shared_ptr<MaterialDefinition>> mMaterialDefinitions;

        MaterialDefinitionRegistry() = default;
        ~MaterialDefinitionRegistry() = default;
    };

}

#endif