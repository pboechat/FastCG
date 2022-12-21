#ifndef FASTCG_RENDERING_SYSTEM_H
#define FASTCG_RENDERING_SYSTEM_H

#include <FastCG/System.h>
#include <FastCG/RenderingStatistics.h>
#include <FastCG/RenderingPath.h>
#include <FastCG/IWorldRenderer.h>
#include <FastCG/GraphicsSystem.h>

#include <memory>
#include <cstdint>

namespace FastCG
{
    class RenderBatchStrategy;
    class ImGuiRenderer;

    struct RenderingSystemArgs
    {
        RenderingPath renderingPath;
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
        std::unique_ptr<RenderBatchStrategy> mpRenderBatchStrategy;
        std::unique_ptr<IWorldRenderer> mpWorldRenderer;
        std::unique_ptr<ImGuiRenderer> mpImGuiRenderer;
        RenderingContext *mpRenderingContext;

        RenderingSystem(const RenderingSystemArgs &rArgs);
        ~RenderingSystem();

        void Initialize();
        void Render();
        void Finalize();
    };

}

#endif