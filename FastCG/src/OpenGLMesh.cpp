#ifdef FASTCG_OPENGL

#include <FastCG/OpenGLRenderingSystem.h>
#include <FastCG/OpenGLMesh.h>

namespace FastCG
{
    OpenGLMesh::OpenGLMesh(const MeshArgs &rArgs) : BaseMesh(rArgs)
    {

        for (const auto &vbDesc : rArgs.vertexAttributeDecriptors)
        {
            mVertexBuffers.emplace_back(OpenGLRenderingSystem::GetInstance()->CreateBuffer({mName + " " + vbDesc.name,
                                                                                            BufferType::VERTEX_ATTRIBUTE,
                                                                                            vbDesc.usage,
                                                                                            vbDesc.dataSize,
                                                                                            vbDesc.pData,
                                                                                            vbDesc.bindingDescriptors}));
        }

        mpIndexBuffer = OpenGLRenderingSystem::GetInstance()->CreateBuffer({mName + " Indices",
                                                                            BufferType::INDICES,
                                                                            rArgs.indices.usage,
                                                                            rArgs.indices.count * sizeof(uint32_t),
                                                                            rArgs.indices.pData});
    }

    OpenGLMesh::~OpenGLMesh()
    {
        if (mpIndexBuffer != nullptr)
        {
            OpenGLRenderingSystem::GetInstance()->DestroyBuffer(mpIndexBuffer);
        }
        for (const auto *pVertexBuffer : mVertexBuffers)
        {
            OpenGLRenderingSystem::GetInstance()->DestroyBuffer(pVertexBuffer);
        }
    }
}

#endif