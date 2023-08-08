#ifdef FASTCG_VULKAN

#include <FastCG/Graphics/Vulkan/VulkanGraphicsSystem.h>
#include <FastCG/Graphics/Vulkan/VulkanExceptions.h>
#include <FastCG/Graphics/Vulkan/VulkanBuffer.h>

#include <cstring>

namespace FastCG
{
    VulkanBuffer::VulkanBuffer(const BufferArgs &rArgs) : BaseBuffer(rArgs)
    {
        CreateBuffer();
    }

    VulkanBuffer::~VulkanBuffer()
    {
        DestroyBuffer();
    }

    void VulkanBuffer::CreateBuffer()
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

        bool usesMappableMemory = (GetUsage() & BufferUsageFlagBit::DYNAMIC) != 0;

        VmaAllocationCreateInfo allocationCreateInfo;
        if (usesMappableMemory)
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
                                               &mBuffer,
                                               &mAllocation,
                                               &mAllocationInfo));

        if (GetData() != nullptr)
        {
            VulkanGraphicsSystem::GetInstance()->GetImmediateGraphicsContext()->Copy(this, GetDataSize(), GetData());
        }

#if _DEBUG
        VulkanGraphicsSystem::GetInstance()->SetObjectName(GetName().c_str(), VK_OBJECT_TYPE_BUFFER, (uint64_t)mBuffer);
#endif
    }

    void VulkanBuffer::DestroyBuffer()
    {
        if (mBuffer != VK_NULL_HANDLE && mAllocation != VK_NULL_HANDLE)
        {
            vmaDestroyBuffer(VulkanGraphicsSystem::GetInstance()->GetAllocator(),
                             mBuffer,
                             mAllocation);
            mBuffer = VK_NULL_HANDLE;
            mAllocation = VK_NULL_HANDLE;
        }
    }

}

#endif