#include <FastCG/WorldSystem.h>
#include <FastCG/Rendering/StandardGeometries.h>
#include <FastCG/Rendering/DeferredWorldRenderer.h>
#include <FastCG/MathT.h>

namespace
{
    float CalculateLightBoundingSphereScale(const FastCG::PointLight *pPointLight)
    {
        auto uDiffuseColor = pPointLight->GetDiffuseColor();
        auto maxChannel = FastCG::MathF::Max(FastCG::MathF::Max(uDiffuseColor.r, uDiffuseColor.g), uDiffuseColor.b);
        auto c = maxChannel * pPointLight->GetIntensity();
        return (8.0f * FastCG::MathF::Sqrt(c) + 1.0f);
    }

}

namespace FastCG
{
    constexpr uint32_t LIGHT_MESH_DETAIL = 20;

    void DeferredWorldRenderer::Initialize()
    {
        BaseWorldRenderer::Initialize();

        CreateGBufferRenderTargets();

        mpStencilPassShader = GraphicsSystem::GetInstance()->FindShader("StencilPass");
        mpDirectionalLightPassShader = GraphicsSystem::GetInstance()->FindShader("DirectionalLightPass");
        mpPointLightPassShader = GraphicsSystem::GetInstance()->FindShader("PointLightPass");

        mpSphereMesh = StandardGeometries::CreateSphere("Deferred Rendering PointLight Sphere", 1, LIGHT_MESH_DETAIL);
    }

    void DeferredWorldRenderer::CreateGBufferRenderTargets()
    {
        mGBufferRenderTargets[0] = GraphicsSystem::GetInstance()->CreateTexture({"G-Buffer Diffuse",
                                                                                 mArgs.rScreenWidth,
                                                                                 mArgs.rScreenHeight,
                                                                                 TextureType::TEXTURE_2D,
                                                                                 TextureFormat::RGBA,
                                                                                 {8, 8, 8, 8},
                                                                                 TextureDataType::FLOAT,
                                                                                 TextureFilter::LINEAR_FILTER,
                                                                                 TextureWrapMode::CLAMP,
                                                                                 false});
        mGBufferRenderTargets[1] = GraphicsSystem::GetInstance()->CreateTexture({"G-Buffer Normal",
                                                                                 mArgs.rScreenWidth,
                                                                                 mArgs.rScreenHeight,
                                                                                 TextureType::TEXTURE_2D,
                                                                                 TextureFormat::RGB,
                                                                                 {8, 8, 8, 8},
                                                                                 TextureDataType::FLOAT,
                                                                                 TextureFilter::LINEAR_FILTER,
                                                                                 TextureWrapMode::CLAMP,
                                                                                 false});
        mGBufferRenderTargets[2] = GraphicsSystem::GetInstance()->CreateTexture({"G-Buffer Specular",
                                                                                 mArgs.rScreenWidth,
                                                                                 mArgs.rScreenHeight,
                                                                                 TextureType::TEXTURE_2D,
                                                                                 TextureFormat::RGBA,
                                                                                 {8, 8, 8, 8},
                                                                                 TextureDataType::FLOAT,
                                                                                 TextureFilter::LINEAR_FILTER,
                                                                                 TextureWrapMode::CLAMP,
                                                                                 false});
        mGBufferRenderTargets[3] = GraphicsSystem::GetInstance()->CreateTexture({"G-Buffer Tangent",
                                                                                 mArgs.rScreenWidth,
                                                                                 mArgs.rScreenHeight,
                                                                                 TextureType::TEXTURE_2D,
                                                                                 TextureFormat::RGBA,
                                                                                 {8, 8, 8, 8},
                                                                                 TextureDataType::FLOAT,
                                                                                 TextureFilter::LINEAR_FILTER,
                                                                                 TextureWrapMode::CLAMP,
                                                                                 false});
        mGBufferRenderTargets[4] = GraphicsSystem::GetInstance()->CreateTexture({"G-Buffer Extra Data",
                                                                                 mArgs.rScreenWidth,
                                                                                 mArgs.rScreenHeight,
                                                                                 TextureType::TEXTURE_2D,
                                                                                 TextureFormat::RGBA,
                                                                                 {8, 8, 8, 8},
                                                                                 TextureDataType::FLOAT,
                                                                                 TextureFilter::LINEAR_FILTER,
                                                                                 TextureWrapMode::CLAMP,
                                                                                 false});
        mGBufferRenderTargets[5] = GraphicsSystem::GetInstance()->CreateTexture({"G-Buffer Depth",
                                                                                 mArgs.rScreenWidth,
                                                                                 mArgs.rScreenHeight,
                                                                                 TextureType::TEXTURE_2D,
                                                                                 TextureFormat::DEPTH_STENCIL,
                                                                                 {24, 8},
                                                                                 TextureDataType::UNSIGNED_INT,
                                                                                 TextureFilter::POINT_FILTER,
                                                                                 TextureWrapMode::CLAMP,
                                                                                 false});
        mGBufferRenderTargets[6] = GraphicsSystem::GetInstance()->CreateTexture({"G-Buffer Final",
                                                                                 mArgs.rScreenWidth,
                                                                                 mArgs.rScreenHeight,
                                                                                 TextureType::TEXTURE_2D,
                                                                                 TextureFormat::RGBA,
                                                                                 {10, 10, 10, 2},
                                                                                 TextureDataType::FLOAT,
                                                                                 TextureFilter::POINT_FILTER,
                                                                                 TextureWrapMode::CLAMP,
                                                                                 false});
    }

