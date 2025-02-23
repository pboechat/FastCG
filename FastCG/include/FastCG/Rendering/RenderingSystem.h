#ifndef FASTCG_RENDERING_SYSTEM_H
#define FASTCG_RENDERING_SYSTEM_H

#include <FastCG/Core/System.h>
#include <FastCG/Graphics/GraphicsSystem.h>
#include <FastCG/Graphics/RenderingPath.h>
#include <FastCG/Rendering/IWorldRenderer.h>
#include <FastCG/Rendering/RenderBatchStrategy.h>
#include <FastCG/Rendering/RenderingStatistics.h>

#include <cstdint>
#include <memory>

namespace FastCG
{
    class ImGuiRenderer;

    struct RenderingSystemArgs
    {
        RenderingPath renderingPath;
        bool hdr;
        const uint32_t &rScreenWidth;
        const uint32_t &rScreenHeight;
        const glm::vec4 &rClearColor;
        const glm::vec4 &rAmbientLight;
        RenderingStatistics &rRenderingStatistics;
    };

    class RenderingSystem
    {
        FASTCG_DECLARE_SYSTEM(RenderingSystem, RenderingSystemArgs);

    public:
        inline const IWorldRenderer *GetWorldRenderer() const
        {
            return const_cast<RenderingSystem *>(this)->GetWorldRenderer();
        }

        inline IWorldRenderer *GetWorldRenderer()
        {
            return mpWorldRenderer.get();
        }

        void RegisterRenderable(const Renderable *pRenderable);
        void UnregisterRenderable(const Renderable *pRenderable);

    private:
        const RenderingSystemArgs mArgs;
        RenderBatchStrategy mRenderBatchStrategy;
        std::unique_ptr<IWorldRenderer> mpWorldRenderer;
        std::unique_ptr<ImGuiRenderer> mpImGuiRenderer;
        GraphicsContext *mpGraphicsContext;

        RenderingSystem(const RenderingSystemArgs &rArgs);
        ~RenderingSystem();

        void Initialize();
        void Render();
        void Resize();
        void Finalize();
    };

}

#endif