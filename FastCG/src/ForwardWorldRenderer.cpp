#include <FastCG/WorldSystem.h>
#include <FastCG/Renderable.h>
#include <FastCG/ForwardWorldRenderer.h>

#include <algorithm>

namespace
{
    void SetupMeshMainSubPass(FastCG::RenderingContext *pRenderingContext)
    {
        pRenderingContext->SetDepthFunc(FastCG::CompareOp::LEQUAL);
        pRenderingContext->SetBlend(false);
    }

    void SetupMeshSecondarySubPasses(FastCG::RenderingContext *pRenderingContext)
    {
        pRenderingContext->SetDepthFunc(FastCG::CompareOp::EQUAL);
        pRenderingContext->SetBlend(true);
        pRenderingContext->SetBlendFunc(FastCG::BlendFunc::ADD, FastCG::BlendFunc::ADD);
        pRenderingContext->SetBlendFactors(FastCG::BlendFactor::ONE, FastCG::BlendFactor::ONE, FastCG::BlendFactor::ONE, FastCG::BlendFactor::ONE);
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
        mRenderTargets[0] = RenderingSystem::GetInstance()->CreateTexture({"Color",
                                                                           mArgs.rScreenWidth,
                                                                           mArgs.rScreenHeight,
                                                                           TextureType::TEXTURE_2D,
                                                                           TextureFormat::RGBA,
                                                                           {10, 10, 10, 2},
                                                                           TextureDataType::FLOAT,
                                                                           TextureFilter::POINT_FILTER,
                                                                           TextureWrapMode::CLAMP,
                                                                           false});

        mRenderTargets[1] = RenderingSystem::GetInstance()->CreateTexture({"Depth",
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
                RenderingSystem::GetInstance()->DestroyTexture(pRenderTarget);
            }
        }
        mRenderTargets = {};
    }

