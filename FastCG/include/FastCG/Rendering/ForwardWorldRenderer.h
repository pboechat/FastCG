#ifndef FASTCG_FORWARD_WORLD_RENDERER_H
#define FASTCG_FORWARD_WORLD_RENDERER_H

#include <FastCG/Rendering/ShaderConstants.h>
#include <FastCG/Rendering/BaseWorldRenderer.h>
#include <FastCG/Graphics/GraphicsSystem.h>

#include <vector>
#include <array>

namespace FastCG
{
    class ForwardWorldRenderer : public BaseWorldRenderer<ForwardRenderingPath::InstanceConstants, ForwardRenderingPath::LightingConstants, ForwardRenderingPath::SceneConstants>
    {
    public:
        ForwardWorldRenderer(const WorldRendererArgs &rArgs) : BaseWorldRenderer(rArgs) {}

        void Initialize() override;
        void Resize() override;
        void Finalize() override;

    protected:
        void OnRender(const Camera *pCamera, GraphicsContext *pGraphicsContext) override;

    private:
        std::vector<const Texture *> mRenderTargets;
        std::vector<const Texture *> mDepthStencilBuffers;
        void CreateRenderTargets();
        void DestroyRenderTargets();
    };

}

#endif