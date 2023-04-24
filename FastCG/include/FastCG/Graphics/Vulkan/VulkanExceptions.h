#ifndef FASTCG_VULKAN_EXCEPTIONS_H
#define FASTCG_VULKAN_EXCEPTIONS_H

#ifdef FASTCG_VULKAN

#include <FastCG/Graphics/Vulkan/Vulkan.h>
#include <FastCG/Graphics/Vulkan/VulkanUtils.h>
#include <FastCG/Core/Exception.h>

#include <sstream>

namespace FastCG
{
    class VulkanException : public Exception
    {
    public:
        VulkanException(const std::string &rReason) : Exception(rReason) {}

    protected:
        VulkanException() = default;
    };

}

#define FASTCG_CHECK_VK_RESULT(vkCall)                                                     \
    {                                                                                      \
        VkResult __vkResult;                                                               \
        if ((__vkResult = vkCall) != VK_SUCCESS)                                           \
        {                                                                                  \
            std::stringstream __stringStream;                                              \
            __stringStream << #vkCall " = " << FastCG::GetVkResultString(__vkResult);      \
            FASTCG_THROW_EXCEPTION(FastCG::VulkanException, __stringStream.str().c_str()); \
        }                                                                                  \
    }

#endif

#endif
