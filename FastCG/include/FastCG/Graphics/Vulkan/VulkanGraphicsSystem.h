#ifndef FASTCG_VULKAN_GRAPHICS_SYSTEM_H
#define FASTCG_VULKAN_GRAPHICS_SYSTEM_H

#ifdef FASTCG_VULKAN

#include <FastCG/System.h>
#include <FastCG/ShaderConstants.h>
#include <FastCG/Graphics/Vulkan/VulkanTexture.h>
#include <FastCG/Graphics/Vulkan/VulkanShader.h>
#include <FastCG/Graphics/Vulkan/VulkanGraphicsContext.h>
#include <FastCG/Graphics/Vulkan/VulkanBuffer.h>
#include <FastCG/Graphics/BaseGraphicsSystem.h>

#include <vector>
#include <unordered_map>
#include <memory>
#include <cstring>
#include <algorithm>

namespace FastCG
{
    class VulkanGraphicsSystem : public BaseGraphicsSystem<VulkanBuffer, VulkanGraphicsContext, VulkanShader, VulkanTexture>
    {
        FASTCG_DECLARE_SYSTEM(VulkanGraphicsSystem, GraphicsSystemArgs);

    protected:
        VulkanGraphicsSystem(const GraphicsSystemArgs &rArgs);
        virtual ~VulkanGraphicsSystem();

        void OnInitialize() override;

    private:
        void Present();
        double GetPresentElapsedTime() const;
        double GetGpuElapsedTime() const;
    };

}

#endif

#endif