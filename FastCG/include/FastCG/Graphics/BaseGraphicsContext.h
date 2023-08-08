#ifndef FASTCG_BASE_GRAPHICS_CONTEXT_H
#define FASTCG_BASE_GRAPHICS_CONTEXT_H

#include <FastCG/Graphics/GraphicsUtils.h>

#include <glm/glm.hpp>

#include <string>
#include <cstdint>

namespace FastCG
{
    template <class BufferT, class ShaderT, class TextureT>
    class BaseGraphicsContext
    {
    public:
        struct Args
        {
            std::string name;

            Args(const std::string &rName = "") : name(rName)
            {
            }
        };

        using Buffer = BufferT;
        using Shader = ShaderT;
        using Texture = TextureT;

        inline const std::string &GetName() const
        {
            return mArgs.name;
        }

        // Template interface
        void Begin();
        void PushDebugMarker(const char *pName);
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
        void Copy(const Buffer *pBuffer, size_t dataSize, const void *pData);
        void Copy(const Texture *pTexture, size_t dataSize, const void *pData);
        void BindShader(const Shader *pShader);
        void BindResource(const Buffer *pBuffer, const char *pName);
        void BindResource(const Texture *pTexture, const char *pName);
        void Blit(const Texture *pSrc, const Texture *pDst);
        void SetRenderTargets(const Texture *const *pRenderTargets, uint32_t renderTargetCount, const Texture *pDepthStencilBuffer);
        void ClearRenderTarget(uint32_t renderTargetIndex, const glm::vec4 &rClearColor);
        void ClearDepthStencilBuffer(float depth, int32_t stencil);
        void ClearDepthBuffer(float depth);
        void ClearStencilBuffer(int32_t stencil);
        void SetVertexBuffers(const Buffer *const *pBuffers, uint32_t bufferCount);
        void SetIndexBuffer(const Buffer *pBuffer);
        void SetPrimitiveType(PrimitiveType primitiveType);
        void DrawIndexed(PrimitiveType primitiveType, uint32_t firstIndex, uint32_t indexCount, int32_t vertexOffset);
        void DrawInstancedIndexed(PrimitiveType primitiveType, uint32_t firstInstance, uint32_t instanceCount, uint32_t firstIndex, uint32_t indexCount, int32_t vertexOffset);
        void End();
        double GetElapsedTime() const;

    protected:
        const Args mArgs;

        BaseGraphicsContext(const Args &rArgs) : mArgs(rArgs) {}
        virtual ~BaseGraphicsContext() = default;
    };

}

#endif