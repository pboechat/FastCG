#ifdef FASTCG_VULKAN

#include <FastCG/Graphics/Vulkan/VulkanGraphicsContext.h>
#include <FastCG/Graphics/Vulkan/VulkanGraphicsSystem.h>

namespace FastCG
{
    VulkanGraphicsContext::VulkanGraphicsContext(const GraphicsContextArgs &rArgs)
        : BaseGraphicsContext<VulkanBuffer, VulkanShader, VulkanTexture>(rArgs)
    {
    }

    VulkanGraphicsContext::~VulkanGraphicsContext()
    {
    }

    void VulkanGraphicsContext::Begin()
    {
    }

    void VulkanGraphicsContext::PushDebugMarker(const char *name)
    {
    }

    void VulkanGraphicsContext::PopDebugMarker()
    {
    }

    void VulkanGraphicsContext::SetViewport(int32_t x, int32_t y, uint32_t width, uint32_t height)
    {
    }

    void VulkanGraphicsContext::SetScissor(int32_t x, int32_t y, uint32_t width, uint32_t height)
    {
    }

    void VulkanGraphicsContext::SetBlend(bool blend)
    {
    }

    void VulkanGraphicsContext::SetBlendFunc(BlendFunc color, BlendFunc alpha)
    {
    }

    void VulkanGraphicsContext::SetBlendFactors(BlendFactor srcColor, BlendFactor dstColor, BlendFactor srcAlpha, BlendFactor dstAlpha)
    {
    }

    void VulkanGraphicsContext::SetStencilTest(bool stencilTest)
    {
    }

    void VulkanGraphicsContext::SetStencilFunc(Face face, CompareOp stencilFunc, int32_t ref, uint32_t mask)
    {
    }

    void VulkanGraphicsContext::SetStencilOp(Face face, StencilOp stencilFail, StencilOp depthFail, StencilOp depthPass)
    {
    }

    void VulkanGraphicsContext::SetStencilWriteMask(Face face, uint32_t mask)
    {
    }

    void VulkanGraphicsContext::SetDepthTest(bool depthTest)
    {
    }

    void VulkanGraphicsContext::SetDepthWrite(bool depthWrite)
    {
    }

    void VulkanGraphicsContext::SetDepthFunc(CompareOp depthFunc)
    {
    }

    void VulkanGraphicsContext::SetScissorTest(bool scissorTest)
    {
    }

    void VulkanGraphicsContext::SetCullMode(Face face)
    {
    }

    void VulkanGraphicsContext::Copy(const VulkanBuffer *pBuffer, size_t dataSize, const void *pData)
    {
    }

    void VulkanGraphicsContext::Copy(const VulkanTexture *pTexture, size_t dataSize, const void *pData)
    {
    }

    void VulkanGraphicsContext::BindShader(const VulkanShader *pShader)
    {
    }

    void VulkanGraphicsContext::BindResource(const VulkanBuffer *pBuffer, uint32_t index)
    {
    }

    void VulkanGraphicsContext::BindResource(const VulkanBuffer *pBuffer, const char *name)
    {
    }

    void VulkanGraphicsContext::BindResource(const VulkanTexture *pTexture, uint32_t index, uint32_t unit)
    {
    }

    void VulkanGraphicsContext::BindResource(const VulkanTexture *pTexture, const char *name, uint32_t unit)
    {
    }

    void VulkanGraphicsContext::Blit(const VulkanTexture *pSrc, const VulkanTexture *pDst)
    {
    }

    void VulkanGraphicsContext::ClearRenderTarget(uint32_t renderTargetIndex, const glm::vec4 &rClearColor)
    {
    }

    void VulkanGraphicsContext::ClearDepthStencilTarget(uint32_t renderTargetIndex, float depth, int32_t stencil)
    {
    }

    void VulkanGraphicsContext::ClearDepthTarget(uint32_t renderTargetIndex, float depth)
    {
    }

    void VulkanGraphicsContext::ClearStencilTarget(uint32_t renderTargetIndex, int32_t stencil)
    {
    }

    void VulkanGraphicsContext::SetRenderTargets(const VulkanTexture *const *pTextures, size_t textureCount)
    {
    }

    void VulkanGraphicsContext::SetVertexBuffers(const VulkanBuffer *const *pBuffers, size_t bufferCount)
    {
    }

    void VulkanGraphicsContext::SetIndexBuffer(const VulkanBuffer *pBuffer)
    {
    }

    void VulkanGraphicsContext::DrawIndexed(PrimitiveType primitiveType, uint32_t firstIndex, uint32_t indexCount, int32_t vertexOffset)
    {
    }

    void VulkanGraphicsContext::DrawInstancedIndexed(PrimitiveType primitiveType, uint32_t firstInstance, uint32_t instanceCount, uint32_t firstIndex, uint32_t indexCount, int32_t vertexOffset)
    {
    }

    void VulkanGraphicsContext::End()
    {
    }

    double VulkanGraphicsContext::GetElapsedTime() const
    {
        return 0;
    }

}

#endif