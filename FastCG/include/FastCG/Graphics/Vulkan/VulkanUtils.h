#ifndef FASTCG_VULKAN_UTILS_H
#define FASTCG_VULKAN_UTILS_H

#ifdef FASTCG_VULKAN

#include <FastCG/Core/Exception.h>
#include <FastCG/Graphics/GraphicsUtils.h>
#include <FastCG/Graphics/Vulkan/Vulkan.h>

#include <algorithm>

#ifdef CASE_RETURN_STRING
#undef CASE_RETURN_STRING
#endif

#define CASE_RETURN_STRING(str)                                                                                        \
    case str:                                                                                                          \
        return #str

namespace FastCG
{
    inline const char *GetVkResultString(VkResult vkResult)
    {
        switch (vkResult)
        {
            CASE_RETURN_STRING(VK_SUCCESS);
            CASE_RETURN_STRING(VK_NOT_READY);
            CASE_RETURN_STRING(VK_TIMEOUT);
            CASE_RETURN_STRING(VK_EVENT_SET);
            CASE_RETURN_STRING(VK_EVENT_RESET);
            CASE_RETURN_STRING(VK_INCOMPLETE);
            CASE_RETURN_STRING(VK_ERROR_OUT_OF_HOST_MEMORY);
            CASE_RETURN_STRING(VK_ERROR_OUT_OF_DEVICE_MEMORY);
            CASE_RETURN_STRING(VK_ERROR_INITIALIZATION_FAILED);
            CASE_RETURN_STRING(VK_ERROR_DEVICE_LOST);
            CASE_RETURN_STRING(VK_ERROR_MEMORY_MAP_FAILED);
            CASE_RETURN_STRING(VK_ERROR_LAYER_NOT_PRESENT);
            CASE_RETURN_STRING(VK_ERROR_EXTENSION_NOT_PRESENT);
            CASE_RETURN_STRING(VK_ERROR_FEATURE_NOT_PRESENT);
            CASE_RETURN_STRING(VK_ERROR_INCOMPATIBLE_DRIVER);
            CASE_RETURN_STRING(VK_ERROR_TOO_MANY_OBJECTS);
            CASE_RETURN_STRING(VK_ERROR_FORMAT_NOT_SUPPORTED);
            CASE_RETURN_STRING(VK_ERROR_FRAGMENTED_POOL);
            CASE_RETURN_STRING(VK_ERROR_OUT_OF_POOL_MEMORY);
            CASE_RETURN_STRING(VK_ERROR_INVALID_EXTERNAL_HANDLE);
            CASE_RETURN_STRING(VK_ERROR_SURFACE_LOST_KHR);
            CASE_RETURN_STRING(VK_ERROR_NATIVE_WINDOW_IN_USE_KHR);
            CASE_RETURN_STRING(VK_SUBOPTIMAL_KHR);
            CASE_RETURN_STRING(VK_ERROR_OUT_OF_DATE_KHR);
            CASE_RETURN_STRING(VK_ERROR_INCOMPATIBLE_DISPLAY_KHR);
            CASE_RETURN_STRING(VK_ERROR_VALIDATION_FAILED_EXT);
            CASE_RETURN_STRING(VK_ERROR_INVALID_SHADER_NV);
            CASE_RETURN_STRING(VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT);
            CASE_RETURN_STRING(VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT);
#if !defined FASTCG_ANDROID
            // FIXME: Vulkan header in NDK 21 doesn't define VK_ERROR_UNKNOWN
            CASE_RETURN_STRING(VK_ERROR_UNKNOWN);
#endif
#if defined VK_API_VERSION_1_2
            CASE_RETURN_STRING(VK_ERROR_FRAGMENTATION);
#endif
        default:
            FASTCG_THROW_EXCEPTION(Exception, "Couldn't get a Vk result string (vkResult: %d)", (int)vkResult);
            return nullptr;
        }
    }

    inline const char *GetVkDebugUtilsMessageSeverityFlagBitsString(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity)
    {
        switch (messageSeverity)
        {
            CASE_RETURN_STRING(VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT);
            CASE_RETURN_STRING(VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT);
            CASE_RETURN_STRING(VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT);
            CASE_RETURN_STRING(VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT);
        default:
            FASTCG_THROW_EXCEPTION(
                Exception, "Couldn't get a Vk debug utils message severity flag bits string (messageSeverity: %d)",
                (int)messageSeverity);
            return nullptr;
        }
    }

    inline const char *GetVkDebugUtilsMessageTypeFlagBitsString(VkDebugUtilsMessageTypeFlagBitsEXT messageType)
    {
        switch (messageType)
        {
            CASE_RETURN_STRING(VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT);
            CASE_RETURN_STRING(VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT);
            CASE_RETURN_STRING(VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT);
        default:
            FASTCG_THROW_EXCEPTION(Exception,
                                   "Couldn't get a Vk debug utils message type flag bits string (messageType: %d)",
                                   (int)messageType);
            return nullptr;
        }
    }

