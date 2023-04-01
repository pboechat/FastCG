#ifdef FASTCG_VULKAN

#include <FastCG/Graphics/Vulkan/VulkanMesh.h>
#include <FastCG/Graphics/Vulkan/VulkanGraphicsSystem.h>

namespace FastCG
{
    VulkanMesh::VulkanMesh(const MeshArgs &rArgs) : BaseMesh(rArgs)
    {

        for (const auto &vbDesc : rArgs.vertexAttributeDecriptors)
        {
            mVertexBuffers.emplace_back(VulkanGraphicsSystem::GetInstance()->CreateBuffer({mName + " " + vbDesc.name,
                                                                                           BufferType::VERTEX_ATTRIBUTE,
                                                                                           vbDesc.usage,
                                                                                           vbDesc.dataSize,
                                                                                           vbDesc.pData,
                                                                                           vbDesc.bindingDescriptors}));
        }

        mpIndexBuffer = VulkanGraphicsSystem::GetInstance()->CreateBuffer({mName + " Indices",
                                                                           BufferType::INDICES,
                                                                           rArgs.indices.usage,
                                                                           rArgs.indices.count * sizeof(uint32_t),
                                                                           rArgs.indices.pData});
    }

    VulkanMesh::~VulkanMesh()
    {
        if (mpIndexBuffer != nullptr)
        {
            VulkanGraphicsSystem::GetInstance()->DestroyBuffer(mpIndexBuffer);
        }
        for (const auto *pVertexBuffer : mVertexBuffers)
        {
            VulkanGraphicsSystem::GetInstance()->DestroyBuffer(pVertexBuffer);
        }
    }
}

#endif