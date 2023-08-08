#ifndef FASTCG_VULKAN_DESCRIPTOR_SET_H
#define FASTCG_VULKAN_DESCRIPTOR_SET_H

#ifdef FASTCG_VULKAN

#include <FastCG/Graphics/Vulkan/Vulkan.h>
#include <FastCG/Graphics/Vulkan/VulkanTexture.h>
#include <FastCG/Graphics/Vulkan/VulkanBuffer.h>

#include <vector>
#include <cstdint>

namespace FastCG
{
    struct VulkanDescriptorSetLayoutBinding
    {
        uint32_t binding{~0u};
        VkDescriptorType type;
        VkShaderStageFlags stageFlags;
    };

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