    inline const char *GetVkFormatString(VkFormat format)
    {
        switch (format)
        {
            CASE_RETURN_STRING(VK_FORMAT_UNDEFINED);
            CASE_RETURN_STRING(VK_FORMAT_R4G4_UNORM_PACK8);
            CASE_RETURN_STRING(VK_FORMAT_R4G4B4A4_UNORM_PACK16);
            CASE_RETURN_STRING(VK_FORMAT_B4G4R4A4_UNORM_PACK16);
            CASE_RETURN_STRING(VK_FORMAT_R5G6B5_UNORM_PACK16);
            CASE_RETURN_STRING(VK_FORMAT_B5G6R5_UNORM_PACK16);
            CASE_RETURN_STRING(VK_FORMAT_R5G5B5A1_UNORM_PACK16);
            CASE_RETURN_STRING(VK_FORMAT_B5G5R5A1_UNORM_PACK16);
            CASE_RETURN_STRING(VK_FORMAT_A1R5G5B5_UNORM_PACK16);
            CASE_RETURN_STRING(VK_FORMAT_R8_UNORM);
            CASE_RETURN_STRING(VK_FORMAT_R8_SNORM);
            CASE_RETURN_STRING(VK_FORMAT_R8_USCALED);
            CASE_RETURN_STRING(VK_FORMAT_R8_SSCALED);
            CASE_RETURN_STRING(VK_FORMAT_R8_UINT);
            CASE_RETURN_STRING(VK_FORMAT_R8_SINT);
            CASE_RETURN_STRING(VK_FORMAT_R8_SRGB);
            CASE_RETURN_STRING(VK_FORMAT_R8G8_UNORM);
            CASE_RETURN_STRING(VK_FORMAT_R8G8_SNORM);
            CASE_RETURN_STRING(VK_FORMAT_R8G8_USCALED);
            CASE_RETURN_STRING(VK_FORMAT_R8G8_SSCALED);
            CASE_RETURN_STRING(VK_FORMAT_R8G8_UINT);
            CASE_RETURN_STRING(VK_FORMAT_R8G8_SINT);
            CASE_RETURN_STRING(VK_FORMAT_R8G8_SRGB);
            CASE_RETURN_STRING(VK_FORMAT_R8G8B8_UNORM);
            CASE_RETURN_STRING(VK_FORMAT_R8G8B8_SNORM);
            CASE_RETURN_STRING(VK_FORMAT_R8G8B8_USCALED);
            CASE_RETURN_STRING(VK_FORMAT_R8G8B8_SSCALED);
            CASE_RETURN_STRING(VK_FORMAT_R8G8B8_UINT);
            CASE_RETURN_STRING(VK_FORMAT_R8G8B8_SINT);
            CASE_RETURN_STRING(VK_FORMAT_R8G8B8_SRGB);
            CASE_RETURN_STRING(VK_FORMAT_B8G8R8_UNORM);
            CASE_RETURN_STRING(VK_FORMAT_B8G8R8_SNORM);
            CASE_RETURN_STRING(VK_FORMAT_B8G8R8_USCALED);
            CASE_RETURN_STRING(VK_FORMAT_B8G8R8_SSCALED);
            CASE_RETURN_STRING(VK_FORMAT_B8G8R8_UINT);
            CASE_RETURN_STRING(VK_FORMAT_B8G8R8_SINT);
            CASE_RETURN_STRING(VK_FORMAT_B8G8R8_SRGB);
            CASE_RETURN_STRING(VK_FORMAT_R8G8B8A8_UNORM);
            CASE_RETURN_STRING(VK_FORMAT_R8G8B8A8_SNORM);
            CASE_RETURN_STRING(VK_FORMAT_R8G8B8A8_USCALED);
            CASE_RETURN_STRING(VK_FORMAT_R8G8B8A8_SSCALED);
            CASE_RETURN_STRING(VK_FORMAT_R8G8B8A8_UINT);
            CASE_RETURN_STRING(VK_FORMAT_R8G8B8A8_SINT);
            CASE_RETURN_STRING(VK_FORMAT_R8G8B8A8_SRGB);
            CASE_RETURN_STRING(VK_FORMAT_B8G8R8A8_UNORM);
            CASE_RETURN_STRING(VK_FORMAT_B8G8R8A8_SNORM);
            CASE_RETURN_STRING(VK_FORMAT_B8G8R8A8_USCALED);
            CASE_RETURN_STRING(VK_FORMAT_B8G8R8A8_SSCALED);
            CASE_RETURN_STRING(VK_FORMAT_B8G8R8A8_UINT);
            CASE_RETURN_STRING(VK_FORMAT_B8G8R8A8_SINT);
            CASE_RETURN_STRING(VK_FORMAT_B8G8R8A8_SRGB);
            CASE_RETURN_STRING(VK_FORMAT_A8B8G8R8_UNORM_PACK32);
            CASE_RETURN_STRING(VK_FORMAT_A8B8G8R8_SNORM_PACK32);
            CASE_RETURN_STRING(VK_FORMAT_A8B8G8R8_USCALED_PACK32);
            CASE_RETURN_STRING(VK_FORMAT_A8B8G8R8_SSCALED_PACK32);
            CASE_RETURN_STRING(VK_FORMAT_A8B8G8R8_UINT_PACK32);
            CASE_RETURN_STRING(VK_FORMAT_A8B8G8R8_SINT_PACK32);
            CASE_RETURN_STRING(VK_FORMAT_A8B8G8R8_SRGB_PACK32);
            CASE_RETURN_STRING(VK_FORMAT_A2R10G10B10_UNORM_PACK32);
            CASE_RETURN_STRING(VK_FORMAT_A2R10G10B10_SNORM_PACK32);
            CASE_RETURN_STRING(VK_FORMAT_A2R10G10B10_USCALED_PACK32);
            CASE_RETURN_STRING(VK_FORMAT_A2R10G10B10_SSCALED_PACK32);
            CASE_RETURN_STRING(VK_FORMAT_A2R10G10B10_UINT_PACK32);
            CASE_RETURN_STRING(VK_FORMAT_A2R10G10B10_SINT_PACK32);
            CASE_RETURN_STRING(VK_FORMAT_A2B10G10R10_UNORM_PACK32);
            CASE_RETURN_STRING(VK_FORMAT_A2B10G10R10_SNORM_PACK32);
            CASE_RETURN_STRING(VK_FORMAT_A2B10G10R10_USCALED_PACK32);
            CASE_RETURN_STRING(VK_FORMAT_A2B10G10R10_SSCALED_PACK32);
            CASE_RETURN_STRING(VK_FORMAT_A2B10G10R10_UINT_PACK32);
            CASE_RETURN_STRING(VK_FORMAT_A2B10G10R10_SINT_PACK32);
            CASE_RETURN_STRING(VK_FORMAT_R16_UNORM);
            CASE_RETURN_STRING(VK_FORMAT_R16_SNORM);
            CASE_RETURN_STRING(VK_FORMAT_R16_USCALED);
            CASE_RETURN_STRING(VK_FORMAT_R16_SSCALED);
            CASE_RETURN_STRING(VK_FORMAT_R16_UINT);
            CASE_RETURN_STRING(VK_FORMAT_R16_SINT);
            CASE_RETURN_STRING(VK_FORMAT_R16_SFLOAT);
            CASE_RETURN_STRING(VK_FORMAT_R16G16_UNORM);
            CASE_RETURN_STRING(VK_FORMAT_R16G16_SNORM);
            CASE_RETURN_STRING(VK_FORMAT_R16G16_USCALED);
            CASE_RETURN_STRING(VK_FORMAT_R16G16_SSCALED);
            CASE_RETURN_STRING(VK_FORMAT_R16G16_UINT);
            CASE_RETURN_STRING(VK_FORMAT_R16G16_SINT);
            CASE_RETURN_STRING(VK_FORMAT_R16G16_SFLOAT);
            CASE_RETURN_STRING(VK_FORMAT_R16G16B16_UNORM);
            CASE_RETURN_STRING(VK_FORMAT_R16G16B16_SNORM);
            CASE_RETURN_STRING(VK_FORMAT_R16G16B16_USCALED);
            CASE_RETURN_STRING(VK_FORMAT_R16G16B16_SSCALED);
            CASE_RETURN_STRING(VK_FORMAT_R16G16B16_UINT);
            CASE_RETURN_STRING(VK_FORMAT_R16G16B16_SINT);
            CASE_RETURN_STRING(VK_FORMAT_R16G16B16_SFLOAT);
            CASE_RETURN_STRING(VK_FORMAT_R16G16B16A16_UNORM);
            CASE_RETURN_STRING(VK_FORMAT_R16G16B16A16_SNORM);
            CASE_RETURN_STRING(VK_FORMAT_R16G16B16A16_USCALED);
            CASE_RETURN_STRING(VK_FORMAT_R16G16B16A16_SSCALED);
            CASE_RETURN_STRING(VK_FORMAT_R16G16B16A16_UINT);
            CASE_RETURN_STRING(VK_FORMAT_R16G16B16A16_SINT);
            CASE_RETURN_STRING(VK_FORMAT_R16G16B16A16_SFLOAT);
            CASE_RETURN_STRING(VK_FORMAT_R32_UINT);
            CASE_RETURN_STRING(VK_FORMAT_R32_SINT);
            CASE_RETURN_STRING(VK_FORMAT_R32_SFLOAT);
            CASE_RETURN_STRING(VK_FORMAT_R32G32_UINT);
            CASE_RETURN_STRING(VK_FORMAT_R32G32_SINT);
            CASE_RETURN_STRING(VK_FORMAT_R32G32_SFLOAT);
            CASE_RETURN_STRING(VK_FORMAT_R32G32B32_UINT);
            CASE_RETURN_STRING(VK_FORMAT_R32G32B32_SINT);
            CASE_RETURN_STRING(VK_FORMAT_R32G32B32_SFLOAT);
            CASE_RETURN_STRING(VK_FORMAT_R32G32B32A32_UINT);
            CASE_RETURN_STRING(VK_FORMAT_R32G32B32A32_SINT);
            CASE_RETURN_STRING(VK_FORMAT_R32G32B32A32_SFLOAT);
            CASE_RETURN_STRING(VK_FORMAT_R64_UINT);
            CASE_RETURN_STRING(VK_FORMAT_R64_SINT);
            CASE_RETURN_STRING(VK_FORMAT_R64_SFLOAT);
            CASE_RETURN_STRING(VK_FORMAT_R64G64_UINT);
            CASE_RETURN_STRING(VK_FORMAT_R64G64_SINT);
            CASE_RETURN_STRING(VK_FORMAT_R64G64_SFLOAT);
            CASE_RETURN_STRING(VK_FORMAT_R64G64B64_UINT);
            CASE_RETURN_STRING(VK_FORMAT_R64G64B64_SINT);
            CASE_RETURN_STRING(VK_FORMAT_R64G64B64_SFLOAT);
            CASE_RETURN_STRING(VK_FORMAT_R64G64B64A64_UINT);
            CASE_RETURN_STRING(VK_FORMAT_R64G64B64A64_SINT);
            CASE_RETURN_STRING(VK_FORMAT_R64G64B64A64_SFLOAT);
            CASE_RETURN_STRING(VK_FORMAT_B10G11R11_UFLOAT_PACK32);
            CASE_RETURN_STRING(VK_FORMAT_E5B9G9R9_UFLOAT_PACK32);
            CASE_RETURN_STRING(VK_FORMAT_D16_UNORM);
            CASE_RETURN_STRING(VK_FORMAT_X8_D24_UNORM_PACK32);
            CASE_RETURN_STRING(VK_FORMAT_D32_SFLOAT);
            CASE_RETURN_STRING(VK_FORMAT_S8_UINT);
            CASE_RETURN_STRING(VK_FORMAT_D16_UNORM_S8_UINT);
            CASE_RETURN_STRING(VK_FORMAT_D24_UNORM_S8_UINT);
            CASE_RETURN_STRING(VK_FORMAT_D32_SFLOAT_S8_UINT);
            CASE_RETURN_STRING(VK_FORMAT_BC1_RGB_UNORM_BLOCK);
            CASE_RETURN_STRING(VK_FORMAT_BC1_RGB_SRGB_BLOCK);
            CASE_RETURN_STRING(VK_FORMAT_BC1_RGBA_UNORM_BLOCK);
            CASE_RETURN_STRING(VK_FORMAT_BC1_RGBA_SRGB_BLOCK);
            CASE_RETURN_STRING(VK_FORMAT_BC2_UNORM_BLOCK);
            CASE_RETURN_STRING(VK_FORMAT_BC2_SRGB_BLOCK);
            CASE_RETURN_STRING(VK_FORMAT_BC3_UNORM_BLOCK);
            CASE_RETURN_STRING(VK_FORMAT_BC3_SRGB_BLOCK);
            CASE_RETURN_STRING(VK_FORMAT_BC4_UNORM_BLOCK);
            CASE_RETURN_STRING(VK_FORMAT_BC4_SNORM_BLOCK);
            CASE_RETURN_STRING(VK_FORMAT_BC5_UNORM_BLOCK);
            CASE_RETURN_STRING(VK_FORMAT_BC5_SNORM_BLOCK);
            CASE_RETURN_STRING(VK_FORMAT_BC6H_UFLOAT_BLOCK);
            CASE_RETURN_STRING(VK_FORMAT_BC6H_SFLOAT_BLOCK);
            CASE_RETURN_STRING(VK_FORMAT_BC7_UNORM_BLOCK);
            CASE_RETURN_STRING(VK_FORMAT_BC7_SRGB_BLOCK);
            CASE_RETURN_STRING(VK_FORMAT_ETC2_R8G8B8_UNORM_BLOCK);
            CASE_RETURN_STRING(VK_FORMAT_ETC2_R8G8B8_SRGB_BLOCK);
            CASE_RETURN_STRING(VK_FORMAT_ETC2_R8G8B8A1_UNORM_BLOCK);
            CASE_RETURN_STRING(VK_FORMAT_ETC2_R8G8B8A1_SRGB_BLOCK);
            CASE_RETURN_STRING(VK_FORMAT_ETC2_R8G8B8A8_UNORM_BLOCK);
            CASE_RETURN_STRING(VK_FORMAT_ETC2_R8G8B8A8_SRGB_BLOCK);
            CASE_RETURN_STRING(VK_FORMAT_EAC_R11_UNORM_BLOCK);
            CASE_RETURN_STRING(VK_FORMAT_EAC_R11_SNORM_BLOCK);
            CASE_RETURN_STRING(VK_FORMAT_EAC_R11G11_UNORM_BLOCK);
            CASE_RETURN_STRING(VK_FORMAT_EAC_R11G11_SNORM_BLOCK);
            CASE_RETURN_STRING(VK_FORMAT_ASTC_4x4_UNORM_BLOCK);
            CASE_RETURN_STRING(VK_FORMAT_ASTC_4x4_SRGB_BLOCK);
            CASE_RETURN_STRING(VK_FORMAT_ASTC_5x4_UNORM_BLOCK);
            CASE_RETURN_STRING(VK_FORMAT_ASTC_5x4_SRGB_BLOCK);
            CASE_RETURN_STRING(VK_FORMAT_ASTC_5x5_UNORM_BLOCK);
            CASE_RETURN_STRING(VK_FORMAT_ASTC_5x5_SRGB_BLOCK);
            CASE_RETURN_STRING(VK_FORMAT_ASTC_6x5_UNORM_BLOCK);
            CASE_RETURN_STRING(VK_FORMAT_ASTC_6x5_SRGB_BLOCK);
            CASE_RETURN_STRING(VK_FORMAT_ASTC_6x6_UNORM_BLOCK);
            CASE_RETURN_STRING(VK_FORMAT_ASTC_6x6_SRGB_BLOCK);
            CASE_RETURN_STRING(VK_FORMAT_ASTC_8x5_UNORM_BLOCK);
            CASE_RETURN_STRING(VK_FORMAT_ASTC_8x5_SRGB_BLOCK);
            CASE_RETURN_STRING(VK_FORMAT_ASTC_8x6_UNORM_BLOCK);
            CASE_RETURN_STRING(VK_FORMAT_ASTC_8x6_SRGB_BLOCK);
            CASE_RETURN_STRING(VK_FORMAT_ASTC_8x8_UNORM_BLOCK);
            CASE_RETURN_STRING(VK_FORMAT_ASTC_8x8_SRGB_BLOCK);
            CASE_RETURN_STRING(VK_FORMAT_ASTC_10x5_UNORM_BLOCK);
            CASE_RETURN_STRING(VK_FORMAT_ASTC_10x5_SRGB_BLOCK);
            CASE_RETURN_STRING(VK_FORMAT_ASTC_10x6_UNORM_BLOCK);
            CASE_RETURN_STRING(VK_FORMAT_ASTC_10x6_SRGB_BLOCK);
            CASE_RETURN_STRING(VK_FORMAT_ASTC_10x8_UNORM_BLOCK);
            CASE_RETURN_STRING(VK_FORMAT_ASTC_10x8_SRGB_BLOCK);
            CASE_RETURN_STRING(VK_FORMAT_ASTC_10x10_UNORM_BLOCK);
            CASE_RETURN_STRING(VK_FORMAT_ASTC_10x10_SRGB_BLOCK);
            CASE_RETURN_STRING(VK_FORMAT_ASTC_12x10_UNORM_BLOCK);
            CASE_RETURN_STRING(VK_FORMAT_ASTC_12x10_SRGB_BLOCK);
            CASE_RETURN_STRING(VK_FORMAT_ASTC_12x12_UNORM_BLOCK);
            CASE_RETURN_STRING(VK_FORMAT_ASTC_12x12_SRGB_BLOCK);
        default:
            FASTCG_THROW_EXCEPTION(Exception, "Couldn't get a Vk format string (format: %d)", (int)format);
            return nullptr;
        }
    }

