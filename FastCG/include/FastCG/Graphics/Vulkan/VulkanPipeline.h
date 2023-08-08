#ifndef FASTCG_VULKAN_PIPELINE_H
#define FASTCG_VULKAN_PIPELINE_H

#ifdef FASTCG_VULKAN

#include <FastCG/Graphics/Vulkan/Vulkan.h>
#include <FastCG/Graphics/Vulkan/VulkanBuffer.h>
#include <FastCG/Graphics/GraphicsUtils.h>

namespace FastCG
{
    struct VulkanPipeline
    {
        VkPipeline pipeline{VK_NULL_HANDLE};
        VkPipelineLayout layout{VK_NULL_HANDLE};
    };

    struct VulkanPipelineDescription
    {
        bool depthTest;
        bool depthWrite;
        CompareOp depthFunc;
        bool scissorTest;
        bool stencilTest;
        StencilState stencilBackState;
        StencilState stencilFrontState;
        Face cullMode;
        bool blend;
        BlendState blendState;
        const VulkanShader *pShader;
    };

}

#endif

#endif