#ifndef FASTCG_VULKAN_SHADER_H
#define FASTCG_VULKAN_SHADER_H

#ifdef FASTCG_VULKAN

#include <FastCG/Graphics/BaseShader.h>

#include <string>

namespace FastCG
{
    class VulkanGraphicsSystem;

    class VulkanShader : public BaseShader
    {
    private:
        VulkanShader(const ShaderArgs &rName);
        VulkanShader(const VulkanShader &rOther) = delete;
        VulkanShader(const VulkanShader &&rOther) = delete;
        virtual ~VulkanShader();

        VulkanShader operator=(const VulkanShader &rOther) = delete;

        friend class VulkanGraphicsSystem;
    };

}

#endif

#endif