#ifndef FASTCG_VULKAN_MATERIAL_H
#define FASTCG_VULKAN_MATERIAL_H

#ifdef FASTCG_VULKAN

#include <FastCG/Graphics/Vulkan/VulkanTexture.h>
#include <FastCG/Graphics/Vulkan/VulkanShader.h>
#include <FastCG/Graphics/Vulkan/VulkanBuffer.h>
#include <FastCG/Graphics/BaseMaterial.h>

namespace FastCG
{
    class VulkanGraphicsSystem;

    class VulkanMaterial : public BaseMaterial<VulkanBuffer, VulkanShader, VulkanTexture>
    {
    private:
        VulkanMaterial(const MaterialArgs &rArgs, const VulkanBuffer *pConstantsBuffer) : BaseMaterial(rArgs, pConstantsBuffer) {}

        friend class VulkanGraphicsSystem;
    };

}

#endif

#endif