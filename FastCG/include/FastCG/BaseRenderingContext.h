#ifndef FASTCG_BASE_RENDERING_CONTEXT_H
#define FASTCG_BASE_RENDERING_CONTEXT_H

#include <glm/glm.hpp>

#include <cstdint>

namespace FastCG
{
    enum class PrimitiveType : uint8_t
    {
        TRIANGLES = 1,

    };

    enum class BlendFunc : uint8_t
    {
        NONE = 0,
        ADD,

    };

    enum class BlendFactor : uint8_t
    {
        ZERO = 0,
        ONE,
        SRC_COLOR,
        DST_COLOR,
        SRC_ALPHA,
        DST_ALPHA,
        ONE_MINUS_SRC_ALPHA,

    };

    enum class Face : uint8_t
    {
        NONE = 0,
        FRONT,
        BACK,
        FRONT_AND_BACK

    };

    enum class CompareOp : uint8_t
    {
        NEVER = 0,
        LESS,
        LEQUAL,
        GREATER,
        GEQUAL,
        EQUAL,
        NOT_EQUAL,
        ALWAYS,

    };

    enum class StencilOp : uint8_t
    {
        KEEP = 0,
        ZERO,
        REPLACE,
        INVERT,
        INCREMENT_AND_CLAMP,
        INCREMENT_AND_WRAP,
        DECREMENT_AND_CLAMP,
        DECREMENT_AND_WRAP,

    };

    template <class BufferT, class ShaderT, class TextureT>
    class BaseRenderingContext
    {
    public:
        using Buffer = BufferT;
        using Shader = ShaderT;
        using Texture = TextureT;

        // Template interface
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
        void SetStencilWriteMask(uint32_t mask);
        void SetDepthTest(bool depthTest);
        void SetDepthWrite(bool depthWrite);
        void SetDepthFunc(CompareOp depthFunc);
        void SetScissorTest(bool scissorTest);
        void SetCullMode(Face face);
        void Copy(const Buffer *pBuffer, size_t dataSize, const void *pData);
        void Copy(const Texture *pTexture, size_t dataSize, const void *pData);
        void Bind(const Shader *pShader);
        void Bind(const Buffer *pBuffer, uint32_t binding);
        void Bind(const Buffer *pBuffer, const char *name);
        void Bind(const Texture *pTexture, uint32_t binding, uint32_t unit);
        void Bind(const Texture *pTexture, const char *name, uint32_t unit);
        void Blit(const Texture *pSrc, const Texture *pDst);
        void SetRenderTargets(const Texture *const *pTextures, size_t textureCount);
        void ClearRenderTarget(uint32_t renderTargetIndex, const glm::vec4 &rClearColor);
        void ClearDepthStencilTarget(uint32_t renderTargetIndex, float depth, int32_t stencil);
        void ClearDepthTarget(uint32_t renderTargetIndex, float depth);
        void ClearStencilTarget(uint32_t renderTargetIndex, int32_t stencil);
        void SetVertexBuffers(const Buffer *const *pBuffers, size_t bufferCount);
        void SetIndexBuffer(const Buffer *pBuffer);
        void SetPrimitiveType(PrimitiveType primitiveType);
        void DrawIndexed(PrimitiveType primitiveType, uint32_t indexCount, uint32_t firstIndex, int32_t vertexOffset);
        void End();
    };

}

#endif