    void DeferredWorldRenderer::DestroyGBufferRenderTargets()
    {
        for (const auto *pRenderTarget : mGBufferRenderTargets)
        {
            if (pRenderTarget != nullptr)
            {
                GraphicsSystem::GetInstance()->DestroyTexture(pRenderTarget);
            }
        }
        mGBufferRenderTargets = {};
    }

    void DeferredWorldRenderer::BindGBufferTextures(GraphicsContext *pGraphicsContext) const
    {
        pGraphicsContext->BindResource(mGBufferRenderTargets[0], "uDiffuseMap", 4);
        pGraphicsContext->BindResource(mGBufferRenderTargets[1], "uNormalMap", 5);
        pGraphicsContext->BindResource(mGBufferRenderTargets[2], "uSpecularMap", 6);
        pGraphicsContext->BindResource(mGBufferRenderTargets[3], "uTangentMap", 7);
        pGraphicsContext->BindResource(mGBufferRenderTargets[4], "uExtraData", 8);
        pGraphicsContext->BindResource(mGBufferRenderTargets[5], "uDepth", 9);
    }

    void DeferredWorldRenderer::UpdateLightingConstants(const PointLight *pPointLight, const glm::mat4 &rInverseView, float nearClip, bool isSSAOEnabled, GraphicsContext *pGraphicsContext)
    {
        BaseWorldRenderer::UpdateLightingConstants(pPointLight, rInverseView, nearClip, isSSAOEnabled, pGraphicsContext);
        BindGBufferTextures(pGraphicsContext);
    }

    void DeferredWorldRenderer::UpdateLightingConstants(const DirectionalLight *pDirectionalLight, const glm::vec3 &rViewDirection, float nearClip, bool isSSAOEnabled, GraphicsContext *pGraphicsContext)
    {
        BaseWorldRenderer::UpdateLightingConstants(pDirectionalLight, rViewDirection, nearClip, isSSAOEnabled, pGraphicsContext);
        BindGBufferTextures(pGraphicsContext);
    }

    void DeferredWorldRenderer::Resize()
    {
        BaseWorldRenderer::Resize();

        if (GraphicsSystem::GetInstance()->IsInitialized())
        {
            DestroyGBufferRenderTargets();
            CreateGBufferRenderTargets();
        }
    }

    void DeferredWorldRenderer::Render(const Camera *pCamera, GraphicsContext *pGraphicsContext)
    {
        assert(pGraphicsContext != nullptr);

        if (pCamera == nullptr)
        {
            return;
        }

        auto view = pCamera->GetView();
        auto inverseView = glm::inverse(view);
        auto projection = pCamera->GetProjection();

        pGraphicsContext->PushDebugMarker("Deferred World Rendering");
        {
            GenerateShadowMaps(pGraphicsContext);

            const auto nearClip = pCamera->GetNearClip();
            const auto isSSAOEnabled = pCamera->IsSSAOEnabled();

            if (isSSAOEnabled)
            {
                GenerateAmbientOcculusionMap(projection, pCamera->GetFieldOfView(), mGBufferRenderTargets[5], pGraphicsContext);
            }

            pGraphicsContext->SetViewport(0, 0, mArgs.rScreenWidth, mArgs.rScreenHeight);
            pGraphicsContext->SetDepthTest(true);
            pGraphicsContext->SetDepthWrite(true);
            pGraphicsContext->SetStencilTest(false);
            pGraphicsContext->SetStencilWriteMask(Face::FRONT_AND_BACK, 0);
            pGraphicsContext->SetScissorTest(false);
            pGraphicsContext->SetCullMode(Face::BACK);
            pGraphicsContext->SetBlend(false);
            pGraphicsContext->SetRenderTargets(mGBufferRenderTargets.data(), 6);

            pGraphicsContext->PushDebugMarker("Clear G-Buffer");
            {
                for (auto i : {0, 1, 2, 3, 4})
                {
                    pGraphicsContext->ClearRenderTarget(i, Colors::NONE);
                }
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
                        const auto *pMaterial = renderBatchIt->pMaterial;

                        pGraphicsContext->PushDebugMarker((pMaterial->GetName() + " Pass").c_str());
                        {
                            assert(renderBatchIt->type != RenderBatchType::SHADOW_CASTERS);

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

                                if (instanceCount == 0)
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
                        }
                        pGraphicsContext->PopDebugMarker();
                    }
                }
                pGraphicsContext->PopDebugMarker();
            }

