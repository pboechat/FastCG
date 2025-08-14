#include <FastCG/Core/Exception.h>
#include <FastCG/Core/Macros.h>
#include <FastCG/Graphics/TextureLoader.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <cstdint>
#include <memory>

namespace FastCG
{
    Texture *TextureLoader::LoadPlain(const std::filesystem::path &rFilePath, TextureLoadSettings settings)
    {
        int width, height, componentCount;
        auto *pData = stbi_load(rFilePath.string().c_str(), &width, &height, &componentCount, 0);
        if (pData == nullptr)
        {
            return nullptr;
        }

        std::unique_ptr<uint8_t[]> transformedData;
        TextureFormat format;
        switch (componentCount)
        {
        case 1:
            format = TextureFormat::R8_UNORM;
            break;
        case 2:
            format = TextureFormat::R8G8_UNORM;
            break;
        case 3: {
            // make data RGBA by adding a constant alpha value (ie, alpha = 1)
            transformedData = std::make_unique<uint8_t[]>(width * height * 4 * sizeof(uint8_t));
            int i = 0, j = 0;
            for (int y = 0; y < height; ++y)
            {
                for (int x = 0; x < width; ++x)
                {
                    for (int c = 0; c < 3; ++c)
                    {
                        transformedData[i++] = pData[j++];
                    }
                    transformedData[i++] = 1;
                }
            }
            stbi_image_free(pData);
            pData = transformedData.get();
            componentCount = 4;
        }
        case 4:
            format = TextureFormat::R8G8B8A8_UNORM;
            break;
        default:
            FASTCG_THROW_EXCEPTION(Exception, "Invalid component count (texture: %s, componentCount: %d)",
                                   rFilePath.string().c_str(), componentCount);
            return nullptr;
        }

        auto *pTexture = GraphicsSystem::GetInstance()->CreateTexture(
            {rFilePath.stem().string(), (uint32_t)width, (uint32_t)height, 1, 1, TextureType::TEXTURE_2D,
             settings.usage, format, settings.filter, settings.wrapMode, pData});
        if (transformedData == nullptr)
        {
            stbi_image_free(pData);
        }

        return pTexture;
    }

}