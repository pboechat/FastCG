#ifndef FASTCG_VULKAN_TEXTURE_H
#define FASTCG_VULKAN_TEXTURE_H

#ifdef FASTCG_VULKAN

#include <FastCG/Graphics/BaseTexture.h>

namespace FastCG
{
    class VulkanGraphicsSystem;

    class VulkanTexture : public BaseTexture
    {
    private:
        VulkanTexture(const TextureArgs &rArgs);
        VulkanTexture(const VulkanTexture &rOther) = delete;
        VulkanTexture(const VulkanTexture &&rOther) = delete;
        virtual ~VulkanTexture();

        VulkanTexture operator=(const VulkanTexture &rOther) = delete;

        friend class VulkanGraphicsSystem;
    };
}

#endif

#endif