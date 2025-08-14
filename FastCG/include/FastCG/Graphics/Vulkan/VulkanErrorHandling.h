#ifndef FASTCG_VULKAN_ERROR_HANDLING_H
#define FASTCG_VULKAN_ERROR_HANDLING_H

#ifdef FASTCG_VULKAN

#include <FastCG/Core/Exception.h>
#include <FastCG/Graphics/Vulkan/Vulkan.h>
#include <FastCG/Graphics/Vulkan/VulkanUtils.h>

#define FASTCG_CHECK_VK_RESULT(vkCall)                                                                                 \
    {                                                                                                                  \
        VkResult __vkResult;                                                                                           \
        if ((__vkResult = vkCall) != VK_SUCCESS)                                                                       \
        {                                                                                                              \
            FASTCG_THROW_EXCEPTION(FastCG::Exception, "Vulkan: %s (error: %s)", #vkCall,                               \
                                   FastCG::GetVkResultString(__vkResult));                                             \
        }                                                                                                              \
    }

#endif

#endif
