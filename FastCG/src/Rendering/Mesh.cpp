#include <FastCG/Rendering/Mesh.h>

#include <algorithm>
#include <cassert>

namespace FastCG
{
    Mesh::Mesh(const MeshArgs &rArgs) : mName(rArgs.name), mIndexCount(rArgs.indices.count), mBounds(rArgs.bounds)
    {
        assert(std::none_of(mVertexBuffers.begin(), mVertexBuffers.end(),
                            [](const auto *pBuffer) { return pBuffer->GetVertexBindingDescriptors().empty(); }));

        for (const auto &vbDesc : rArgs.vertexAttributeDecriptors)
        {
            mVertexBuffers.emplace_back(GraphicsSystem::GetInstance()->CreateBuffer(
                {vbDesc.name, (BufferUsageFlags)(vbDesc.usage | BufferUsageFlagBit::VERTEX_BUFFER), vbDesc.dataSize,
                 vbDesc.pData, vbDesc.bindingDescriptors}));
        }

        mpIndexBuffer = GraphicsSystem::GetInstance()->CreateBuffer(
            {mName + " Indices", (BufferUsageFlags)(rArgs.indices.usage | BufferUsageFlagBit::INDEX_BUFFER),
             rArgs.indices.count * sizeof(uint32_t), rArgs.indices.pData});
    }

    Mesh::~Mesh()
    {
        if (mpIndexBuffer != nullptr)
        {
            GraphicsSystem::GetInstance()->DestroyBuffer(mpIndexBuffer);
        }
        for (const auto *pVertexBuffer : mVertexBuffers)
        {
            GraphicsSystem::GetInstance()->DestroyBuffer(pVertexBuffer);
        }
    }
}