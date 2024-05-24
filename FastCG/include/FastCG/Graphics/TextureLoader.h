#ifndef FASTCG_TEXTURE_LOADER_H
#define FASTCG_TEXTURE_LOADER_H

#include <FastCG/Graphics/GraphicsSystem.h>

#include <string>

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
        static Texture *LoadKTX(const std::string &rFilePath, TextureLoadSettings settings = {});
        static Texture *LoadDDS(const std::string &rFilePath, TextureLoadSettings settings = {});
        static Texture *LoadPlain(const std::string &rFilePath, TextureLoadSettings settings = {});
        static Texture *Load(const std::string &rFilePath, TextureLoadSettings settings = {});

    private:
        TextureLoader() = delete;
        ~TextureLoader() = delete;
    };

}

#endif