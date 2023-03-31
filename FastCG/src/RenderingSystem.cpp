#include <FastCG/WorldSystem.h>
#include <FastCG/RenderingSystem.h>
#include <FastCG/RenderBatchStrategy.h>
#include <FastCG/ImGuiRenderer.h>
#include <FastCG/ForwardWorldRenderer.h>
#include <FastCG/DeferredWorldRenderer.h>

#include <imgui.h>

namespace FastCG
{
    RenderingSystem::RenderingSystem(const RenderingSystemArgs &rArgs) : mArgs(rArgs),
                                                                         mpRenderBatchStrategy(std::make_unique<RenderBatchStrategy>())
    {
    }

    RenderingSystem::~RenderingSystem() = default;

    void RenderingSystem::Initialize()
    {
        switch (mArgs.renderingPath)
        {
        case RenderingPath::FORWARD:
            mpWorldRenderer = std::unique_ptr<IWorldRenderer>(new ForwardWorldRenderer({mArgs.rScreenWidth,
                                                                                        mArgs.rScreenHeight,
                                                                                        mArgs.rClearColor,
                                                                                        mArgs.rAmbientLight,
                                                                                        mpRenderBatchStrategy->GetRenderBatches(),
                                                                                        mArgs.rRenderingStatistics}));
            break;
        case RenderingPath::DEFERRED:
            mpWorldRenderer = std::unique_ptr<IWorldRenderer>(new DeferredWorldRenderer({mArgs.rScreenWidth,
                                                                                         mArgs.rScreenHeight,
                                                                                         mArgs.rClearColor,
                                                                                         mArgs.rAmbientLight,
                                                                                         mpRenderBatchStrategy->GetRenderBatches(),
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

        mpRenderingContext = GraphicsSystem::GetInstance()->CreateRenderingContext({"Default Rendering Context"});
    }

    void RenderingSystem::RegisterRenderable(const Renderable *pRenderable)
    {
        assert(pRenderable != nullptr);
        mpRenderBatchStrategy->AddRenderable(pRenderable);
    }

    void RenderingSystem::UnregisterRenderable(const Renderable *pRenderable)
    {
        assert(pRenderable != nullptr);
        mpRenderBatchStrategy->RemoveRenderable(pRenderable);
    }

    void RenderingSystem::Render()
    {
        assert(mpWorldRenderer != nullptr);

        auto *pMainCamera = WorldSystem::GetInstance()->GetMainCamera();
        ImGui::Render();
        auto *pImDrawData = ImGui::GetDrawData();

        mpRenderingContext->Begin();
        {
            mpWorldRenderer->Render(pMainCamera, mpRenderingContext);
            mpImGuiRenderer->Render(pImDrawData, mpRenderingContext);
        }
        mpRenderingContext->End();
    }

    void RenderingSystem::Finalize()
    {
        GraphicsSystem::GetInstance()->DestroyRenderingContext(mpRenderingContext);

        mpImGuiRenderer->Finalize();
        mpWorldRenderer->Finalize();
    }
}