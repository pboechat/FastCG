#ifndef FASTCG_VULKAN_TEXTURE_H
#define FASTCG_VULKAN_TEXTURE_H

#ifdef FASTCG_VULKAN

#include <FastCG/Graphics/Vulkan/Vulkan.h>
#include <FastCG/Graphics/Vulkan/VulkanUtils.h>
#include <FastCG/Graphics/BaseTexture.h>

namespace FastCG
{
    class VulkanGraphicsContext;
    class VulkanGraphicsSystem;

    class VulkanTexture : public BaseTexture
    {
    public:
        struct Args : BaseTexture::Args
        {
            VkImage image;

            Args(const std::string &rName = "",
                 uint32_t width = 1,
                 uint32_t height = 1,
                 uint32_t depthOrSlices = 1,
                 uint8_t mipCount = 1,
                 TextureType type = TextureType::TEXTURE_2D,
                 TextureUsageFlags usage = TextureUsageFlagBit::SAMPLED,
                 TextureFormat format = TextureFormat::R8G8B8A8_UNORM,
                 TextureFilter filter = TextureFilter::LINEAR_FILTER,
                 TextureWrapMode wrapMode = TextureWrapMode::CLAMP,
                 const uint8_t *pData = nullptr,
                 VkImage image = VK_NULL_HANDLE) : BaseTexture::Args(rName,
                                                                     width,
                                                                     height,
                                                                     depthOrSlices,
                                                                     mipCount,
                                                                     type,
                                                                     usage,
                                                                     format,
                                                                     filter,
                                                                     wrapMode,
                                                                     pData),
                                                   image(image)
            {
            }
        };

        VulkanTexture(const Args &rArgs);
        VulkanTexture(const VulkanTexture &rOther) = delete;
        VulkanTexture(const VulkanTexture &&rOther) = delete;
        virtual ~VulkanTexture();

        VulkanTexture operator=(const VulkanTexture &rOther) = delete;

    private:
        VkImage mImage;
        VmaAllocation mAllocation{VK_NULL_HANDLE};
        VmaAllocationInfo mAllocationInfo;
        VkImageView mDefaultImageView{VK_NULL_HANDLE};
        VkSampler mDefaultSampler{VK_NULL_HANDLE};
        bool mUsesMappableMemory{false};

        inline VkImage GetImage() const
        {
            return mImage;
        }
        inline bool OwnsImage() const
        {
            return mAllocation != VK_NULL_HANDLE;
        }
        inline VmaAllocation GetAllocation() const
        {
            return mAllocation;
        }
        inline VmaAllocationInfo GetAllocationInfo() const
        {
            return mAllocationInfo;
        }
        inline VkImageView GetDefaultImageView() const
        {
            return mDefaultImageView;
        }
        inline VkSampler GetDefaultSampler() const
        {
            return mDefaultSampler;
        }
        inline VkFormat GetVulkanFormat() const
        {
            return GetVkFormat(GetFormat());
        }
        inline VkImageAspectFlags GetAspectFlags() const
        {
            return GetVkImageAspectFlags(GetFormat());
        }
        inline VkImageLayout GetRestingLayout() const
        {
            return GetVkRestingLayout(GetUsage(), GetFormat());
        }
        inline VkAccessFlags GetDefaultAccessFlags() const
        {
            return GetVkImageAccessFlags(GetUsage(), GetFormat());
        }
        inline VkPipelineStageFlags GetDefaultStageFlags() const
        {
            return GetVkImagePipelineStageFlags(GetUsage());
        }
        inline bool UsesMappableMemory() const
        {
            return mUsesMappableMemory;
        }

        void CreateImage();
        void DestroyImage();
        void TransitionToRestingLayout();
        void CreateDefaultImageView();
        void CreateDefaultSampler();
        void DestroyDefaultImageView();
        void DestroyDefaultSampler();

        friend class VulkanGraphicsContext;
        friend class VulkanGraphicsSystem;
    };
}

#endif

#endif