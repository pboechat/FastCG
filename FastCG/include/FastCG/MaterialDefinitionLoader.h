#ifndef FASTCG_MATERIAL_DEFINITION_LOADER_H
#define FASTCG_MATERIAL_DEFINITION_LOADER_H

#include <FastCG/Graphics/GraphicsSystem.h>

#include <string>

namespace FastCG
{
    class MaterialDefinitionLoader
    {
    public:
        static const MaterialDefinition *Load(const std::string &rFilePath);
        static std::string Dump(const MaterialDefinition *pMaterialDefinition);

    private:
        MaterialDefinitionLoader() = delete;
        ~MaterialDefinitionLoader() = delete;
    };

}

#endif