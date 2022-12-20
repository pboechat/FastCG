#ifndef FASTCG_SHADER_IMPORTER_H
#define FASTCG_SHADER_IMPORTER_H

#include <FastCG/RenderingSystem.h>
#include <FastCG/RenderingPath.h>

namespace FastCG
{
    class ShaderImporter
    {
    public:
        static void Import();

    private:
        ShaderImporter() = delete;
        ~ShaderImporter() = delete;
    };

}

#endif