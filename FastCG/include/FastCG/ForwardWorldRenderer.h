#ifndef FASTCG_FORWARD_WORLD_RENDERER_H
#define FASTCG_FORWARD_WORLD_RENDERER_H

#include <FastCG/ShaderConstants.h>
#include <FastCG/BaseWorldRenderer.h>

#include <array>

namespace FastCG
{
    class ForwardWorldRenderer : public BaseWorldRenderer<ForwardRenderingPath::InstanceConstants, ForwardRenderingPath::LightingConstants, ForwardRenderingPath::SceneConstants>
    {
    public:
        ForwardWorldRenderer(const WorldRendererArgs &rArgs) : BaseWorldRenderer(rArgs) {}

        void Initialize() override;
        void Resize() override;
        void Render(const Camera *pCamera, RenderingContext *pRenderingContext) override;
        void Finalize() override;

    private:
        std::array<const Texture *, 2> mRenderTargets{};

        void CreateRenderTargets();
        void DestroyRenderTargets();
    };

}

#endif