    void ForwardWorldRenderer::Resize()
    {
        BaseWorldRenderer::Resize();

        if (RenderingSystem::GetInstance()->IsInitialized())
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

    void ForwardWorldRenderer::Render(const Camera *pCamera, RenderingContext *pRenderingContext)
    {
        assert(pRenderingContext != nullptr);

        if (pCamera == nullptr)
        {
            return;
        }

        auto view = pCamera->GetView();
        auto inverseView = glm::inverse(view);
        auto projection = pCamera->GetProjection();

        pRenderingContext->PushDebugMarker("Forward World Rendering");
        {
            GenerateShadowMaps(pRenderingContext);

            const auto nearClip = pCamera->GetNearClip();
            const auto isSSAOEnabled = pCamera->IsSSAOEnabled();

            if (isSSAOEnabled)
            {
                GenerateAmbientOcculusionMap(projection, pCamera->GetFieldOfView(), mRenderTargets[1], pRenderingContext);
            }

            pRenderingContext->SetViewport(0, 0, mArgs.rScreenWidth, mArgs.rScreenHeight);
            pRenderingContext->SetDepthTest(true);
            pRenderingContext->SetDepthWrite(true);
            pRenderingContext->SetStencilTest(false);
            pRenderingContext->SetScissorTest(false);
            pRenderingContext->SetCullMode(Face::BACK);
            pRenderingContext->SetBlend(false);
            pRenderingContext->SetRenderTargets(mRenderTargets.data(), mRenderTargets.size());

            pRenderingContext->PushDebugMarker("Clear Render Targets");
            {
                pRenderingContext->ClearRenderTarget(0, mArgs.rClearColor);
                pRenderingContext->ClearDepthStencilTarget(0, 1, 0);
            }
            pRenderingContext->PopDebugMarker();

            auto renderBatchesIt = mArgs.rRenderBatches.cbegin() + 1;
            if (renderBatchesIt != mArgs.rRenderBatches.cend())
            {
                pRenderingContext->PushDebugMarker("Material Passes");
                {
                    UpdateSceneConstants(view, inverseView, projection, pRenderingContext);

                    for (; renderBatchesIt != mArgs.rRenderBatches.cend(); ++renderBatchesIt)
                    {
                        assert(renderBatchesIt->type == RenderBatchType::MATERIAL_BASED);

                        const auto *pMaterial = renderBatchesIt->pMaterial;

                        pRenderingContext->PushDebugMarker((pMaterial->GetName() + " Pass").c_str());
                        {
                            SetupMaterial(pMaterial, pRenderingContext);

                            pRenderingContext->Bind(mpSceneConstantsBuffer, OpenGLShader::SCENE_CONSTANTS_BINDING_INDEX);

                            for (const auto *pRenderable : renderBatchesIt->renderables)
                            {
                                if (!pRenderable->GetGameObject()->IsActive())
                                {
                                    continue;
                                }

                                UpdateInstanceConstants(pRenderable->GetGameObject()->GetTransform()->GetModel(), view, projection, pRenderingContext);
                                pRenderingContext->Bind(mpInstanceConstantsBuffer, OpenGLShader::INSTANCE_CONSTANTS_BINDING_INDEX);

                                const auto *pMesh = pRenderable->GetMesh();

                                pRenderingContext->SetVertexBuffers(pMesh->GetVertexBuffers(), pMesh->GetVertexBufferCount());
                                pRenderingContext->SetIndexBuffer(pMesh->GetIndexBuffer());

                                const auto &rDirectionalLights = WorldSystem::GetInstance()->GetDirectionalLights();
                                const auto &rPointLights = WorldSystem::GetInstance()->GetPointLights();

                                if (rDirectionalLights.size() == 0 && rPointLights.size() == 0)
                                {
                                    pRenderingContext->DrawIndexed(PrimitiveType::TRIANGLES, pMesh->GetIndexCount(), 0, 0);

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
                                        pRenderingContext->PushDebugMarker((pMaterial->GetName() + " Directional Light Sub-Pass (" + std::to_string(i) + ")").c_str());
                                        {
                                            switch (lastSubpassType)
                                            {
                                            case SubpassType::ST_NONE:
                                                SetupMeshMainSubPass(pRenderingContext);
                                                break;
                                            case SubpassType::ST_MAIN:
                                                SetupMeshSecondarySubPasses(pRenderingContext);
                                                break;
                                            default:
                                                break;
                                            }

                                            const auto *pDirectionalLight = rDirectionalLights[i];

                                            auto directionalLightPosition = glm::normalize(pDirectionalLight->GetGameObject()->GetTransform()->GetWorldPosition());

                                            UpdateLightingConstants(pDirectionalLight, transposeView * directionalLightPosition, nearClip, isSSAOEnabled, pRenderingContext);
                                            pRenderingContext->Bind(mpLightingConstantsBuffer, OpenGLShader::LIGHTING_CONSTANTS_BINDING_INDEX);

                                            pRenderingContext->DrawIndexed(PrimitiveType::TRIANGLES, pMesh->GetIndexCount(), 0, 0);

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
                                        pRenderingContext->PopDebugMarker();
                                    }

                                    for (size_t i = 0; i < rPointLights.size(); i++)
                                    {
                                        pRenderingContext->PushDebugMarker((pMaterial->GetName() + " Point Light Sub-Pass (" + std::to_string(i) + ")").c_str());
                                        {
                                            switch (lastSubpassType)
                                            {
                                            case SubpassType::ST_NONE:
                                                SetupMeshMainSubPass(pRenderingContext);
                                                break;
                                            case SubpassType::ST_MAIN:
                                                SetupMeshSecondarySubPasses(pRenderingContext);
                                                break;
                                            default:
                                                break;
                                            }

                                            UpdateLightingConstants(rPointLights[i], inverseView, nearClip, isSSAOEnabled, pRenderingContext);

                                            pRenderingContext->Bind(mpLightingConstantsBuffer, OpenGLShader::LIGHTING_CONSTANTS_BINDING_INDEX);

                                            pRenderingContext->DrawIndexed(PrimitiveType::TRIANGLES, pMesh->GetIndexCount(), 0, 0);

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
                                        pRenderingContext->PopDebugMarker();
                                    }
                                    mArgs.rRenderingStatistics.triangles += pMesh->GetTriangleCount();
                                }
                            }
                        }
                        pRenderingContext->PopDebugMarker();
                    }
                }
                pRenderingContext->PopDebugMarker();
            }

            pRenderingContext->PushDebugMarker("Blit Color Render Target into Color Backbuffer");
            {
                pRenderingContext->Blit(mRenderTargets[0], RenderingSystem::GetInstance()->GetBackbuffer());
            }
            pRenderingContext->PopDebugMarker();
        }
        pRenderingContext->PopDebugMarker();
    }
}