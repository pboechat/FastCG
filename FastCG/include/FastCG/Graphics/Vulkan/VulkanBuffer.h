#ifndef FASTCG_VULKAN_BUFFER_H
#define FASTCG_VULKAN_BUFFER_H

#ifdef FASTCG_VULKAN

#include <FastCG/Graphics/BaseBuffer.h>

namespace FastCG
{
    class VulkanGraphicsSystem;

    class VulkanBuffer : public BaseBuffer
    {
    private:
        VulkanBuffer(const BufferArgs &rArgs);
        VulkanBuffer(const VulkanBuffer &rOther) = delete;
        VulkanBuffer(const VulkanBuffer &&rOther) = delete;
        virtual ~VulkanBuffer();

        VulkanBuffer &operator=(const VulkanBuffer &rOther) = delete;

        friend class VulkanGraphicsSystem;
    };

}

#endif

#endif