            pGraphicsContext->PushDebugMarker("Clear G-Buffer Final Render Target");
            {
                pGraphicsContext->SetRenderTargets(&mGBufferRenderTargets[6], 1);
                pGraphicsContext->ClearRenderTarget(0, Colors::NONE);
            }
            pGraphicsContext->PopDebugMarker();

            pGraphicsContext->PushDebugMarker("Point Light Passes");
            {
                const auto &rPointLights = WorldSystem::GetInstance()->GetPointLights();
                for (size_t i = 0; i < rPointLights.size(); i++)
                {
                    const auto *pPointLight = rPointLights[i];

                    pGraphicsContext->PushDebugMarker((std::string("Point Light Pass (") + std::to_string(i) + ")").c_str());
                    {
                        auto model = glm::scale(pPointLight->GetGameObject()->GetTransform()->GetModel(), glm::vec3(CalculateLightBoundingSphereScale(pPointLight)));

                        UpdateInstanceConstants(model, view, projection, pGraphicsContext);

                        pGraphicsContext->PushDebugMarker((std::string("Point Light (") + std::to_string(i) + ") Stencil Sub-Pass").c_str());
                        {
                            pGraphicsContext->SetRenderTargets(&mGBufferRenderTargets[5], 1);
                            pGraphicsContext->ClearStencilTarget(0, 0);

                            pGraphicsContext->SetStencilWriteMask(Face::FRONT_AND_BACK, 0xff);
                            pGraphicsContext->SetDepthTest(true);
                            pGraphicsContext->SetDepthWrite(false);
                            pGraphicsContext->SetStencilTest(true);
                            pGraphicsContext->SetStencilFunc(Face::FRONT_AND_BACK, CompareOp::ALWAYS, 0, 0);
                            pGraphicsContext->SetStencilOp(Face::BACK, StencilOp::KEEP, StencilOp::INCREMENT_AND_CLAMP, StencilOp::KEEP);
                            pGraphicsContext->SetStencilOp(Face::FRONT, StencilOp::KEEP, StencilOp::DECREMENT_AND_CLAMP, StencilOp::KEEP);
                            pGraphicsContext->SetBlend(false);
                            pGraphicsContext->SetCullMode(Face::NONE);

                            pGraphicsContext->BindShader(mpStencilPassShader);

                            pGraphicsContext->BindResource(mpInstanceConstantsBuffer, INSTANCE_CONSTANTS_SHADER_RESOURCE_INDEX);

                            pGraphicsContext->SetVertexBuffers(mpSphereMesh->GetVertexBuffers(), mpSphereMesh->GetVertexBufferCount());
                            pGraphicsContext->SetIndexBuffer(mpSphereMesh->GetIndexBuffer());

                            pGraphicsContext->DrawIndexed(PrimitiveType::TRIANGLES, 0, mpSphereMesh->GetIndexCount(), 0);

                            mArgs.rRenderingStatistics.drawCalls++;
                        }
                        pGraphicsContext->PopDebugMarker();

                        pGraphicsContext->PushDebugMarker((std::string("Point Light (") + std::to_string(i) + ") Color Sub-Pass").c_str());
                        {
                            pGraphicsContext->SetRenderTargets(&mGBufferRenderTargets[5], 2);

                            pGraphicsContext->SetDepthTest(false);
                            pGraphicsContext->SetDepthWrite(false);
                            pGraphicsContext->SetStencilTest(true);
                            pGraphicsContext->SetStencilWriteMask(Face::FRONT_AND_BACK, 0);
                            pGraphicsContext->SetStencilFunc(Face::FRONT_AND_BACK, CompareOp::NOT_EQUAL, 0, 0xff);
                            pGraphicsContext->SetBlend(true);
                            pGraphicsContext->SetBlendFunc(BlendFunc::ADD, BlendFunc::NONE);
                            pGraphicsContext->SetBlendFactors(BlendFactor::ONE, BlendFactor::ONE, BlendFactor::ZERO, BlendFactor::ZERO);
                            pGraphicsContext->SetCullMode(Face::FRONT);

                            pGraphicsContext->BindShader(mpPointLightPassShader);

                            pGraphicsContext->BindResource(mpSceneConstantsBuffer, SCENE_CONSTANTS_SHADER_RESOURCE_INDEX);
                            UpdateInstanceConstants(model, view, projection, pGraphicsContext);

                            pGraphicsContext->BindResource(mpInstanceConstantsBuffer, INSTANCE_CONSTANTS_SHADER_RESOURCE_INDEX);
                            UpdateLightingConstants(pPointLight, inverseView, nearClip, isSSAOEnabled, pGraphicsContext);

                            pGraphicsContext->BindResource(mpLightingConstantsBuffer, LIGHTING_CONSTANTS_SHADER_RESOURCE_INDEX);

                            pGraphicsContext->SetVertexBuffers(mpSphereMesh->GetVertexBuffers(), mpSphereMesh->GetVertexBufferCount());
                            pGraphicsContext->SetIndexBuffer(mpSphereMesh->GetIndexBuffer());

                            pGraphicsContext->DrawIndexed(PrimitiveType::TRIANGLES, 0, mpSphereMesh->GetIndexCount(), 0);

                            mArgs.rRenderingStatistics.drawCalls++;
                        }
                        pGraphicsContext->PopDebugMarker();
                    }
                    pGraphicsContext->PopDebugMarker();
                }
            }
            pGraphicsContext->PopDebugMarker();

