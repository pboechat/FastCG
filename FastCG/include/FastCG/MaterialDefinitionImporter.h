#ifndef FASTCG_MATERIAL_DEFINITION_IMPORTER_H
#define FASTCG_MATERIAL_DEFINITION_IMPORTER_H

namespace FastCG
{
    class MaterialDefinitionImporter
    {
    public:
        inline static void Import();

    private:
        MaterialDefinitionImporter() = delete;
        ~MaterialDefinitionImporter() = delete;
    };

}

#include <FastCG/MaterialDefinitionImporter.inc>

#endif