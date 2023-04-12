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
        mRenderTargets[0] = GraphicsSystem::GetInstance()->CreateTexture({"Color",
                                                                          mArgs.rScreenWidth,
                                                                          mArgs.rScreenHeight,
                                                                          TextureType::TEXTURE_2D,
                                                                          TextureFormat::RGBA,
                                                                          {10, 10, 10, 2},
                                                                          TextureDataType::FLOAT,
                                                                          TextureFilter::POINT_FILTER,
                                                                          TextureWrapMode::CLAMP,
                                                                          false});

        mRenderTargets[1] = GraphicsSystem::GetInstance()->CreateTexture({"Depth",
                                                                          mArgs.rScreenWidth,
                                                                          mArgs.rScreenHeight,
                                                                          TextureType::TEXTURE_2D,
                                                                          TextureFormat::DEPTH_STENCIL,
                                                                          {24, 8},
                                                                          TextureDataType::UNSIGNED_INT,
                                                                          TextureFilter::POINT_FILTER,
                                                                          TextureWrapMode::CLAMP,
                                                                          false});
    }

    void ForwardWorldRenderer::DestroyRenderTargets()
    {
        for (auto *pRenderTarget : mRenderTargets)
        {
            if (pRenderTarget != nullptr)
            {
                GraphicsSystem::GetInstance()->DestroyTexture(pRenderTarget);
            }
        }
        mRenderTargets = {};
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

    void ForwardWorldRenderer::Render(const Camera *pCamera, GraphicsContext *pGraphicsContext)
    {
        assert(pGraphicsContext != nullptr);

        if (pCamera == nullptr)
        {
            return;
        }

        auto view = pCamera->GetView();
        auto inverseView = glm::inverse(view);
        auto projection = pCamera->GetProjection();

        pGraphicsContext->PushDebugMarker("Forward World Rendering");
        {
            GenerateShadowMaps(pGraphicsContext);

            const auto nearClip = pCamera->GetNearClip();
            const auto isSSAOEnabled = pCamera->IsSSAOEnabled();

            if (isSSAOEnabled)
            {
                GenerateAmbientOcculusionMap(projection, pCamera->GetFieldOfView(), mRenderTargets[1], pGraphicsContext);
            }

            pGraphicsContext->SetViewport(0, 0, mArgs.rScreenWidth, mArgs.rScreenHeight);
            pGraphicsContext->SetDepthTest(true);
            pGraphicsContext->SetDepthWrite(true);
            pGraphicsContext->SetStencilTest(false);
            pGraphicsContext->SetScissorTest(false);
            pGraphicsContext->SetCullMode(Face::BACK);
            pGraphicsContext->SetBlend(false);
            pGraphicsContext->SetRenderTargets(mRenderTargets.data(), mRenderTargets.size());

            pGraphicsContext->PushDebugMarker("Clear Render Targets");
            {
                pGraphicsContext->ClearRenderTarget(0, mArgs.rClearColor);
                pGraphicsContext->ClearDepthStencilTarget(0, 1, 0);
            }
            pGraphicsContext->PopDebugMarker();

            auto renderBatchIt = mArgs.rRenderBatches.cbegin() + 1;
            if (renderBatchIt != mArgs.rRenderBatches.cend())
            {
                pGraphicsContext->PushDebugMarker("Material Passes");
                {
                    UpdateSceneConstants(view, inverseView, projection, pGraphicsContext);

                    for (; renderBatchIt != mArgs.rRenderBatches.cend(); ++renderBatchIt)
                    {
                        assert(renderBatchIt->type != RenderBatchType::SHADOW_CASTERS);

                        const auto *pMaterial = renderBatchIt->pMaterial;

                        pGraphicsContext->PushDebugMarker((pMaterial->GetName() + " Pass").c_str());
                        {
                            BindMaterial(pMaterial, pGraphicsContext);

                            pGraphicsContext->BindResource(mpSceneConstantsBuffer, SCENE_CONSTANTS_SHADER_RESOURCE_INDEX);

                            for (auto it = renderBatchIt->renderablesPerMesh.cbegin(); it != renderBatchIt->renderablesPerMesh.cend(); ++it)
                            {
                                const auto *pMesh = it->first;
                                const auto &rRenderables = it->second;

                                auto instanceCount = UpdateInstanceConstants(rRenderables, view, projection, pGraphicsContext);
                                if (instanceCount == 0)
                                {
                                    continue;
                                }

                                pGraphicsContext->BindResource(mpInstanceConstantsBuffer, INSTANCE_CONSTANTS_SHADER_RESOURCE_INDEX);

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

                                            UpdateLightingConstants(pDirectionalLight, transposeView * directionalLightPosition, nearClip, isSSAOEnabled, pGraphicsContext);
                                            pGraphicsContext->BindResource(mpLightingConstantsBuffer, LIGHTING_CONSTANTS_SHADER_RESOURCE_INDEX);

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

                                            UpdateLightingConstants(rPointLights[i], inverseView, nearClip, isSSAOEnabled, pGraphicsContext);
                                            pGraphicsContext->BindResource(mpLightingConstantsBuffer, LIGHTING_CONSTANTS_SHADER_RESOURCE_INDEX);

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

            pGraphicsContext->PushDebugMarker("Blit Color Render Target into Color Backbuffer");
            {
                pGraphicsContext->Blit(mRenderTargets[0], GraphicsSystem::GetInstance()->GetBackbuffer());
            }
            pGraphicsContext->PopDebugMarker();
        }
        pGraphicsContext->PopDebugMarker();
    }
}