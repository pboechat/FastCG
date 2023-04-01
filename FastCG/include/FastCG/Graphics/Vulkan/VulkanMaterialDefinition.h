#ifndef FASTCG_VULKAN_MATERIAL_DEFINITION_H
#define FASTCG_VULKAN_MATERIAL_DEFINITION_H

#ifdef FASTCG_VULKAN

#include <FastCG/Graphics/Vulkan/VulkanTexture.h>
#include <FastCG/Graphics/Vulkan/VulkanShader.h>
#include <FastCG/Graphics/BaseMaterialDefinition.h>

namespace FastCG
{
    class VulkanGraphicsSystem;

    class VulkanMaterialDefinition : public BaseMaterialDefinition<VulkanShader, VulkanTexture>
    {
    private:
        VulkanMaterialDefinition(const MaterialDefinitionArgs &rArgs) : BaseMaterialDefinition(rArgs)
        {
        }
        virtual ~VulkanMaterialDefinition() = default;

        friend class VulkanGraphicsSystem;
    };

}

#endif

#endif