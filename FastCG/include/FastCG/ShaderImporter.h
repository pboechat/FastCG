#ifndef FASTCG_SHADER_IMPORTER_H
#define FASTCG_SHADER_IMPORTER_H

#include <FastCG/RenderingSystem.h>
#include <FastCG/RenderingPath.h>

namespace FastCG
{
    class ShaderImporter
    {
    public:
        inline static void Import(RenderingPath renderingPath);

    private:
        ShaderImporter() = delete;
        ~ShaderImporter() = delete;
    };

}

#include <FastCG/ShaderImporter.inc>

#endif