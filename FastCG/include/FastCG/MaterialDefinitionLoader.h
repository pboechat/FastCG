#ifndef FASTCG_MATERIAL_DEFINITION_LOADER_H
#define FASTCG_MATERIAL_DEFINITION_LOADER_H

namespace FastCG
{
    class MaterialDefinitionLoader
    {
    public:
        inline static void LoadMaterialDefinitions();

    private:
        MaterialDefinitionLoader() = delete;
        ~MaterialDefinitionLoader() = delete;
    };

}

#include <FastCG/MaterialDefinitionLoader.inc>

#endif