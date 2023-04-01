#ifndef FASTCG_VULKAN_GRAPHICS_CONTEXT_H
#define FASTCG_VULKAN_GRAPHICS_CONTEXT_H

#ifdef FASTCG_VULKAN

#include <FastCG/Graphics/Vulkan/VulkanTexture.h>
#include <FastCG/Graphics/Vulkan/VulkanShader.h>
#include <FastCG/Graphics/Vulkan/VulkanBuffer.h>
#include <FastCG/Graphics/BaseGraphicsContext.h>

namespace FastCG
{
    class VulkanGraphicsSystem;

    class VulkanGraphicsContext : public BaseGraphicsContext<VulkanBuffer, VulkanShader, VulkanTexture>
    {
    public:
        void Begin();
        void PushDebugMarker(const char *name);
        void PopDebugMarker();
        void SetViewport(int32_t x, int32_t y, uint32_t width, uint32_t height);
        void SetScissor(int32_t x, int32_t y, uint32_t width, uint32_t height);
        void SetBlend(bool blend);
        void SetBlendFunc(BlendFunc color, BlendFunc alpha);
        void SetBlendFactors(BlendFactor srcColor, BlendFactor dstColor, BlendFactor srcAlpha, BlendFactor dstAlpha);
        void SetStencilTest(bool stencilTest);
        void SetStencilFunc(Face face, CompareOp stencilFunc, int32_t ref, uint32_t mask);
        void SetStencilOp(Face face, StencilOp stencilFail, StencilOp depthFail, StencilOp depthPass);
        void SetStencilWriteMask(Face face, uint32_t mask);
        void SetDepthTest(bool depthTest);
        void SetDepthWrite(bool depthWrite);
        void SetDepthFunc(CompareOp depthFunc);
        void SetScissorTest(bool scissorTest);
        void SetCullMode(Face face);
        void Copy(const VulkanBuffer *pBuffer, size_t dataSize, const void *pData);
        void Copy(const VulkanTexture *pTexture, size_t dataSize, const void *pData);
        void BindShader(const VulkanShader *pShader);
        void BindResource(const VulkanBuffer *pBuffer, uint32_t index);
        void BindResource(const VulkanBuffer *pBuffer, const char *name);
        void BindResource(const VulkanTexture *pTexture, uint32_t index, uint32_t unit);
        void BindResource(const VulkanTexture *pTexture, const char *name, uint32_t unit);
        void Blit(const VulkanTexture *pSrc, const VulkanTexture *pDst);
        void SetRenderTargets(const VulkanTexture *const *pTextures, size_t textureCount);
        void ClearRenderTarget(uint32_t renderTargetIndex, const glm::vec4 &rClearColor);
        void ClearDepthStencilTarget(uint32_t renderTargetIndex, float depth, int32_t stencil);
        void ClearDepthTarget(uint32_t renderTargetIndex, float depth);
        void ClearStencilTarget(uint32_t renderTargetIndex, int32_t stencil);
        void SetVertexBuffers(const VulkanBuffer *const *pBuffers, size_t bufferCount);
        void SetIndexBuffer(const VulkanBuffer *pBuffer);
        void DrawIndexed(PrimitiveType primitiveType, uint32_t firstIndex, uint32_t indexCount, int32_t vertexOffset);
        void DrawInstancedIndexed(PrimitiveType primitiveType, uint32_t firstInstance, uint32_t instanceCount, uint32_t firstIndex, uint32_t indexCount, int32_t vertexOffset);
        void End();
        double GetElapsedTime() const;

    private:
        VulkanGraphicsContext(const GraphicsContextArgs &rArgs);
        virtual ~VulkanGraphicsContext();

        friend class VulkanGraphicsSystem;
    };

}

#endif

#endif