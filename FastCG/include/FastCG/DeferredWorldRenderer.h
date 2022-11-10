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

        void Initialize() override;
        void Render(const Camera *pCamera, RenderingContext *pRenderingContext) override;
        void Finalize() override;

    protected:
        void BindGBufferTextures(RenderingContext *pRenderingContext) const;
        void UpdateLightingConstants(const PointLight *pPointLight, const glm::mat4 &rView, float near, bool isSSAOEnabled, RenderingContext *pRenderingContext) override;
        void UpdateLightingConstants(const DirectionalLight *pDirectionalLight, const glm::vec3 &rDirection, float near, bool isSSAOEnabled, RenderingContext *pRenderingContext) override;

    private:
        std::array<const Texture *, 7> mGBufferRenderTargets;
        const Shader *mpStencilPassShader{nullptr};
        const Shader *mpPointLightPassShader{nullptr};
        const Shader *mpDirectionalLightPassShader{nullptr};
        const Mesh *mpSphereMesh{nullptr};
    };

}

#endif