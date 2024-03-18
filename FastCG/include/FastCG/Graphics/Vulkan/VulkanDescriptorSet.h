#ifndef FASTCG_VULKAN_DESCRIPTOR_SET_H
#define FASTCG_VULKAN_DESCRIPTOR_SET_H

#ifdef FASTCG_VULKAN

#include <FastCG/Graphics/Vulkan/Vulkan.h>
#include <FastCG/Graphics/Vulkan/VulkanTexture.h>
#include <FastCG/Graphics/Vulkan/VulkanBuffer.h>
#include <FastCG/Core/Macros.h>

#include <cstdint>

namespace FastCG
{
    struct VulkanDescriptorSetBinding
    {
        union
        {
            const VulkanBuffer *pBuffer;
            const VulkanTexture *pTexture;
        };
    };

    struct VulkanDescriptorSet
    {
        static constexpr uint32_t MAX_BINDING_COUNT = 32;

        uint32_t bindingCount;
        VulkanDescriptorSetBinding pBindings[MAX_BINDING_COUNT];
    };

    struct VulkanPipelineLayout
    {
        static constexpr uint32_t MAX_SET_COUNT = 16;

        uint32_t setCount;
        VulkanDescriptorSet pSet[MAX_SET_COUNT];
    };

    // remove all padding cause struct memory is used to compute hash
    FASTCG_PACKED_PREFIX struct VulkanDescriptorSetLayoutBinding
    {
        uint32_t binding{~0u};
        VkDescriptorType type;
        VkShaderStageFlags stageFlags;
    } FASTCG_PACKED_SUFFIX;

    struct VulkanDescriptorSetLayout
    {
        uint32_t bindingLayoutCount;
        VulkanDescriptorSetLayoutBinding pBindingLayouts[VulkanDescriptorSet::MAX_BINDING_COUNT];
    };

    struct VulkanPipelineLayoutDescription
    {
        uint32_t setLayoutCount;
        VulkanDescriptorSetLayout pSetLayouts[VulkanPipelineLayout::MAX_SET_COUNT];
    };
}

#endif

#endif