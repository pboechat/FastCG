#ifdef FASTCG_VULKAN

#include <FastCG/Graphics/Vulkan/VulkanErrorHandling.h>
#include <FastCG/Graphics/Vulkan/VulkanGraphicsSystem.h>
#include <FastCG/Graphics/Vulkan/VulkanTexture.h>

#include <cstring>

namespace FastCG
{
    VulkanTexture::VulkanTexture(const Args &rArgs) : BaseTexture(rArgs), mImage(rArgs.image)
    {
        if (mImage == VK_NULL_HANDLE)
        {
            CreateImage();
        }
#if _DEBUG
        VulkanGraphicsSystem::GetInstance()->SetObjectName((GetName() + " (VkImage)").c_str(), VK_OBJECT_TYPE_IMAGE,
                                                           (uint64_t)mImage);
#endif
        CreateDefaultImageView();
        if ((GetUsage() & TextureUsageFlagBit::SAMPLED) != 0)
        {
            CreateDefaultSampler();
        }
        InitialTransitionToRestingLayout();
    }

    VulkanTexture::~VulkanTexture()
    {
        DestroyDefaultSampler();
        DestroyDefaultImageView();
        if (OwnsImage())
        {
            DestroyImage();
        }
    }

    void VulkanTexture::CreateImage()
    {
        VkImageCreateInfo imageCreateInfo;
        imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageCreateInfo.pNext = nullptr;
        imageCreateInfo.flags = GetVkImageCreateFlags(GetType());
        imageCreateInfo.imageType = GetVkImageType(GetType());
        imageCreateInfo.format = GetVulkanFormat();
        imageCreateInfo.extent = {GetWidth(), GetHeight(), GetDepth()};
        imageCreateInfo.mipLevels = GetMipCount();
        imageCreateInfo.arrayLayers = GetSlices();
        imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT; // multisampled texture not supported yet

        const auto *pFormatProperties =
            VulkanGraphicsSystem::GetInstance()->GetFormatProperties(imageCreateInfo.format);
        assert(pFormatProperties != nullptr);
        if ((pFormatProperties->optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT) != 0)
        {
            imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        }
        else if ((pFormatProperties->linearTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT) != 0)
        {
            imageCreateInfo.tiling = VK_IMAGE_TILING_LINEAR;
            // linearly tiled images should use mappable memory at the moment
            mUsesMappableMemory = true;
        }
        else
        {
            FASTCG_THROW_EXCEPTION(Exception, "Vulkan: No tiling features found for format %s (texture: %s)",
                                   GetVkFormatString(imageCreateInfo.format), mName.c_str());
        }

        imageCreateInfo.usage = GetVkImageUsageFlags(GetUsage(), GetFormat());
        imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

        VmaAllocationCreateInfo allocationCreateInfo;
        if (mUsesMappableMemory)
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
        allocationCreateInfo.priority = 0;

        FASTCG_CHECK_VK_RESULT(vmaCreateImage(VulkanGraphicsSystem::GetInstance()->GetAllocator(), &imageCreateInfo,
                                              &allocationCreateInfo, &mImage, &mAllocation, &mAllocationInfo));

        if (GetData() != nullptr)
        {
            VulkanGraphicsSystem::GetInstance()->GetImmediateGraphicsContext()->Copy(this, GetData(), GetDataSize());
        }
    }

    void VulkanTexture::DestroyImage()
    {
        if (mImage != VK_NULL_HANDLE)
        {
            vmaDestroyImage(VulkanGraphicsSystem::GetInstance()->GetAllocator(), mImage, mAllocation);
            mImage = VK_NULL_HANDLE;
            mAllocation = VK_NULL_HANDLE;
        }
    }

    void VulkanTexture::InitialTransitionToRestingLayout()
    {
        // assume no wait transition is safe
        VulkanGraphicsSystem::GetInstance()->GetImmediateGraphicsContext()->AddTextureMemoryBarrier(
            this, VK_IMAGE_LAYOUT_UNDEFINED, GetRestingLayout(), 0, VK_ACCESS_MEMORY_READ_BIT,
            VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT);
    }

