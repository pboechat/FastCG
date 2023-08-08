#include <FastCG/World/WorldSystem.h>
#include <FastCG/Rendering/Renderable.h>
#include <FastCG/Rendering/ForwardWorldRenderer.h>

#include <algorithm>

namespace
{
    void SetupMeshSecondarySubPasses(FastCG::GraphicsContext *pGraphicsContext)
    {
        pGraphicsContext->SetDepthFunc(FastCG::CompareOp::EQUAL);
        pGraphicsContext->SetBlend(true);
        pGraphicsContext->SetBlendFunc(FastCG::BlendFunc::ADD, FastCG::BlendFunc::ADD);
        pGraphicsContext->SetBlendFactors(FastCG::BlendFactor::ONE, FastCG::BlendFactor::ONE, FastCG::BlendFactor::ONE, FastCG::BlendFactor::ONE);
    }

}

namespace FastCG
{
    void ForwardWorldRenderer::Initialize()
    {
        BaseWorldRenderer::Initialize();

        CreateRenderTargets();
    }

    void ForwardWorldRenderer::CreateRenderTargets()
    {
        mRenderTargets.resize(GraphicsSystem::GetInstance()->GetMaxSimultaneousFrames());
        for (auto *&pFrameRenderTarget : mRenderTargets)
        {
            pFrameRenderTarget = GraphicsSystem::GetInstance()->CreateTexture({"Color",
                                                                               mArgs.rScreenWidth,
                                                                               mArgs.rScreenHeight,
                                                                               TextureType::TEXTURE_2D,
                                                                               TextureUsageFlagBit::SAMPLED | TextureUsageFlagBit::RENDER_TARGET,
                                                                               TextureFormat::RGBA,
                                                                               {10, 10, 10, 2},
                                                                               TextureDataType::FLOAT,
                                                                               TextureFilter::LINEAR_FILTER,
                                                                               TextureWrapMode::CLAMP,
                                                                               false});
        }

        mDepthStencilBuffers.resize(GraphicsSystem::GetInstance()->GetMaxSimultaneousFrames());
        for (auto *&pFrameDepthStencilTarget : mDepthStencilBuffers)
        {
            pFrameDepthStencilTarget = GraphicsSystem::GetInstance()->CreateTexture({"Depth",
                                                                                     mArgs.rScreenWidth,
                                                                                     mArgs.rScreenHeight,
                                                                                     TextureType::TEXTURE_2D,
                                                                                     TextureUsageFlagBit::RENDER_TARGET,
                                                                                     TextureFormat::DEPTH_STENCIL,
                                                                                     {24, 8},
                                                                                     TextureDataType::UNSIGNED_INT,
                                                                                     TextureFilter::POINT_FILTER,
                                                                                     TextureWrapMode::CLAMP,
                                                                                     false});
        }
    }

    void ForwardWorldRenderer::DestroyRenderTargets()
    {
        for (auto *pRenderTarget : mRenderTargets)
        {
            GraphicsSystem::GetInstance()->DestroyTexture(pRenderTarget);
        }
        mRenderTargets.clear();

        for (auto *pDepthStencilTarget : mDepthStencilBuffers)
        {
            GraphicsSystem::GetInstance()->DestroyTexture(pDepthStencilTarget);
        }
        mDepthStencilBuffers.clear();
    }

    void ForwardWorldRenderer::Resize()
    {
        BaseWorldRenderer::Resize();

        if (GraphicsSystem::GetInstance()->IsInitialized())
        {
            DestroyRenderTargets();
            CreateRenderTargets();
        }
    }

    void ForwardWorldRenderer::Finalize()
    {
        DestroyRenderTargets();

        BaseWorldRenderer::Finalize();
    }