            pGraphicsContext->PushDebugMarker("Directional Light Passes");
            {
                pGraphicsContext->SetRenderTargets(&mGBufferRenderTargets[6], 1);

                pGraphicsContext->SetDepthTest(false);
                pGraphicsContext->SetDepthWrite(false);
                pGraphicsContext->SetStencilTest(false);
                pGraphicsContext->SetStencilWriteMask(Face::FRONT_AND_BACK, 0);
                pGraphicsContext->SetBlend(true);
                pGraphicsContext->SetBlendFunc(BlendFunc::ADD, BlendFunc::NONE);
                pGraphicsContext->SetBlendFactors(BlendFactor::ONE, BlendFactor::ONE, BlendFactor::ZERO, BlendFactor::ZERO);
                pGraphicsContext->SetCullMode(Face::BACK);

                pGraphicsContext->BindShader(mpDirectionalLightPassShader);

                auto viewTranspose = glm::transpose(glm::toMat3(pCamera->GetGameObject()->GetTransform()->GetWorldRotation()));
                const auto &rDirectionalLights = WorldSystem::GetInstance()->GetDirectionalLights();
                for (size_t i = 0; i < rDirectionalLights.size(); i++)
                {
                    const auto *pDirectionalLight = rDirectionalLights[i];
                    pGraphicsContext->PushDebugMarker((std::string("Directional Light Pass (") + std::to_string(i) + ")").c_str());
                    {
                        UpdateLightingConstants(pDirectionalLight, glm::normalize(viewTranspose * pDirectionalLight->GetDirection()), nearClip, isSSAOEnabled, pGraphicsContext);
                        pGraphicsContext->BindResource(mpLightingConstantsBuffer, LIGHTING_CONSTANTS_SHADER_RESOURCE_INDEX);

                        pGraphicsContext->SetVertexBuffers(mpQuadMesh->GetVertexBuffers(), mpQuadMesh->GetVertexBufferCount());
                        pGraphicsContext->SetIndexBuffer(mpQuadMesh->GetIndexBuffer());

                        pGraphicsContext->DrawIndexed(PrimitiveType::TRIANGLES, 0, mpQuadMesh->GetIndexCount(), 0);

                        mArgs.rRenderingStatistics.drawCalls++;
                    }
                    pGraphicsContext->PopDebugMarker();
                }
            }
            pGraphicsContext->PopDebugMarker();

            pGraphicsContext->PushDebugMarker("Blit G-Buffer Final Render Target into Color Backbuffer");
            {
                pGraphicsContext->Blit(mGBufferRenderTargets[6], GraphicsSystem::GetInstance()->GetBackbuffer());
            }
            pGraphicsContext->PopDebugMarker();
        }
        pGraphicsContext->PopDebugMarker();
    }

    void DeferredWorldRenderer::Finalize()
    {
        mpSphereMesh = nullptr;

        DestroyGBufferRenderTargets();

        BaseWorldRenderer::Finalize();
    }
}