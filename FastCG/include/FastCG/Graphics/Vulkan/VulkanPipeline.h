#ifndef FASTCG_VULKAN_PIPELINE_H
#define FASTCG_VULKAN_PIPELINE_H

#ifdef FASTCG_VULKAN

#include <FastCG/Graphics/Vulkan/Vulkan.h>
#include <FastCG/Graphics/Vulkan/VulkanBuffer.h>
#include <FastCG/Graphics/GraphicsUtils.h>
#include <FastCG/Core/Macros.h>

#include <vector>

namespace FastCG
{
    struct VulkanPipeline
    {
        VkPipeline pipeline{VK_NULL_HANDLE};
        VkPipelineLayout layout{VK_NULL_HANDLE};
    };

    // remove all padding cause struct memory is used to compute hash
    FASTCG_PACKED_PREFIX struct VulkanPipelineDescription
    {
        const VulkanShader *pShader;
        union
        {
            struct
            {
                VkViewport viewport;
                VkRect2D scissor;
                uint32_t vertexBufferCount;
                const VulkanBuffer *ppVertexBuffers[16];
                const VulkanBuffer *pIndexBuffer;
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
            } graphicsInfo;
            struct
            {
            } computeInfo;
        };
    } FASTCG_PACKED_SUFFIX;

}

#endif

#endif