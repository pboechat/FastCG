#ifndef FASTCG_DEFERRED_WORLD_RENDERER_H
#define FASTCG_DEFERRED_WORLD_RENDERER_H

#include <FastCG/Rendering/BaseWorldRenderer.h>
#include <FastCG/Rendering/ShaderConstants.h>

#include <array>
#include <memory>
#include <vector>

namespace FastCG
{
    class DeferredWorldRenderer
        : public BaseWorldRenderer<DeferredRenderingPath::InstanceConstants, DeferredRenderingPath::LightingConstants,
                                   DeferredRenderingPath::SceneConstants>
    {
    public:
        DeferredWorldRenderer(const WorldRendererArgs &rArgs) : BaseWorldRenderer(rArgs)
        {
        }

        void Initialize() override;
        void Resize() override;
        void Finalize() override;

    protected:
        void OnRender(const Camera *pCamera, GraphicsContext *pGraphicsContext) override;
        void CreateGBuffers();
        void DestroyGBuffers();
        void BindGBuffer(GraphicsContext *pGraphicsContext) const;
        const Buffer *UpdateLightingConstants(const PointLight *pPointLight,
                                              GraphicsContext *pGraphicsContext) override;
        const Buffer *UpdateLightingConstants(const DirectionalLight *pDirectionalLight,
                                              const glm::vec3 &rViewDirection,
                                              GraphicsContext *pGraphicsContext) override;

    private:
        std::vector<std::array<const Texture *, 6>> mGBuffers;
        std::vector<const Texture *> mDepthStencilBuffers;
        const Shader *mpStencilPassShader{nullptr};
        const Shader *mpPointLightPassShader{nullptr};
        const Shader *mpDirectionalLightPassShader{nullptr};
        std::unique_ptr<Mesh> mpSphereMesh{nullptr};
    };

}

#endif