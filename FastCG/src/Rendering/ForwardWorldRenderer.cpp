#include <FastCG/Rendering/ForwardWorldRenderer.h>
#include <FastCG/Rendering/Renderable.h>
#include <FastCG/World/WorldSystem.h>

#include <algorithm>

namespace
{
    void SetupMeshSecondarySubPasses(FastCG::GraphicsContext *pGraphicsContext)
    {
        pGraphicsContext->SetDepthFunc(FastCG::CompareOp::EQUAL);
        pGraphicsContext->SetBlend(true);
        pGraphicsContext->SetBlendFunc(FastCG::BlendFunc::ADD, FastCG::BlendFunc::ADD);
        pGraphicsContext->SetBlendFactors(FastCG::BlendFactor::ONE, FastCG::BlendFactor::ONE, FastCG::BlendFactor::ONE,
                                          FastCG::BlendFactor::ONE);
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
        for (size_t i = 0; i < mRenderTargets.size(); ++i)
        {
            mRenderTargets[i] = GraphicsSystem::GetInstance()->CreateTexture(
                {"Color Buffer " + std::to_string(i), mArgs.rScreenWidth, mArgs.rScreenHeight, 1, 1,
                 TextureType::TEXTURE_2D, TextureUsageFlagBit::SAMPLED | TextureUsageFlagBit::RENDER_TARGET,
                 TextureFormat::R8G8B8A8_UNORM, TextureFilter::LINEAR_FILTER, TextureWrapMode::CLAMP});
        }

        mDepthStencilBuffers.resize(GraphicsSystem::GetInstance()->GetMaxSimultaneousFrames());
        for (size_t i = 0; i < mDepthStencilBuffers.size(); ++i)
        {
            mDepthStencilBuffers[i] = GraphicsSystem::GetInstance()->CreateTexture(
                {"Depth Buffer " + std::to_string(i), mArgs.rScreenWidth, mArgs.rScreenHeight, 1, 1,
                 TextureType::TEXTURE_2D, TextureUsageFlagBit::SAMPLED | TextureUsageFlagBit::RENDER_TARGET,
                 TextureFormat::D24_UNORM_S8_UINT, TextureFilter::POINT_FILTER, TextureWrapMode::CLAMP});
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
        pGraphicsContext->PushDebugMarker("Forward World Rendering");
        {
            const auto *pCurrentRenderTarget = mRenderTargets[GraphicsSystem::GetInstance()->GetCurrentFrame()];
            const auto *pCurrentDepthStencilBuffer =
                mDepthStencilBuffers[GraphicsSystem::GetInstance()->GetCurrentFrame()];

            glm::mat4 projection;
            auto isSSAOEnabled = false;

            if (pCamera != nullptr)
            {
                projection = pCamera->GetProjection();
                isSSAOEnabled = pCamera->IsSSAOEnabled();

                if (isSSAOEnabled)
                {
                    // use depth from mMaxSimultaneousFrames ago
                    GenerateAmbientOcculusionMap(projection, pCamera->GetFieldOfView(), pCurrentDepthStencilBuffer,
                                                 pGraphicsContext);
                }
            }

            pGraphicsContext->SetViewport(0, 0, pCurrentRenderTarget->GetWidth(), pCurrentRenderTarget->GetHeight());
            pGraphicsContext->SetRenderTargets(&pCurrentRenderTarget, 1, pCurrentDepthStencilBuffer);

            pGraphicsContext->PushDebugMarker("Clear Render Targets");
            {
                pGraphicsContext->ClearRenderTarget(0, mArgs.rClearColor);
                pGraphicsContext->ClearDepthBuffer(1);
            }
            pGraphicsContext->PopDebugMarker();

            if (pCamera != nullptr)
            {
                GenerateShadowMaps(pGraphicsContext);

                const auto view = pCamera->GetView();
                const auto inverseView = glm::inverse(view);
                const auto nearClip = pCamera->GetNearClip();

                pGraphicsContext->SetViewport(0, 0, pCurrentRenderTarget->GetWidth(),
                                              pCurrentRenderTarget->GetHeight());
                pGraphicsContext->SetDepthTest(true);
                pGraphicsContext->SetDepthFunc(CompareOp::LESS);
                pGraphicsContext->SetDepthWrite(true);
                pGraphicsContext->SetStencilTest(false);
                pGraphicsContext->SetScissorTest(false);
                pGraphicsContext->SetCullMode(Face::BACK);
                pGraphicsContext->SetBlend(false);
                pGraphicsContext->SetRenderTargets(&pCurrentRenderTarget, 1, pCurrentDepthStencilBuffer);

                const auto *pSceneConstantsBuffer =
                    UpdateSceneConstants(view, inverseView, projection, pGraphicsContext);

                auto ProcessMaterialPasses = [&](const auto &rFirstRenderBatchIt, const auto &rLastRenderBatchIt) {
                    const auto *pFogConstantsBuffer =
                        UpdateFogConstants(WorldSystem::GetInstance()->GetFog(), pGraphicsContext);

                    for (auto renderBatchIt = rFirstRenderBatchIt; renderBatchIt != rLastRenderBatchIt; ++renderBatchIt)
                    {
                        const auto &rpMaterial = renderBatchIt->pMaterial;

                        pGraphicsContext->PushDebugMarker((rpMaterial->GetName() + " Pass").c_str());
                        {
                            BindMaterial(rpMaterial, pGraphicsContext);

                            pGraphicsContext->BindResource(pSceneConstantsBuffer, SCENE_CONSTANTS_SHADER_RESOURCE_NAME);
                            pGraphicsContext->BindResource(pFogConstantsBuffer, FOG_CONSTANTS_SHADER_RESOURCE_NAME);

                            UpdateSSAOConstants(isSSAOEnabled, pGraphicsContext);

                            for (auto it = renderBatchIt->renderablesPerMesh.cbegin();
                                 it != renderBatchIt->renderablesPerMesh.cend(); ++it)
                            {
                                SetGraphicsContextState(rpMaterial->GetGraphicsContextState(), pGraphicsContext);

                                const auto &rpMesh = it->first;
                                const auto &rRenderables = it->second;

                                uint32_t instanceCount;
                                const Buffer *pInstanceConstantsBuffer;
                                {
                                    auto result =
                                        UpdateInstanceConstants(rRenderables, view, projection, pGraphicsContext);
                                    instanceCount = result.first;
                                    pInstanceConstantsBuffer = result.second;
                                }

                                if (instanceCount == 0)
                                {
                                    continue;
                                }

                                pGraphicsContext->BindResource(pInstanceConstantsBuffer,
                                                               INSTANCE_CONSTANTS_SHADER_RESOURCE_NAME);

                                pGraphicsContext->SetVertexBuffers(rpMesh->GetVertexBuffers(),
                                                                   rpMesh->GetVertexBufferCount());
                                pGraphicsContext->SetIndexBuffer(rpMesh->GetIndexBuffer());

                                const auto &rDirectionalLights = WorldSystem::GetInstance()->GetDirectionalLights();
                                const auto &rPointLights = WorldSystem::GetInstance()->GetPointLights();

                                if (rDirectionalLights.size() == 0 && rPointLights.size() == 0)
                                {
                                    const auto *pLightingConstantsBuffer = EmptyLightingConstants(pGraphicsContext);
                                    pGraphicsContext->BindResource(pLightingConstantsBuffer,
                                                                   LIGHTING_CONSTANTS_SHADER_RESOURCE_NAME);
                                    const auto *pPCSSConstantsBuffer = EmptyPCSSConstants(pGraphicsContext);
                                    pGraphicsContext->BindResource(pPCSSConstantsBuffer,
                                                                   PCSS_CONSTANTS_SHADER_RESOURCE_NAME);

                                    if (instanceCount == 1)
                                    {
                                        pGraphicsContext->DrawIndexed(PrimitiveType::TRIANGLES, 0,
                                                                      rpMesh->GetIndexCount(), 0);
                                    }
                                    else
                                    {
                                        pGraphicsContext->DrawInstancedIndexed(
                                            PrimitiveType::TRIANGLES, 0, instanceCount, 0, rpMesh->GetIndexCount(), 0);
                                    }

                                    mArgs.rRenderingStatistics.drawCalls++;
                                    mArgs.rRenderingStatistics.triangles += rpMesh->GetTriangleCount();
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

                                    for (size_t i = 0; i < rDirectionalLights.size(); i++)
                                    {
                                        pGraphicsContext->PushDebugMarker((rpMaterial->GetName() +
                                                                           " Directional Light Sub-Pass (" +
                                                                           std::to_string(i) + ")")
                                                                              .c_str());
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

                                            const auto *pLightingConstantsBuffer = UpdateLightingConstants(
                                                pDirectionalLight, pDirectionalLight->GetDirection(), pGraphicsContext);
                                            pGraphicsContext->BindResource(pLightingConstantsBuffer,
                                                                           LIGHTING_CONSTANTS_SHADER_RESOURCE_NAME);
                                            const auto *pPCSSConstantsBuffer =
                                                UpdatePCSSConstants(pDirectionalLight, nearClip, pGraphicsContext);
                                            pGraphicsContext->BindResource(pPCSSConstantsBuffer,
                                                                           PCSS_CONSTANTS_SHADER_RESOURCE_NAME);

                                            if (instanceCount == 1)
                                            {
                                                pGraphicsContext->DrawIndexed(PrimitiveType::TRIANGLES, 0,
                                                                              rpMesh->GetIndexCount(), 0);
                                            }
                                            else
                                            {
                                                pGraphicsContext->DrawInstancedIndexed(PrimitiveType::TRIANGLES, 0,
                                                                                       instanceCount, 0,
                                                                                       rpMesh->GetIndexCount(), 0);
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
                                        pGraphicsContext->PushDebugMarker((rpMaterial->GetName() +
                                                                           " Point Light Sub-Pass (" +
                                                                           std::to_string(i) + ")")
                                                                              .c_str());
                                        {
                                            switch (lastSubpassType)
                                            {
                                            case SubpassType::ST_MAIN:
                                                SetupMeshSecondarySubPasses(pGraphicsContext);
                                                break;
                                            default:
                                                break;
                                            }

                                            const auto *pLightingConstantsBuffer =
                                                UpdateLightingConstants(rPointLights[i], view, pGraphicsContext);
                                            pGraphicsContext->BindResource(pLightingConstantsBuffer,
                                                                           LIGHTING_CONSTANTS_SHADER_RESOURCE_NAME);
                                            const auto *pPCSSConstantsBuffer =
                                                UpdatePCSSConstants(rPointLights[i], nearClip, pGraphicsContext);
                                            pGraphicsContext->BindResource(pPCSSConstantsBuffer,
                                                                           PCSS_CONSTANTS_SHADER_RESOURCE_NAME);

                                            if (instanceCount == 1)
                                            {
                                                pGraphicsContext->DrawIndexed(PrimitiveType::TRIANGLES, 0,
                                                                              rpMesh->GetIndexCount(), 0);
                                            }
                                            else
                                            {
                                                pGraphicsContext->DrawInstancedIndexed(PrimitiveType::TRIANGLES, 0,
                                                                                       instanceCount, 0,
                                                                                       rpMesh->GetIndexCount(), 0);
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
                                    mArgs.rRenderingStatistics.triangles += rpMesh->GetTriangleCount();
                                }
                            }
                        }
                        pGraphicsContext->PopDebugMarker();
                    }
                };

                auto opaqueRenderBatchesIt = mArgs.rRenderBatchStrategy.GetFirstOpaqueMaterialRenderBatchIterator();
                auto transparentRenderBatchesIt =
                    mArgs.rRenderBatchStrategy.GetFirstTransparentMaterialRenderBatchIterator();
                if (opaqueRenderBatchesIt != transparentRenderBatchesIt)
                {
                    pGraphicsContext->PushDebugMarker("Opaque Material Passes");
                    {
                        ProcessMaterialPasses(opaqueRenderBatchesIt, transparentRenderBatchesIt);
                    }
                    pGraphicsContext->PopDebugMarker();
                }

                RenderSkybox(pCurrentRenderTarget, pCurrentDepthStencilBuffer, pSceneConstantsBuffer, view, projection,
                             pGraphicsContext);

                auto lastRenderBatchIt = mArgs.rRenderBatchStrategy.GetLastRenderBatchIterator();
                if (transparentRenderBatchesIt != lastRenderBatchIt)
                {
                    pGraphicsContext->PushDebugMarker("Transparent Material Passes");
                    {
                        ProcessMaterialPasses(transparentRenderBatchesIt, lastRenderBatchIt);
                    }
                    pGraphicsContext->PopDebugMarker();
                }
            }

            if (mArgs.hdr)
            {
                Tonemap(pCurrentRenderTarget, GraphicsSystem::GetInstance()->GetBackbuffer(), pGraphicsContext);
            }
            else
            {
                pGraphicsContext->PushDebugMarker("Blit Color Render Target into Backbuffer");
                {
                    pGraphicsContext->Blit(pCurrentRenderTarget, GraphicsSystem::GetInstance()->GetBackbuffer());
                }
            }
            pGraphicsContext->PopDebugMarker();
        }
        pGraphicsContext->PopDebugMarker();
    }
}