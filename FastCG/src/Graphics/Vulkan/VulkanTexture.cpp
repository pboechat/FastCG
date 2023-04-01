#ifdef FASTCG_VULKAN

#include <FastCG/Graphics/Vulkan/VulkanTexture.h>

namespace FastCG
{
    VulkanTexture::VulkanTexture(const TextureArgs &rArgs) : BaseTexture(rArgs)
    {
    }

    VulkanTexture::~VulkanTexture()
    {
    }
}

#endif