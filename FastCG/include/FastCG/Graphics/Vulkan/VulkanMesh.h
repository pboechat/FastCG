#ifndef FASTCG_VULKAN_MESH_H
#define FASTCG_VULKAN_MESH_H

#ifdef FASTCG_VULKAN

#include <FastCG/Graphics/Vulkan/VulkanBuffer.h>
#include <FastCG/Graphics/BaseMesh.h>

namespace FastCG
{
    class VulkanGraphicsSystem;

    class VulkanMesh : public BaseMesh<VulkanBuffer>
    {
    private:
        VulkanMesh(const MeshArgs &rArgs);
        VulkanMesh(const VulkanMesh &rOther) = delete;
        VulkanMesh(const VulkanMesh &&rOther) = delete;
        virtual ~VulkanMesh();

        VulkanMesh operator=(const VulkanMesh &rOther) = delete;

        friend class VulkanGraphicsSystem;
    };

}

#endif

#endif