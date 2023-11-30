#include <FastCG/World/WorldSystem.h>
#include <FastCG/UI/ImGuiRenderer.h>
#include <FastCG/Rendering/RenderingSystem.h>
#include <FastCG/Rendering/ForwardWorldRenderer.h>
#include <FastCG/Rendering/DeferredWorldRenderer.h>
#include <FastCG/Core/Log.h>

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
            mpWorldRenderer = std::unique_ptr<IWorldRenderer>(new ForwardWorldRenderer({mArgs.rScreenWidth,
                                                                                        mArgs.rScreenHeight,
                                                                                        mArgs.rClearColor,
                                                                                        mArgs.rAmbientLight,
                                                                                        mRenderBatchStrategy,
                                                                                        mArgs.rRenderingStatistics}));
            break;
        case RenderingPath::DEFERRED:
            mpWorldRenderer = std::unique_ptr<IWorldRenderer>(new DeferredWorldRenderer({mArgs.rScreenWidth,
                                                                                         mArgs.rScreenHeight,
                                                                                         mArgs.rClearColor,
                                                                                         mArgs.rAmbientLight,
                                                                                         mRenderBatchStrategy,
                                                                                         mArgs.rRenderingStatistics}));
            break;
        default:
            FASTCG_THROW_EXCEPTION(Exception, "Unhandled rendering path: %d", (int)mArgs.renderingPath);
            break;
        }
        mpWorldRenderer->Initialize();

        mpImGuiRenderer = std::make_unique<ImGuiRenderer>(ImGuiRendererArgs{mArgs.rScreenWidth,
                                                                            mArgs.rScreenHeight});

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

        auto *pMainCamera = WorldSystem::GetInstance()->GetMainCamera();
        ImGui::Render();
        auto *pImDrawData = ImGui::GetDrawData();

        if (!GraphicsSystem::GetInstance()->IsHeadless())
        {
            mpGraphicsContext->Begin();
            {
                mpWorldRenderer->Render(pMainCamera, mpGraphicsContext);
                mpImGuiRenderer->Render(pImDrawData, mpGraphicsContext);
            }
            mpGraphicsContext->End();
        }
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