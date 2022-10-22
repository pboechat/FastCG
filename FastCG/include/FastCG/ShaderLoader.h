#ifndef FASTCG_SHADER_LOADER_H
#define FASTCG_SHADER_LOADER_H

#include <FastCG/RenderingSystem.h>
#include <FastCG/RenderingPath.h>

namespace FastCG
{
    class ShaderLoader
    {
    public:
        inline static void LoadShaders(RenderingPath renderingPath);

    private:
        inline static bool ExtractShaderInfo(const std::string &rShaderFileName, std::string &rShaderName, ShaderType &rShaderType, RenderingPathMask &shaderRenderingPathMask);

        ShaderLoader() = delete;
        ~ShaderLoader() = delete;
    };

}

#include <FastCG/ShaderLoader.inc>

#endif