    inline const char *GetVkImageLayoutString(VkImageLayout layout)
    {
        switch (layout)
        {
            CASE_RETURN_STRING(VK_IMAGE_LAYOUT_UNDEFINED);
            CASE_RETURN_STRING(VK_IMAGE_LAYOUT_GENERAL);
            CASE_RETURN_STRING(VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
            CASE_RETURN_STRING(VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
            CASE_RETURN_STRING(VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL);
            CASE_RETURN_STRING(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
            CASE_RETURN_STRING(VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
            CASE_RETURN_STRING(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
            CASE_RETURN_STRING(VK_IMAGE_LAYOUT_PREINITIALIZED);
            CASE_RETURN_STRING(VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL);
            CASE_RETURN_STRING(VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL);
            CASE_RETURN_STRING(VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
#if defined VK_API_VERSION_1_2
            CASE_RETURN_STRING(VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL);
            CASE_RETURN_STRING(VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_OPTIMAL);
            CASE_RETURN_STRING(VK_IMAGE_LAYOUT_STENCIL_ATTACHMENT_OPTIMAL);
            CASE_RETURN_STRING(VK_IMAGE_LAYOUT_STENCIL_READ_ONLY_OPTIMAL);
#endif
#if defined VK_API_VERSION_1_3
            CASE_RETURN_STRING(VK_IMAGE_LAYOUT_READ_ONLY_OPTIMAL);
            CASE_RETURN_STRING(VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL);
#endif
        default:
            FASTCG_THROW_EXCEPTION(Exception, "Couldn't get a Vk image layout string (layout: %d)", (int)layout);
            return nullptr;
        }
    }

    inline VkImageCreateFlags GetVkImageCreateFlags(TextureType type)
    {
        VkImageCreateFlags createFlags = 0;
        if (type == TextureType::TEXTURE_CUBE_MAP)
        {
            createFlags |= VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
        }
        if (type == TextureType::TEXTURE_2D_ARRAY)
        {
            createFlags |= VK_IMAGE_CREATE_2D_ARRAY_COMPATIBLE_BIT;
        }
        return createFlags;
    }

    inline VkImageType GetVkImageType(TextureType type)
    {
        switch (type)
        {
        case TextureType::TEXTURE_1D:
            return VK_IMAGE_TYPE_1D;
        case TextureType::TEXTURE_2D:
        case TextureType::TEXTURE_CUBE_MAP:
        case TextureType::TEXTURE_2D_ARRAY:
            return VK_IMAGE_TYPE_2D;
        case TextureType::TEXTURE_3D:
            return VK_IMAGE_TYPE_3D;
        default:
            FASTCG_THROW_EXCEPTION(Exception, "Couldn't get a Vk image type (textureType: %s)",
                                   GetTextureTypeString(type));
            return (VkImageType)0;
        }
    }

    inline VkImageViewType GetVkImageViewType(TextureType type)
    {
        switch (type)
        {
        case TextureType::TEXTURE_1D:
            return VK_IMAGE_VIEW_TYPE_1D;
        case TextureType::TEXTURE_2D:
            return VK_IMAGE_VIEW_TYPE_2D;
        case TextureType::TEXTURE_CUBE_MAP:
            return VK_IMAGE_VIEW_TYPE_CUBE;
        case TextureType::TEXTURE_2D_ARRAY:
            return VK_IMAGE_VIEW_TYPE_2D_ARRAY;
        case TextureType::TEXTURE_3D:
            return VK_IMAGE_VIEW_TYPE_3D;
        default:
            FASTCG_THROW_EXCEPTION(Exception, "Couldn't get a Vk image type (textureType: %s)",
                                   GetTextureTypeString(type));
            return (VkImageViewType)0;
        }
    }

    inline VkImageUsageFlags GetVkImageUsageFlags(TextureUsageFlags usage, TextureFormat format)
    {
        VkImageUsageFlags usageFlags = 0;
        if ((usage & TextureUsageFlagBit::SAMPLED) != 0)
        {
            usageFlags |= VK_IMAGE_USAGE_SAMPLED_BIT;
        }
        if ((usage & TextureUsageFlagBit::RENDER_TARGET) != 0)
        {
            if (IsDepthFormat(format))
            {
                usageFlags |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
            }
            else
            {
                usageFlags |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
            }
        }
        usageFlags |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
        usageFlags |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
        return usageFlags;
    }

    inline VkImageAspectFlags GetVkImageAspectFlags(TextureFormat format)
    {
        if (IsColorFormat(format))
        {
            return VK_IMAGE_ASPECT_COLOR_BIT;
        }
        else if (IsDepthFormat(format))
        {
            if (HasStencil(format))
            {
                return VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
            }
            else
            {
                return VK_IMAGE_ASPECT_DEPTH_BIT;
            }
        }
        FASTCG_THROW_EXCEPTION(Exception, "Couldn't get a Vk image aspect flag (textureFormat: %s)",
                               GetTextureFormatString(format));
        return (VkImageAspectFlags)0;
    }

    inline VkImageLayout GetVkRestingLayout(TextureUsageFlags usage, TextureFormat format)
    {
        if ((usage & TextureUsageFlagBit::PRESENT) != 0)
        {
            return VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        }
        else if ((usage & TextureUsageFlagBit::RENDER_TARGET) != 0)
        {
            if (IsDepthFormat(format))
            {
                return VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
            }
            else
            {
                return VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            }
        }
        else if ((usage & TextureUsageFlagBit::SAMPLED) != 0)
        {
            return VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        }
        else
        {
            return VK_IMAGE_LAYOUT_GENERAL;
        }
    }

    inline VkAccessFlags GetVkImageAccessFlags(TextureUsageFlags usage, TextureFormat format)
    {
        VkAccessFlags accessFlags = 0;
        if ((usage & TextureUsageFlagBit::SAMPLED) != 0)
        {
            accessFlags |= VK_ACCESS_SHADER_READ_BIT;
        }
        if ((usage & TextureUsageFlagBit::RENDER_TARGET) != 0)
        {
            if (IsDepthFormat(format))
            {
                accessFlags |= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
            }
            else
            {
                accessFlags |= VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
            }
        }
        if ((usage & TextureUsageFlagBit::PRESENT) != 0)
        {
            accessFlags |= VK_ACCESS_MEMORY_READ_BIT;
        }
        return accessFlags;
    }

    inline VkPipelineStageFlags GetVkImagePipelineStageFlags(TextureUsageFlags usage)
    {
        VkPipelineStageFlags stageFlags = 0;
        if ((usage & TextureUsageFlagBit::SAMPLED) != 0)
        {
            stageFlags |= VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        }
        if ((usage & TextureUsageFlagBit::RENDER_TARGET) != 0)
        {
            stageFlags |= VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        }
        if ((usage & TextureUsageFlagBit::PRESENT) != 0)
        {
            stageFlags |= VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT;
        }
        return stageFlags;
    }

    inline VkBufferUsageFlags GetVkBufferUsageFlags(BufferUsageFlags usage)
    {
        VkBufferUsageFlags usageFlags = 0;
        if ((usage & BufferUsageFlagBit::UNIFORM) != 0)
        {
            usageFlags |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
        }
        if ((usage & BufferUsageFlagBit::SHADER_STORAGE) != 0)
        {
            usageFlags |= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
        }
        if ((usage & BufferUsageFlagBit::VERTEX_BUFFER) != 0)
        {
            usageFlags |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
        }
        if ((usage & BufferUsageFlagBit::INDEX_BUFFER) != 0)
        {
            usageFlags |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
        }
        usageFlags |= VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
        usageFlags |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;
        return usageFlags;
    }

    inline VkPipelineStageFlags GetVkBufferPipelineStageFlags(BufferUsageFlags usage)
    {
        VkPipelineStageFlags stageFlags = 0;
        if ((usage & BufferUsageFlagBit::UNIFORM) != 0 || (usage & BufferUsageFlagBit::SHADER_STORAGE) != 0)
        {
            stageFlags |= VK_PIPELINE_STAGE_VERTEX_SHADER_BIT | VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        }
        if ((usage & BufferUsageFlagBit::INDEX_BUFFER) != 0 || (usage & BufferUsageFlagBit::VERTEX_BUFFER) != 0)
        {
            stageFlags |= VK_PIPELINE_STAGE_VERTEX_INPUT_BIT;
        }
        if ((usage & BufferUsageFlagBit::DYNAMIC) != 0)
        {
            stageFlags |= VK_PIPELINE_STAGE_TRANSFER_BIT;
        }
        assert(stageFlags != 0);
        return stageFlags;
    }

    inline VkAccessFlags GetVkBufferAccessFlags(BufferUsageFlags usage)
    {
        VkAccessFlags accessFlags = 0;
        if ((usage & BufferUsageFlagBit::INDEX_BUFFER) != 0)
        {
            accessFlags |= VK_ACCESS_INDEX_READ_BIT;
        }
        if ((usage & BufferUsageFlagBit::SHADER_STORAGE) != 0)
        {
            accessFlags |= VK_ACCESS_UNIFORM_READ_BIT | VK_ACCESS_SHADER_READ_BIT;
        }
        if ((usage & BufferUsageFlagBit::DYNAMIC) != 0)
        {
            accessFlags |= VK_ACCESS_TRANSFER_READ_BIT;
        }
        if ((usage & BufferUsageFlagBit::UNIFORM) != 0)
        {
            accessFlags |= VK_ACCESS_UNIFORM_READ_BIT | VK_ACCESS_SHADER_READ_BIT;
        }
        if ((usage & BufferUsageFlagBit::VERTEX_BUFFER) != 0)
        {
            accessFlags |= VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT;
        }
        assert(accessFlags != 0);
        return accessFlags;
    }

    inline VkShaderStageFlagBits GetVkShaderStageFlagBit(ShaderType shaderType)
    {
        switch (shaderType)
        {
        case ShaderType::FRAGMENT:
            return VK_SHADER_STAGE_FRAGMENT_BIT;
        case ShaderType::VERTEX:
            return VK_SHADER_STAGE_VERTEX_BIT;
        case ShaderType::COMPUTE:
            return VK_SHADER_STAGE_COMPUTE_BIT;
        default:
            FASTCG_THROW_EXCEPTION(Exception, "Vulkan: Couldn't get a Vk stage flag bit (shaderType: %s)",
                                   GetShaderTypeString(shaderType));
            return (VkShaderStageFlagBits)0;
        }
    }

    inline VkCullModeFlags GetVkCullModeFlags(Face cullMode)
    {
        switch (cullMode)
        {
        case Face::NONE:
            return (VkCullModeFlags)0;
        case Face::FRONT:
            return VK_CULL_MODE_FRONT_BIT;
        case Face::BACK:
            return VK_CULL_MODE_BACK_BIT;
        case Face::FRONT_AND_BACK:
            return VK_CULL_MODE_FRONT_BIT | VK_CULL_MODE_BACK_BIT;
        default:
            FASTCG_THROW_EXCEPTION(Exception, "Vulkan: Couldn't get Vk cull mode flags (cullMode: %s)",
                                   GetFaceString(cullMode));
            return (VkCullModeFlags)0;
        }
    }

    inline VkCompareOp GetVkCompareOp(CompareOp compareOp)
    {
        switch (compareOp)
        {
        case CompareOp::ALWAYS:
            return VK_COMPARE_OP_ALWAYS;
        case CompareOp::EQUAL:
            return VK_COMPARE_OP_EQUAL;
        case CompareOp::GEQUAL:
            return VK_COMPARE_OP_GREATER_OR_EQUAL;
        case CompareOp::GREATER:
            return VK_COMPARE_OP_GREATER;
        case CompareOp::LEQUAL:
            return VK_COMPARE_OP_LESS_OR_EQUAL;
        case CompareOp::LESS:
            return VK_COMPARE_OP_LESS;
        case CompareOp::NEVER:
            return VK_COMPARE_OP_NEVER;
        case CompareOp::NOT_EQUAL:
            return VK_COMPARE_OP_NOT_EQUAL;
        default:
            FASTCG_THROW_EXCEPTION(Exception, "Vulkan: Couldn't get Vk compare op (compareOp: %s)",
                                   GetCompareOpString(compareOp));
            return (VkCompareOp)0;
        }
    }

    inline VkStencilOp GetVkStencilOp(StencilOp stencilOp)
    {
        switch (stencilOp)
        {
        case StencilOp::DECREMENT_AND_CLAMP:
            return VK_STENCIL_OP_DECREMENT_AND_CLAMP;
        case StencilOp::DECREMENT_AND_WRAP:
            return VK_STENCIL_OP_DECREMENT_AND_WRAP;
        case StencilOp::INCREMENT_AND_CLAMP:
            return VK_STENCIL_OP_INCREMENT_AND_CLAMP;
        case StencilOp::INCREMENT_AND_WRAP:
            return VK_STENCIL_OP_INCREMENT_AND_WRAP;
        case StencilOp::INVERT:
            return VK_STENCIL_OP_INVERT;
        case StencilOp::KEEP:
            return VK_STENCIL_OP_KEEP;
        case StencilOp::REPLACE:
            return VK_STENCIL_OP_REPLACE;
        case StencilOp::ZERO:
            return VK_STENCIL_OP_ZERO;
        default:
            FASTCG_THROW_EXCEPTION(Exception, "Vulkan: Couldn't get Vk stencil op (stencilOp %s)",
                                   GetStencilOpString(stencilOp));
            return (VkStencilOp)0;
        }
    }

    inline VkBlendOp GetVkBlendOp(BlendFunc blendFunc)
    {
        switch (blendFunc)
        {
        case BlendFunc::NONE:
            return (VkBlendOp)0;
        case BlendFunc::ADD:
            return VK_BLEND_OP_ADD;
        default:
            FASTCG_THROW_EXCEPTION(Exception, "Vulkan: Couldn't get a Vk blend op (blendFunc: %s)",
                                   GetBlendFuncString(blendFunc));
            return (VkBlendOp)0;
        }
    }

    inline VkBlendFactor GetVkBlendFactor(BlendFactor blendFactor)
    {
        switch (blendFactor)
        {
        case BlendFactor::ZERO:
            return VK_BLEND_FACTOR_ZERO;
        case BlendFactor::ONE:
            return VK_BLEND_FACTOR_ONE;
        case BlendFactor::SRC_COLOR:
            return VK_BLEND_FACTOR_SRC_COLOR;
        case BlendFactor::DST_COLOR:
            return VK_BLEND_FACTOR_DST_COLOR;
        case BlendFactor::SRC_ALPHA:
            return VK_BLEND_FACTOR_SRC_ALPHA;
        case BlendFactor::DST_ALPHA:
            return VK_BLEND_FACTOR_DST_ALPHA;
        case BlendFactor::ONE_MINUS_SRC_COLOR:
            return VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR;
        case BlendFactor::ONE_MINUS_SRC_ALPHA:
            return VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        default:
            FASTCG_THROW_EXCEPTION(Exception, "Vulkan: Couldn't get a Vk blend factor (blendFactor: %s)",
                                   GetBlendFactorString(blendFactor));
            return (VkBlendFactor)0;
        }
    }

    inline VkFilter GetVkFilter(TextureFilter filter)
    {
        switch (filter)
        {
        case TextureFilter::POINT_FILTER:
            return VK_FILTER_NEAREST;
        case TextureFilter::LINEAR_FILTER:
            return VK_FILTER_LINEAR;
        default:
            FASTCG_THROW_EXCEPTION(Exception, "Vulkan: Couldn't get a Vk filter (filter: %s)",
                                   GetTextureFilterString(filter));
            return (VkFilter)0;
        }
    }

    inline VkSamplerAddressMode GetVkAddressMode(TextureWrapMode wrapMode)
    {
        switch (wrapMode)
        {
        case TextureWrapMode::CLAMP:
            return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        case TextureWrapMode::REPEAT:
            return VK_SAMPLER_ADDRESS_MODE_REPEAT;
        default:
            FASTCG_THROW_EXCEPTION(Exception, "Vulkan: Couldn't get a Vk address mode (wrapMode: %s)",
                                   GetTextureWrapModeString(wrapMode));
            return (VkSamplerAddressMode)0;
        }
    }

#ifdef CONVERSION_TABLE_ENTRY
#undef CONVERSION_TABLE_ENTRY
#endif
#define CONVERSION_TABLE_ENTRY(format)                                                                                 \
    {                                                                                                                  \
        TextureFormat::format, VK_FORMAT_##format                                                                      \
    }

    constexpr struct
    {
        TextureFormat format;
        VkFormat vkFormat;
    } FORMAT_CONVERSION_TABLE[] = {CONVERSION_TABLE_ENTRY(R32_SFLOAT),
                                   CONVERSION_TABLE_ENTRY(R16_SFLOAT),
                                   CONVERSION_TABLE_ENTRY(R16_UNORM),
                                   CONVERSION_TABLE_ENTRY(R8_UNORM),
                                   CONVERSION_TABLE_ENTRY(R32G32_SFLOAT),
                                   CONVERSION_TABLE_ENTRY(R16G16_SFLOAT),
                                   CONVERSION_TABLE_ENTRY(R16G16_UNORM),
                                   CONVERSION_TABLE_ENTRY(R8G8_UNORM),
                                   CONVERSION_TABLE_ENTRY(R32G32B32_SFLOAT),
                                   CONVERSION_TABLE_ENTRY(R16G16B16_UNORM),
                                   CONVERSION_TABLE_ENTRY(R16G16B16_SFLOAT),
                                   CONVERSION_TABLE_ENTRY(R8G8B8_UNORM),
                                   CONVERSION_TABLE_ENTRY(B8G8R8_UNORM),
                                   CONVERSION_TABLE_ENTRY(B10G11R11_UFLOAT_PACK32),
                                   CONVERSION_TABLE_ENTRY(R32G32B32A32_SFLOAT),
                                   CONVERSION_TABLE_ENTRY(R16G16B16A16_UNORM),
                                   CONVERSION_TABLE_ENTRY(A2R10G10B10_UNORM_PACK32),
                                   CONVERSION_TABLE_ENTRY(R8G8B8A8_UNORM),
                                   CONVERSION_TABLE_ENTRY(B8G8R8A8_UNORM),
                                   CONVERSION_TABLE_ENTRY(D24_UNORM_S8_UINT),
                                   CONVERSION_TABLE_ENTRY(D32_SFLOAT),
                                   CONVERSION_TABLE_ENTRY(X8_D24_UNORM_PACK32),
                                   CONVERSION_TABLE_ENTRY(D16_UNORM),
                                   CONVERSION_TABLE_ENTRY(BC1_RGB_UNORM_BLOCK),
                                   CONVERSION_TABLE_ENTRY(BC1_RGBA_UNORM_BLOCK),
                                   CONVERSION_TABLE_ENTRY(BC2_UNORM_BLOCK),
                                   CONVERSION_TABLE_ENTRY(BC3_UNORM_BLOCK),
                                   CONVERSION_TABLE_ENTRY(BC4_UNORM_BLOCK),
                                   CONVERSION_TABLE_ENTRY(BC4_SNORM_BLOCK),
                                   CONVERSION_TABLE_ENTRY(BC5_UNORM_BLOCK),
                                   CONVERSION_TABLE_ENTRY(BC5_SNORM_BLOCK),
                                   CONVERSION_TABLE_ENTRY(BC6H_UFLOAT_BLOCK),
                                   CONVERSION_TABLE_ENTRY(BC6H_SFLOAT_BLOCK),
                                   CONVERSION_TABLE_ENTRY(BC7_UNORM_BLOCK),
                                   CONVERSION_TABLE_ENTRY(ASTC_4x4_UNORM_BLOCK),
                                   CONVERSION_TABLE_ENTRY(ASTC_5x4_UNORM_BLOCK),
                                   CONVERSION_TABLE_ENTRY(ASTC_5x5_UNORM_BLOCK),
                                   CONVERSION_TABLE_ENTRY(ASTC_6x5_UNORM_BLOCK),
                                   CONVERSION_TABLE_ENTRY(ASTC_6x6_UNORM_BLOCK),
                                   CONVERSION_TABLE_ENTRY(ASTC_8x5_UNORM_BLOCK),
                                   CONVERSION_TABLE_ENTRY(ASTC_8x6_UNORM_BLOCK),
                                   CONVERSION_TABLE_ENTRY(ASTC_8x8_UNORM_BLOCK),
                                   CONVERSION_TABLE_ENTRY(ASTC_10x5_UNORM_BLOCK),
                                   CONVERSION_TABLE_ENTRY(ASTC_10x6_UNORM_BLOCK),
                                   CONVERSION_TABLE_ENTRY(ASTC_10x8_UNORM_BLOCK),
                                   CONVERSION_TABLE_ENTRY(ASTC_10x10_UNORM_BLOCK),
                                   CONVERSION_TABLE_ENTRY(ASTC_12x10_UNORM_BLOCK),
                                   CONVERSION_TABLE_ENTRY(ASTC_12x12_UNORM_BLOCK)};

#undef CONVERSION_TABLE_ENTRY

    inline VkFormat GetVkFormat(TextureFormat format)
    {
        const auto *pTableStart = &FORMAT_CONVERSION_TABLE[0];
        const auto *pTableEnd = FORMAT_CONVERSION_TABLE + FASTCG_ARRAYSIZE(FORMAT_CONVERSION_TABLE);
        auto it =
            std::find_if(pTableStart, pTableEnd, [&](const auto &rTableEntry) { return rTableEntry.format == format; });
        if (it == pTableEnd)
        {
            FASTCG_THROW_EXCEPTION(Exception, "Vulkan: Couldn't get a Vk format (format: %s)",
                                   GetTextureFormatString(format));
            return (VkFormat)0;
        }
        return it->vkFormat;
    }

    inline TextureFormat GetTextureFormat(VkFormat vkFormat)
    {
        const auto *pTableStart = &FORMAT_CONVERSION_TABLE[0];
        const auto *pTableEnd = FORMAT_CONVERSION_TABLE + FASTCG_ARRAYSIZE(FORMAT_CONVERSION_TABLE);
        auto it = std::find_if(pTableStart, pTableEnd,
                               [&](const auto &rTableEntry) { return rTableEntry.vkFormat == vkFormat; });
        if (it == pTableEnd)
        {
            FASTCG_THROW_EXCEPTION(Exception, "Vulkan: Couldn't get a texture format (vkFormat: %s)",
                                   GetVkFormatString(vkFormat));
            return (TextureFormat)0;
        }
        return it->format;
    }

    inline VkFormat GetVkFormat(VertexDataType dataType, uint32_t components)
    {
        switch (dataType)
        {
        case VertexDataType::FLOAT:
            if (components == 1)
            {
                return VK_FORMAT_R32_SFLOAT;
            }
            else if (components == 2)
            {
                return VK_FORMAT_R32G32_SFLOAT;
            }
            else if (components == 3)
            {
                return VK_FORMAT_R32G32B32_SFLOAT;
            }
            else if (components == 4)
            {
                return VK_FORMAT_R32G32B32A32_SFLOAT;
            }
        case VertexDataType::UNSIGNED_BYTE:
            if (components == 1)
            {
                return VK_FORMAT_R8_UNORM;
            }
            else if (components == 2)
            {
                return VK_FORMAT_R8G8_UNORM;
            }
            else if (components == 3)
            {
                return VK_FORMAT_R8G8B8_UNORM;
            }
            else if (components == 4)
            {
                return VK_FORMAT_R8G8B8A8_UNORM;
            }
        default:
            FASTCG_THROW_EXCEPTION(Exception, "Vulkan: Couldn't get a Vk format (dataType: %s, components: %d)",
                                   GetVertexDataTypeString(dataType), components);
            return (VkFormat)0;
        }
    }

    inline uint32_t GetVkStride(VkFormat vkFormat)
    {
        switch (vkFormat)
        {
        case VK_FORMAT_R32_SFLOAT:
            return 4;
        case VK_FORMAT_R32G32_SFLOAT:
            return 8;
        case VK_FORMAT_R32G32B32_SFLOAT:
            return 12;
        case VK_FORMAT_R32G32B32A32_SFLOAT:
            return 16;
        case VK_FORMAT_R8G8B8A8_UNORM:
            return 4;
        default:
            FASTCG_THROW_EXCEPTION(Exception, "Vulkan: Couldn't get a Vk stride (vkFormat: %s)",
                                   GetVkFormatString(vkFormat));
            return 0;
        }
    }

    inline bool IsVkReadOnlyAccessFlags(VkAccessFlags accessFlags)
    {
        constexpr VkAccessFlags writeFlags = VK_ACCESS_TRANSFER_WRITE_BIT | VK_ACCESS_HOST_WRITE_BIT |
                                             VK_ACCESS_SHADER_WRITE_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT |
                                             VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT | VK_ACCESS_MEMORY_WRITE_BIT
            // | VK_ACCESS_TRANSFORM_FEEDBACK_WRITE_BIT_EXT
            // | VK_ACCESS_ACCELERATION_STRUCTURE_WRITE_BIT_KHR
            // | VK_ACCESS_COMMAND_PREPROCESS_WRITE_BIT_NV
            ;
        return (accessFlags & writeFlags) == 0;
    }
}

#endif

#endif