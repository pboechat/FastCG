#ifndef FASTCG_VULKAN_DESCRIPTOR_SET_H
#define FASTCG_VULKAN_DESCRIPTOR_SET_H

#ifdef FASTCG_VULKAN

#include <FastCG/Graphics/Vulkan/Vulkan.h>
#include <FastCG/Graphics/Vulkan/VulkanTexture.h>
#include <FastCG/Graphics/Vulkan/VulkanBuffer.h>
#include <FastCG/Core/Macros.h>

#include <vector>
#include <cstdint>

namespace FastCG
{
    // remove all padding cause struct memory is used to compute hash
    FASTCG_PACKED_PREFIX struct VulkanDescriptorSetLayoutBinding
    {
        uint32_t binding{~0u};
        VkDescriptorType type;
        VkShaderStageFlags stageFlags;
    } FASTCG_PACKED_SUFFIX;

    using VulkanDescriptorSetLayout = std::vector<VulkanDescriptorSetLayoutBinding>;

    struct VulkanDescriptorSetBinding
    {
        union
        {
            const VulkanBuffer *pBuffer;
            const VulkanTexture *pTexture;
        };
    };

    using VulkanDescriptorSet = std::vector<VulkanDescriptorSetBinding>;

    using VulkanPipelineLayoutDescription = std::vector<VulkanDescriptorSetLayout>;
}

#endif

#endif