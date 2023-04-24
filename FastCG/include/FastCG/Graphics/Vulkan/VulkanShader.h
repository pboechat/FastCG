#ifndef FASTCG_VULKAN_SHADER_H
#define FASTCG_VULKAN_SHADER_H

#ifdef FASTCG_VULKAN

#include <FastCG/Graphics/BaseShader.h>

namespace FastCG
{
    class VulkanGraphicsSystem;

    class VulkanShader : public BaseShader
    {
    public:
        VulkanShader(const ShaderArgs &rName);
        VulkanShader(const VulkanShader &rOther) = delete;
        VulkanShader(const VulkanShader &&rOther) = delete;
        virtual ~VulkanShader();

        VulkanShader operator=(const VulkanShader &rOther) = delete;

    private:
        friend class VulkanGraphicsSystem;
    };

}

#endif

#endif