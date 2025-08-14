#ifndef FASTCG_SHADER_SOURCE_H
#define FASTCG_SHADER_SOURCE_H

#include <FastCG/Graphics/GraphicsSystem.h>

#include <filesystem>
#include <string>

namespace FastCG
{
    class ShaderSource final
    {
    public:
        inline static std::string ParseFile(const std::filesystem::path &rFilePath);
        inline static void ParseSource(std::string &rSource, const std::filesystem::path &rIncludePath);

    private:
        ShaderSource() = delete;
        ~ShaderSource() = delete;
    };

}

#include <FastCG/Graphics/ShaderSource.inc>

#endif
