#ifndef FASTCG_VULKAN_RENDER_PASS_H
#define FASTCG_VULKAN_RENDER_PASS_H

#ifdef FASTCG_VULKAN

#include <FastCG/Graphics/Vulkan/Vulkan.h>
#include <FastCG/Graphics/Vulkan/VulkanTexture.h>

#include <vector>
#include <cstdint>

namespace FastCG
{
    enum VulkanClearRequestFlagBit
    {
        NONE = 0,
        STENCIL = 1 << 0,
        COLOR_OR_DEPTH = 1 << 1,
    };

    using VulkanClearRequestFlags = uint8_t;

    struct VulkanClearRequest
    {
        const VulkanTexture *pTexture;
        VkClearValue value;
        VulkanClearRequestFlags flags;
    };

    struct VulkanRenderPassDescription
    {
        static constexpr uint32_t MAX_RENDER_TARGET_COUNT = 8;

        uint32_t renderTargetCount;
        const VulkanTexture *ppRenderTargets[MAX_RENDER_TARGET_COUNT];
        const VulkanTexture *pDepthStencilBuffer;
    };

}

#endif

#endif