#ifndef FASTCG_VULKAN_EXCEPTIONS_H
#define FASTCG_VULKAN_EXCEPTIONS_H

#ifdef FASTCG_VULKAN

#include <FastCG/Core/Exception.h>
#include <FastCG/Graphics/Vulkan/Vulkan.h>
#include <FastCG/Graphics/Vulkan/VulkanUtils.h>

namespace FastCG
{
    class VulkanException : public Exception
    {
    public:
        VulkanException(const std::string &rReason) : Exception(rReason)
        {
        }

    protected:
        VulkanException() = default;
    };

}

#define FASTCG_CHECK_VK_RESULT(vkCall)                                                                                 \
    {                                                                                                                  \
        VkResult __vkResult;                                                                                           \
        if ((__vkResult = vkCall) != VK_SUCCESS)                                                                       \
        {                                                                                                              \
            FASTCG_THROW_EXCEPTION(FastCG::VulkanException, "Vulkan: %s (error: %s)", #vkCall,                         \
                                   FastCG::GetVkResultString(__vkResult));                                             \
        }                                                                                                              \
    }

#endif

#endif
