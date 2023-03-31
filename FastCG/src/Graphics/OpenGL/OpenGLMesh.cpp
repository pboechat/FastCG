#ifdef FASTCG_OPENGL

#include <FastCG/Graphics/OpenGL/OpenGLMesh.h>
#include <FastCG/Graphics/OpenGL/OpenGLGraphicsSystem.h>

namespace FastCG
{
    OpenGLMesh::OpenGLMesh(const MeshArgs &rArgs) : BaseMesh(rArgs)
    {

        for (const auto &vbDesc : rArgs.vertexAttributeDecriptors)
        {
            mVertexBuffers.emplace_back(OpenGLGraphicsSystem::GetInstance()->CreateBuffer({mName + " " + vbDesc.name,
                                                                                           BufferType::VERTEX_ATTRIBUTE,
                                                                                           vbDesc.usage,
                                                                                           vbDesc.dataSize,
                                                                                           vbDesc.pData,
                                                                                           vbDesc.bindingDescriptors}));
        }

        mpIndexBuffer = OpenGLGraphicsSystem::GetInstance()->CreateBuffer({mName + " Indices",
                                                                           BufferType::INDICES,
                                                                           rArgs.indices.usage,
                                                                           rArgs.indices.count * sizeof(uint32_t),
                                                                           rArgs.indices.pData});
    }

    OpenGLMesh::~OpenGLMesh()
    {
        if (mpIndexBuffer != nullptr)
        {
            OpenGLGraphicsSystem::GetInstance()->DestroyBuffer(mpIndexBuffer);
        }
        for (const auto *pVertexBuffer : mVertexBuffers)
        {
            OpenGLGraphicsSystem::GetInstance()->DestroyBuffer(pVertexBuffer);
        }
    }
}

#endif