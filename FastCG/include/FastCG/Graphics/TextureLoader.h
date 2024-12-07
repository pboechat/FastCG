#ifndef FASTCG_TEXTURE_LOADER_H
#define FASTCG_TEXTURE_LOADER_H

#include <FastCG/Graphics/GraphicsSystem.h>

#include <filesystem>

namespace FastCG
{
    struct TextureLoadSettings
    {
        TextureUsageFlags usage{TextureUsageFlagBit::SAMPLED};
        TextureFilter filter{TextureFilter::LINEAR_FILTER};
        TextureWrapMode wrapMode{TextureWrapMode::CLAMP};
    };

    class TextureLoader final
    {
    public:
        static Texture *LoadKTX(const std::filesystem::path &rFilePath, TextureLoadSettings settings = {});
        static Texture *LoadDDS(const std::filesystem::path &rFilePath, TextureLoadSettings settings = {});
        static Texture *LoadPlain(const std::filesystem::path &rFilePath, TextureLoadSettings settings = {});
        static Texture *Load(const std::filesystem::path &rFilePath, TextureLoadSettings settings = {});

    private:
        TextureLoader() = delete;
        ~TextureLoader() = delete;
    };

}

#endif