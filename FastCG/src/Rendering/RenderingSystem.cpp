#include <FastCG/Core/Log.h>
#include <FastCG/Rendering/DeferredWorldRenderer.h>
#include <FastCG/Rendering/ForwardWorldRenderer.h>
#include <FastCG/Rendering/RenderingSystem.h>
#include <FastCG/UI/ImGuiRenderer.h>
#include <FastCG/World/WorldSystem.h>

#include <imgui.h>

namespace FastCG
{
    RenderingSystem::RenderingSystem(const RenderingSystemArgs &rArgs) : mArgs(rArgs)
    {
    }

    RenderingSystem::~RenderingSystem() = default;

    void RenderingSystem::Initialize()
    {
        FASTCG_LOG_DEBUG(RenderingSystem, "\tInitializing rendering system");

        switch (mArgs.renderingPath)
        {
        case RenderingPath::FORWARD:
            mpWorldRenderer = std::unique_ptr<IWorldRenderer>(
                new ForwardWorldRenderer({mArgs.hdr, mArgs.rScreenWidth, mArgs.rScreenHeight, mArgs.rClearColor,
                                          mArgs.rAmbientLight, mRenderBatchStrategy, mArgs.rRenderingStatistics}));
            break;
        case RenderingPath::DEFERRED:
            mpWorldRenderer = std::unique_ptr<IWorldRenderer>(
                new DeferredWorldRenderer({mArgs.hdr, mArgs.rScreenWidth, mArgs.rScreenHeight, mArgs.rClearColor,
                                           mArgs.rAmbientLight, mRenderBatchStrategy, mArgs.rRenderingStatistics}));
            break;
        default:
            FASTCG_THROW_EXCEPTION(Exception, "Unhandled rendering path: %d", (int)mArgs.renderingPath);
            break;
        }
        mpWorldRenderer->Initialize();

        mpImGuiRenderer = std::make_unique<ImGuiRenderer>(ImGuiRendererArgs{mArgs.rScreenWidth, mArgs.rScreenHeight});

        mpImGuiRenderer->Initialize();

        mpGraphicsContext = GraphicsSystem::GetInstance()->CreateGraphicsContext({"Default Rendering Context"});
    }

    void RenderingSystem::RegisterRenderable(const Renderable *pRenderable)
    {
        assert(pRenderable != nullptr);
        mRenderBatchStrategy.AddRenderable(pRenderable);
    }

    void RenderingSystem::UnregisterRenderable(const Renderable *pRenderable)
    {
        assert(pRenderable != nullptr);
        mRenderBatchStrategy.RemoveRenderable(pRenderable);
    }

    void RenderingSystem::Render()
    {
        assert(mpWorldRenderer != nullptr);

        ImGui::Render();

        mpGraphicsContext->Begin();
        {
            mpWorldRenderer->Render(WorldSystem::GetInstance()->GetMainCamera(), mpGraphicsContext);
            mpImGuiRenderer->Render(ImGui::GetDrawData(), mpGraphicsContext);
        }
        mpGraphicsContext->End();
    }

    void RenderingSystem::Resize()
    {
        mpWorldRenderer->Resize();
    }

    void RenderingSystem::Finalize()
    {
        GraphicsSystem::GetInstance()->DestroyGraphicsContext(mpGraphicsContext);

        mpImGuiRenderer->Finalize();
        mpWorldRenderer->Finalize();
    }
}