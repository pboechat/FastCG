#ifndef FASTCG_VULKAN_BUFFER_H
#define FASTCG_VULKAN_BUFFER_H

#ifdef FASTCG_VULKAN

#include <FastCG/Graphics/BaseBuffer.h>
#include <FastCG/Graphics/Vulkan/Vulkan.h>
#include <FastCG/Graphics/Vulkan/VulkanUtils.h>

namespace FastCG
{
    class VulkanGraphicsContext;
    class VulkanGraphicsSystem;

    class VulkanBuffer : public BaseBuffer
    {
    public:
        struct Args : BaseBuffer::Args
        {
            bool forceSingleFrameDataCount;
            VulkanGraphicsContext *pGraphicsContext;

            Args(const std::string &rName = "", BufferUsageFlags usage = BufferUsageFlagBit::UNIFORM,
                 size_t dataSize = 0, const void *pData = nullptr,
                 const std::vector<VertexBindingDescriptor> &rVertexBindingDescriptors = {},
                 bool forceSingleFrameDataCount = false, VulkanGraphicsContext *pGraphicsContext = nullptr)
                : BaseBuffer::Args(rName, usage, dataSize, pData, rVertexBindingDescriptors),
                  forceSingleFrameDataCount(forceSingleFrameDataCount), pGraphicsContext(pGraphicsContext)
            {
            }
        };

        struct FrameData
        {
            VkBuffer buffer;
            VmaAllocation allocation;
            VmaAllocationInfo allocationInfo;
        };

        VulkanBuffer(const Args &rArgs);
        VulkanBuffer(const VulkanBuffer &rOther) = delete;
        VulkanBuffer(const VulkanBuffer &&rOther) = delete;
        virtual ~VulkanBuffer();

        inline const FrameData &GetFrameData(uint32_t frameIndex) const
        {
            return mFrameData[frameIndex];
        }
        inline bool IsMultiFrame() const
        {
            return IsDynamic() && !mForceSingleFrameDataCount;
        }

        VulkanBuffer &operator=(const VulkanBuffer &rOther) = delete;

    private:
        std::vector<FrameData> mFrameData;
        bool mForceSingleFrameDataCount;

        inline VkAccessFlags GetDefaultAccessFlags() const
        {
            return GetVkBufferAccessFlags(GetUsage());
        }
        inline VkPipelineStageFlags GetDefaultStageFlags() const
        {
            return GetVkBufferPipelineStageFlags(GetUsage());
        }
        inline bool IsDynamic() const
        {
            return (GetUsage() & BufferUsageFlagBit::DYNAMIC) != 0;
        }
        inline bool UsesMappableMemory() const
        {
            return IsDynamic();
        }
        void CreateBuffer(VulkanGraphicsContext *pGraphicsContext);
        void DestroyBuffer();

        friend class VulkanGraphicsContext;
        friend class VulkanGraphicsSystem;
    };

}

#endif

#endif