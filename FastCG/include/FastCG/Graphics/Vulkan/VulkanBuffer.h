#ifndef FASTCG_VULKAN_BUFFER_H
#define FASTCG_VULKAN_BUFFER_H

#ifdef FASTCG_VULKAN

#include <FastCG/Graphics/Vulkan/Vulkan.h>
#include <FastCG/Graphics/BaseBuffer.h>

namespace FastCG
{
    class VulkanGraphicsContext;
    class VulkanGraphicsSystem;

    class VulkanBuffer : public BaseBuffer
    {
    public:
        VulkanBuffer(const BufferArgs &rArgs);
        VulkanBuffer(const VulkanBuffer &rOther) = delete;
        VulkanBuffer(const VulkanBuffer &&rOther) = delete;
        virtual ~VulkanBuffer();

        VulkanBuffer &operator=(const VulkanBuffer &rOther) = delete;

    private:
        VkBuffer mBuffer{VK_NULL_HANDLE};
        VmaAllocation mAllocation{VK_NULL_HANDLE};
        VmaAllocationInfo mAllocationInfo;

        inline VkBuffer GetBuffer() const
        {
            return mBuffer;
        }
        inline VmaAllocation GetAllocation() const
        {
            return mAllocation;
        }
        inline VmaAllocationInfo GetAllocationInfo() const
        {
            return mAllocationInfo;
        }
        inline VkAccessFlags GetDefaultAccessFlags() const
        {
            return GetVkBufferAccessFlags(GetUsage());
        }
        inline VkPipelineStageFlags GetDefaultStageFlags() const
        {
            return GetVkBufferPipelineStageFlags(GetUsage());
        }
        void CreateBuffer();
        void DestroyBuffer();

        friend class VulkanGraphicsContext;
        friend class VulkanGraphicsSystem;
    };

}

#endif

#endif