#ifndef FASTCG_MATERIAL_DEFINITION_LOADER_H
#define FASTCG_MATERIAL_DEFINITION_LOADER_H

#include <FastCG/Rendering/MaterialDefinition.h>

#include <memory>
#include <string>

namespace FastCG
{
    class MaterialDefinitionLoader
    {
    public:
        static std::unique_ptr<MaterialDefinition> Load(const std::string &rFilePath);
        static std::string Dump(const std::unique_ptr<MaterialDefinition> &pMaterialDefinition);

    private:
        MaterialDefinitionLoader() = delete;
        ~MaterialDefinitionLoader() = delete;
    };

}

#endif