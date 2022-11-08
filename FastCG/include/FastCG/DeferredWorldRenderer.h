#ifndef FASTCG_DEFERRED_WORLD_RENDERER_H
#define FASTCG_DEFERRED_WORLD_RENDERER_H

#include <FastCG/ShaderConstants.h>
#include <FastCG/BaseWorldRenderer.h>

#include <array>

namespace FastCG
{
    class DeferredWorldRenderer : public BaseWorldRenderer<DeferredRenderingPath::InstanceConstants, DeferredRenderingPath::LightingConstants, DeferredRenderingPath::SceneConstants>
    {
    public:
        DeferredWorldRenderer(const WorldRendererArgs &rArgs) : BaseWorldRenderer(rArgs) {}

        inline void SetSSAORadius(float radius)
        {
            mSSAOHighFrequencyPassConstants.radius = radius;
        }

        inline float GetSSAORadius() const
        {
            return mSSAOHighFrequencyPassConstants.radius;
        }

        inline void SetSSAODistanceScale(float distanceScale)
        {
            mSSAOHighFrequencyPassConstants.distanceScale = distanceScale;
        }

        inline float GetSSAODistanceScale() const
        {
            return mSSAOHighFrequencyPassConstants.distanceScale;
        }

        inline bool IsSSAOBlurEnabled() const
        {
            return mSSAOBlurEnabled;
        }

        inline void SetSSAOBlurEnabled(bool ssaoBlurEnabled)
        {
            mSSAOBlurEnabled = ssaoBlurEnabled;
        }

        void Initialize() override;
        void Render(const Camera *pCamera, RenderingContext *pRenderingContext) override;
        void Finalize() override;

    protected:
        void BindGBufferTextures(RenderingContext *pRenderingContext) const;
        void BindSSAOTexture(bool isSSAOEnabled, RenderingContext *pRenderingContext) const;
        void UpdateSceneConstants(const glm::mat4 &rView, const glm::mat4 &rProjection, float fov, RenderingContext *pRenderingContext);
        void UpdateLightingConstants(const PointLight *pPointLight, const glm::mat4 &rView, bool isSSAOEnabled, RenderingContext *pRenderingContext);
        void UpdateLightingConstants(const DirectionalLight *pDirectionalLight, const glm::vec3 &rDirection, bool isSSAOEnabled, RenderingContext *pRenderingContext);

    private:
        std::array<const Texture *, 7> mGBufferRenderTargets;
        std::array<const Texture *, 2> mSSAORenderTargets;
        const Shader *mpStencilPassShader{nullptr};
        const Shader *mpPointLightPassShader{nullptr};
        const Shader *mpDirectionalLightPassShader{nullptr};
        const Shader *mpSSAOHighFrequencyPassShader{nullptr};
        const Shader *mpSSAOBlurPassShader{nullptr};
        const Buffer *mpSSAOHighFrequencyPassConstantsBuffer{nullptr};
        SSAOHighFrequencyPassConstants mSSAOHighFrequencyPassConstants{};
        const Texture *mpNoiseTexture{nullptr};
        const Texture *mpEmptySSAOTexture{nullptr};
        const Mesh *mpQuadMesh{nullptr};
        const Mesh *mpSphereMesh{nullptr};
        bool mSSAOBlurEnabled{true};
    };

}

#endif