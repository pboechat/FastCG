#ifndef FASTCG_SHADER_IMPORTER_H
#define FASTCG_SHADER_IMPORTER_H

#include <FastCG/RenderingPath.h>
#include <FastCG/Graphics/GraphicsSystem.h>

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