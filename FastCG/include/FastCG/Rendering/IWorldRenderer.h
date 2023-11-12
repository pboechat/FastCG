#ifndef FASTCG_I_WORLD_RENDERER_H
#define FASTCG_I_WORLD_RENDERER_H

#include <FastCG/Rendering/Camera.h>
#include <FastCG/Graphics/GraphicsSystem.h>

namespace FastCG
{
    class IWorldRenderer
    {
    public:
        virtual ~IWorldRenderer() = default;

        virtual float GetShadowMapBias() const = 0;
        virtual void SetShadowMapBias(float shadowMapBias) = 0;
        virtual float GetPCSSUvScale() const = 0;
        virtual void SetPCSSUvScale(float uvScale) = 0;
        virtual uint32_t GetPCSSBlockerSearchSamples() const = 0;
        virtual void SetPCSSBlockerSearchSamples(uint32_t pcssBlockerSearchSamples) = 0;
        virtual uint32_t GetPCSSPCFSamples() const = 0;
        virtual void SetPCSSPCFSamples(uint32_t pcssPCFSamples) = 0;
        virtual float GetSSAORadius() const = 0;
        virtual void SetSSAORadius(float radius) = 0;
        virtual bool IsSSAOBlurEnabled() const = 0;
        virtual void SetSSAOBlurEnabled(bool ssaoBlurEnabled) = 0;
        virtual void Initialize() = 0;
        virtual void Resize() = 0;
        virtual void Finalize() = 0;
        virtual void Render(const Camera *pCamera, GraphicsContext *pGraphicsContext) = 0;
    };

}

#endif