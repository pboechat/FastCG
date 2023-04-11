#include <FastCG/Rendering/Mesh.h>

#include <cassert>
#include <algorithm>

namespace FastCG
{
    Mesh::Mesh(const MeshArgs &rArgs) : mName(rArgs.name),
                                        mIndexCount(rArgs.indices.count),
                                        mBounds(rArgs.bounds)
    {
        assert(std::none_of(mVertexBuffers.begin(), mVertexBuffers.end(), [](const auto *pBuffer)
                            { return pBuffer->GetVertexBindingDescriptors().empty(); }));

        for (const auto &vbDesc : rArgs.vertexAttributeDecriptors)
        {
            mVertexBuffers.emplace_back(GraphicsSystem::GetInstance()->CreateBuffer({mName + " " + vbDesc.name,
                                                                                     BufferType::VERTEX_ATTRIBUTE,
                                                                                     vbDesc.usage,
                                                                                     vbDesc.dataSize,
                                                                                     vbDesc.pData,
                                                                                     vbDesc.bindingDescriptors}));
        }

        mpIndexBuffer = GraphicsSystem::GetInstance()->CreateBuffer({mName + " Indices",
                                                                     BufferType::INDICES,
                                                                     rArgs.indices.usage,
                                                                     rArgs.indices.count * sizeof(uint32_t),
                                                                     rArgs.indices.pData});
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