    void ForwardWorldRenderer::OnRender(const Camera *pCamera, GraphicsContext *pGraphicsContext)
    {
        auto view = pCamera->GetView();
        auto inverseView = glm::inverse(view);
        auto projection = pCamera->GetProjection();

        auto *pCurrentRenderTarget = mRenderTargets[GraphicsSystem::GetInstance()->GetCurrentFrame()];
        auto *pCurrentDepthStencilBuffer = mDepthStencilBuffers[GraphicsSystem::GetInstance()->GetCurrentFrame()];

        pGraphicsContext->PushDebugMarker("Forward World Rendering");
        {
            GenerateShadowMaps(pGraphicsContext);

            const auto nearClip = pCamera->GetNearClip();
            const auto isSSAOEnabled = pCamera->IsSSAOEnabled();

            if (isSSAOEnabled)
            {
                GenerateAmbientOcculusionMap(projection, pCamera->GetFieldOfView(), pCurrentDepthStencilBuffer, pGraphicsContext);
            }

            pGraphicsContext->SetViewport(0, 0, pCurrentRenderTarget->GetWidth(), pCurrentRenderTarget->GetHeight());
            pGraphicsContext->SetDepthTest(true);
            pGraphicsContext->SetDepthWrite(true);
            pGraphicsContext->SetStencilTest(false);
            pGraphicsContext->SetScissorTest(false);
            pGraphicsContext->SetCullMode(Face::BACK);
            pGraphicsContext->SetBlend(false);
            pGraphicsContext->SetRenderTargets(&pCurrentRenderTarget, 1, pCurrentDepthStencilBuffer);

            pGraphicsContext->PushDebugMarker("Clear Render Targets");
            {
                pGraphicsContext->ClearRenderTarget(0, mArgs.rClearColor);
                pGraphicsContext->ClearDepthStencilBuffer(1, 0);
            }
            pGraphicsContext->PopDebugMarker();

            auto renderBatchIt = mArgs.rRenderBatches.cbegin() + 1;
            if (renderBatchIt != mArgs.rRenderBatches.cend())
            {
                pGraphicsContext->PushDebugMarker("Material Passes");
                {
                    const auto *pSceneConstantsBuffer = UpdateSceneConstants(view, inverseView, projection, pGraphicsContext);

                    for (; renderBatchIt != mArgs.rRenderBatches.cend(); ++renderBatchIt)
                    {
                        assert(renderBatchIt->type != RenderBatchType::SHADOW_CASTERS);

                        const auto *pMaterial = renderBatchIt->pMaterial;

                        pGraphicsContext->PushDebugMarker((pMaterial->GetName() + " Pass").c_str());
                        {
                            BindMaterial(pMaterial, pGraphicsContext);

                            pGraphicsContext->BindResource(pSceneConstantsBuffer, SCENE_CONSTANTS_SHADER_RESOURCE_NAME);

                            for (auto it = renderBatchIt->renderablesPerMesh.cbegin(); it != renderBatchIt->renderablesPerMesh.cend(); ++it)
                            {
                                const auto *pMesh = it->first;
                                const auto &rRenderables = it->second;

                                uint32_t instanceCount;
                                const Buffer *pInstanceConstantsBuffer;
                                {
                                    auto result = UpdateInstanceConstants(rRenderables, view, projection, pGraphicsContext);
                                    instanceCount = result.first;
                                    pInstanceConstantsBuffer = result.second;
                                }

                                if (instanceCount == 0)
                                {
                                    continue;
                                }

                                pGraphicsContext->BindResource(pInstanceConstantsBuffer, INSTANCE_CONSTANTS_SHADER_RESOURCE_NAME);

                                pGraphicsContext->SetVertexBuffers(pMesh->GetVertexBuffers(), pMesh->GetVertexBufferCount());
                                pGraphicsContext->SetIndexBuffer(pMesh->GetIndexBuffer());

                                const auto &rDirectionalLights = WorldSystem::GetInstance()->GetDirectionalLights();
                                const auto &rPointLights = WorldSystem::GetInstance()->GetPointLights();

                                if (rDirectionalLights.size() == 0 && rPointLights.size() == 0)
                                {
                                    if (instanceCount == 1)
                                    {
                                        pGraphicsContext->DrawIndexed(PrimitiveType::TRIANGLES, 0, pMesh->GetIndexCount(), 0);
                                    }
                                    else
                                    {
                                        pGraphicsContext->DrawInstancedIndexed(PrimitiveType::TRIANGLES, 0, instanceCount, 0, pMesh->GetIndexCount(), 0);
                                    }

                                    mArgs.rRenderingStatistics.drawCalls++;
                                    mArgs.rRenderingStatistics.triangles += pMesh->GetTriangleCount();
                                }
                                else
                                {
                                    enum class SubpassType : uint8_t
                                    {
                                        ST_NONE = 0,
                                        ST_MAIN,
                                        ST_SECONDARY
                                    };

                                    SubpassType lastSubpassType{SubpassType::ST_NONE};

                                    auto transposeView = glm::transpose(glm::toMat3(pCamera->GetGameObject()->GetTransform()->GetWorldRotation()));

                                    for (size_t i = 0; i < rDirectionalLights.size(); i++)
                                    {
                                        pGraphicsContext->PushDebugMarker((pMaterial->GetName() + " Directional Light Sub-Pass (" + std::to_string(i) + ")").c_str());
                                        {
                                            switch (lastSubpassType)
                                            {
                                            case SubpassType::ST_MAIN:
                                                SetupMeshSecondarySubPasses(pGraphicsContext);
                                                break;
                                            default:
                                                break;
                                            }

                                            const auto *pDirectionalLight = rDirectionalLights[i];

                                            auto directionalLightPosition = glm::normalize(pDirectionalLight->GetGameObject()->GetTransform()->GetWorldPosition());

                                            const auto *pLightingConstantsBuffer = UpdateLightingConstants(pDirectionalLight, transposeView * directionalLightPosition, nearClip, isSSAOEnabled, pGraphicsContext);
                                            pGraphicsContext->BindResource(pLightingConstantsBuffer, LIGHTING_CONSTANTS_SHADER_RESOURCE_NAME);

                                            if (instanceCount == 1)
                                            {
                                                pGraphicsContext->DrawIndexed(PrimitiveType::TRIANGLES, 0, pMesh->GetIndexCount(), 0);
                                            }
                                            else
                                            {
                                                pGraphicsContext->DrawInstancedIndexed(PrimitiveType::TRIANGLES, 0, instanceCount, 0, pMesh->GetIndexCount(), 0);
                                            }

                                            mArgs.rRenderingStatistics.drawCalls++;

                                            switch (lastSubpassType)
                                            {
                                            case SubpassType::ST_NONE:
                                                lastSubpassType = SubpassType::ST_MAIN;
                                                break;
                                            case SubpassType::ST_MAIN:
                                                lastSubpassType = SubpassType::ST_SECONDARY;
                                                break;
                                            default:
                                                break;
                                            }
                                        }
                                        pGraphicsContext->PopDebugMarker();
                                    }

                                    for (size_t i = 0; i < rPointLights.size(); i++)
                                    {
                                        pGraphicsContext->PushDebugMarker((pMaterial->GetName() + " Point Light Sub-Pass (" + std::to_string(i) + ")").c_str());
                                        {
                                            switch (lastSubpassType)
                                            {
                                            case SubpassType::ST_MAIN:
                                                SetupMeshSecondarySubPasses(pGraphicsContext);
                                                break;
                                            default:
                                                break;
                                            }

                                            const auto *pLightingConstantsBuffer = UpdateLightingConstants(rPointLights[i], inverseView, nearClip, isSSAOEnabled, pGraphicsContext);
                                            pGraphicsContext->BindResource(pLightingConstantsBuffer, LIGHTING_CONSTANTS_SHADER_RESOURCE_NAME);

                                            if (instanceCount == 1)
                                            {
                                                pGraphicsContext->DrawIndexed(PrimitiveType::TRIANGLES, 0, pMesh->GetIndexCount(), 0);
                                            }
                                            else
                                            {
                                                pGraphicsContext->DrawInstancedIndexed(PrimitiveType::TRIANGLES, 0, instanceCount, 0, pMesh->GetIndexCount(), 0);
                                            }

                                            mArgs.rRenderingStatistics.drawCalls++;

                                            switch (lastSubpassType)
                                            {
                                            case SubpassType::ST_NONE:
                                                lastSubpassType = SubpassType::ST_MAIN;
                                                break;
                                            case SubpassType::ST_MAIN:
                                                lastSubpassType = SubpassType::ST_SECONDARY;
                                                break;
                                            default:
                                                break;
                                            }
                                        }
                                        pGraphicsContext->PopDebugMarker();
                                    }
                                    mArgs.rRenderingStatistics.triangles += pMesh->GetTriangleCount();
                                }
                            }
                        }
                        pGraphicsContext->PopDebugMarker();
                    }
                }
                pGraphicsContext->PopDebugMarker();
            }

            pGraphicsContext->PushDebugMarker("Blit Color Render Target into Backbuffer");
            {
                pGraphicsContext->Blit(pCurrentRenderTarget, GraphicsSystem::GetInstance()->GetBackbuffer());
            }
            pGraphicsContext->PopDebugMarker();
        }
        pGraphicsContext->PopDebugMarker();
    }
}