    void VulkanTexture::CreateDefaultImageView()
    {
        VkImageViewCreateInfo imageViewCreateInfo;
        imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        imageViewCreateInfo.pNext = nullptr;
        imageViewCreateInfo.flags = 0;
        imageViewCreateInfo.image = mImage;
        imageViewCreateInfo.viewType = GetVkImageViewType(GetType());
        imageViewCreateInfo.format = GetVulkanFormat();
        imageViewCreateInfo.components =
            VkComponentMapping{VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY,
                               VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY};
        if (IsDepthFormat(GetFormat()))
        {
            // TODO: support stencil
            imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
        }
        else
        {
            imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        }
        imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
        imageViewCreateInfo.subresourceRange.levelCount = GetMipCount();
        imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
        imageViewCreateInfo.subresourceRange.layerCount = GetSlices();
        FASTCG_CHECK_VK_RESULT(vkCreateImageView(VulkanGraphicsSystem::GetInstance()->GetDevice(), &imageViewCreateInfo,
                                                 VulkanGraphicsSystem::GetInstance()->GetAllocationCallbacks(),
                                                 &mDefaultImageView));
#if _DEBUG
        VulkanGraphicsSystem::GetInstance()->SetObjectName((GetName() + " (VkImageView)").c_str(),
                                                           VK_OBJECT_TYPE_IMAGE_VIEW, (uint64_t)mDefaultImageView);
#endif
    }

    void VulkanTexture::CreateDefaultSampler()
    {
        VkSamplerCreateInfo samplerCreateInfo;
        samplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        samplerCreateInfo.pNext = nullptr;
        samplerCreateInfo.flags = 0;
        samplerCreateInfo.magFilter = samplerCreateInfo.minFilter = GetVkFilter(GetFilter());
        samplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
        samplerCreateInfo.addressModeU = samplerCreateInfo.addressModeV = samplerCreateInfo.addressModeW =
            GetVkAddressMode(GetWrapMode());
        samplerCreateInfo.mipLodBias = 0;
        samplerCreateInfo.anisotropyEnable = VK_FALSE;
        samplerCreateInfo.maxAnisotropy = 0;
        samplerCreateInfo.compareEnable = VK_FALSE;
        samplerCreateInfo.compareOp = VK_COMPARE_OP_NEVER;
        samplerCreateInfo.minLod = 0;
        samplerCreateInfo.maxLod = VK_LOD_CLAMP_NONE;
        samplerCreateInfo.borderColor = VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK;
        samplerCreateInfo.unnormalizedCoordinates = VK_FALSE;

        FASTCG_CHECK_VK_RESULT(vkCreateSampler(VulkanGraphicsSystem::GetInstance()->GetDevice(), &samplerCreateInfo,
                                               VulkanGraphicsSystem::GetInstance()->GetAllocationCallbacks(),
                                               &mDefaultSampler));

#if _DEBUG
        VulkanGraphicsSystem::GetInstance()->SetObjectName((GetName() + " (VkSampler)").c_str(), VK_OBJECT_TYPE_SAMPLER,
                                                           (uint64_t)mDefaultSampler);
#endif
    }

    void VulkanTexture::DestroyDefaultImageView()
    {
        if (mDefaultImageView != VK_NULL_HANDLE)
        {
            vkDestroyImageView(VulkanGraphicsSystem::GetInstance()->GetDevice(), mDefaultImageView,
                               VulkanGraphicsSystem::GetInstance()->GetAllocationCallbacks());
            mDefaultImageView = VK_NULL_HANDLE;
        }
    }

    void VulkanTexture::DestroyDefaultSampler()
    {
        if (mDefaultSampler != VK_NULL_HANDLE)
        {
            vkDestroySampler(VulkanGraphicsSystem::GetInstance()->GetDevice(), mDefaultSampler,
                             VulkanGraphicsSystem::GetInstance()->GetAllocationCallbacks());
            mDefaultSampler = VK_NULL_HANDLE;
        }
    }
}

#endif