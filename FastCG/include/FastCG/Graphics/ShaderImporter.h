#ifndef FASTCG_SHADER_IMPORTER_H
#define FASTCG_SHADER_IMPORTER_H

#include <FastCG/Graphics/GraphicsSystem.h>

namespace FastCG
{
    class ShaderImporter final
    {
    public:
        static void Import();

    private:
        ShaderImporter() = delete;
        ~ShaderImporter() = delete;
    };

}

#endif