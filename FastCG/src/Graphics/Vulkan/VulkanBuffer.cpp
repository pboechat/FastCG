#ifdef FASTCG_VULKAN

#include <FastCG/Graphics/Vulkan/VulkanGraphicsSystem.h>
#include <FastCG/Graphics/Vulkan/VulkanExceptions.h>
#include <FastCG/Graphics/Vulkan/VulkanBuffer.h>

#include <cstring>

namespace FastCG
{
    VulkanBuffer::VulkanBuffer(const Args &rArgs) : BaseBuffer(rArgs), mForceSingleFrameDataCount(rArgs.forceSingleFrameDataCount)
    {
        CreateBuffer(rArgs.pGraphicsContext != nullptr ? rArgs.pGraphicsContext : VulkanGraphicsSystem::GetInstance()->GetImmediateGraphicsContext());
    }

    VulkanBuffer::~VulkanBuffer()
    {
        DestroyBuffer();
    }

    void VulkanBuffer::CreateBuffer(VulkanGraphicsContext *pGraphicsContext)
    {
        VkBufferCreateInfo bufferCreateInfo;
        bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferCreateInfo.pNext = nullptr;
        bufferCreateInfo.flags = 0;
        bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        bufferCreateInfo.queueFamilyIndexCount = 0;
        bufferCreateInfo.pQueueFamilyIndices = nullptr;
        bufferCreateInfo.usage = GetVkBufferUsageFlags(GetUsage());
        bufferCreateInfo.size = GetDataSize();

        uint32_t frameDataCount = IsMultiFrame() ? VulkanGraphicsSystem::GetInstance()->GetMaxSimultaneousFrames() : 1;
        mFrameData.resize(frameDataCount);

        for (uint32_t i = 0; i < frameDataCount; ++i)
        {
            VmaAllocationCreateInfo allocationCreateInfo;
            if (UsesMappableMemory())
            {
                allocationCreateInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
            }
            else
            {
                allocationCreateInfo.flags = 0;
            }
            allocationCreateInfo.usage = VMA_MEMORY_USAGE_AUTO;
            allocationCreateInfo.requiredFlags = 0;
            allocationCreateInfo.preferredFlags = 0;
            allocationCreateInfo.memoryTypeBits = 0;
            allocationCreateInfo.pool = VK_NULL_HANDLE;
            allocationCreateInfo.pUserData = nullptr;

            FASTCG_CHECK_VK_RESULT(vmaCreateBuffer(VulkanGraphicsSystem::GetInstance()->GetAllocator(),
                                                   &bufferCreateInfo,
                                                   &allocationCreateInfo,
                                                   &mFrameData[i].buffer,
                                                   &mFrameData[i].allocation,
                                                   &mFrameData[i].allocationInfo));

            if (GetData() != nullptr)
            {
                pGraphicsContext->Copy(this, GetData(), i, GetDataSize());
            }

#if _DEBUG
            VulkanGraphicsSystem::GetInstance()->SetObjectName((GetName() + (frameDataCount > 1 ? " (" + std::to_string(i) + ")" : "") + " (VkBuffer)").c_str(), VK_OBJECT_TYPE_BUFFER, (uint64_t)mFrameData[i].buffer);
#endif
        }
    }

    void VulkanBuffer::DestroyBuffer()
    {
        for (auto &frameData : mFrameData)
        {
            if (frameData.buffer == VK_NULL_HANDLE || frameData.allocation == VK_NULL_HANDLE)
            {
                continue;
            }
            vmaDestroyBuffer(VulkanGraphicsSystem::GetInstance()->GetAllocator(),
                             frameData.buffer,
                             frameData.allocation);
            frameData.buffer = VK_NULL_HANDLE;
            frameData.allocation = VK_NULL_HANDLE;
        }
        mFrameData.clear();
    }

}

#endif